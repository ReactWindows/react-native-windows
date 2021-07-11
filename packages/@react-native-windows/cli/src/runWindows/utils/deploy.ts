/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {spawn, execSync, SpawnOptions} from 'child_process';
import fs from 'fs';
import http from 'http';
import path from 'path';
import glob from 'glob';
import parse from 'xml-parser';
import WinAppDeployTool from './winappdeploytool';
import {
  newInfo,
  newSuccess,
  newError,
  newWarn,
  newSpinner,
  commandWithProgress,
  runPowerShellScriptFunction,
} from './commandWithProgress';
import * as build from './build';
import {BuildConfig, RunWindowsOptions} from '../runWindowsOptions';
import MSBuildTools from './msbuildtools';
import {Config} from '@react-native-community/cli-types';
import {WindowsProjectConfig} from '../../config/projectConfig';
import {CodedError} from '@react-native-windows/telemetry';
import Version from './version';

function pushd(pathArg: string): () => void {
  const cwd = process.cwd();
  process.chdir(pathArg);
  return () => process.chdir(cwd);
}

export function getBuildConfiguration(options: RunWindowsOptions): BuildConfig {
  return options.release
    ? options.bundle
      ? 'ReleaseBundle'
      : 'Release'
    : options.bundle
    ? 'DebugBundle'
    : 'Debug';
}

function shouldLaunchApp(options: RunWindowsOptions): boolean {
  return options.launch;
}

function getAppPackage(
  options: RunWindowsOptions,
  projectName?: string,
): string {
  const configuration = getBuildConfiguration(options);
  const packageFolder =
    options.arch === 'x86'
      ? `{*_x86_${configuration}_*,*_Win32_${configuration}_*}`
      : `*_${options.arch}_${configuration}_*`;

  const appPackageGlob = `${options.root}/windows/{*/AppPackages,AppPackages/*}/${packageFolder}`;
  const appPackageCandidates = glob.sync(appPackageGlob);
  let appPackage;
  if (appPackageCandidates.length === 1 || !projectName) {
    appPackage = appPackageCandidates[0];
  } else if (appPackageCandidates.length > 1) {
    const filteredAppPackageCandidates = appPackageCandidates.filter(x =>
      x.includes(projectName),
    );
    if (filteredAppPackageCandidates.length >= 1) {
      appPackage = filteredAppPackageCandidates[0];
    }
  }

  if (!appPackage && options.release) {
    // in the latest vs, Release is removed
    newWarn(
      'No package found in *_Release_* folder, removing the _Release_ prefix and checking again',
    );

    const rootGlob = `${options.root}/windows/{*/AppPackages,AppPackages/*}`;
    const newGlob = `${rootGlob}/*_${
      options.arch === 'x86' ? '{Win32,x86}' : options.arch
    }_Test`;

    const result = glob.sync(newGlob);
    if (result.length > 1 && projectName) {
      const newFilteredGlobs = result.filter(x => x.includes(projectName));
      if (newFilteredGlobs.length >= 1) {
        newWarn(`More than one app package found: ${result}`);
      }
      appPackage = newFilteredGlobs[0];
    } else if (result.length === 1) {
      // we're good
      appPackage = result[0];
    }
  }

  if (!appPackage) {
    throw new CodedError(
      'NoAppPackage',
      `Unable to find app package using search path: "${appPackageGlob}"`,
    );
  }
  return appPackage;
}

function getWindowsStoreAppUtils(options: RunWindowsOptions) {
  const popd = pushd(options.root);
  const windowsStoreAppUtilsPath = path.resolve(
    __dirname,
    '..',
    '..',
    '..',
    'powershell',
    'WindowsStoreAppUtils.ps1',
  );
  execSync(`powershell -NoProfile Unblock-File "${windowsStoreAppUtilsPath}"`);
  popd();
  return windowsStoreAppUtilsPath;
}

function getAppxManifestPath(
  options: RunWindowsOptions,
  projectName?: string,
): string {
  const configuration = getBuildConfiguration(options);
  // C++ x86 manifest would go under windows/Debug whereas x64 goes under windows/x64/Debug
  // If we've built both, this causes us to end up with two matches, so we have to carefully select the right folder
  let archFolder;
  if (options.arch !== 'x86') {
    archFolder = `${options.arch}/${configuration}`;
  } else {
    archFolder = `${configuration}`;
  }

  const appxManifestGlob = `windows/{*/bin/${options.arch}/${configuration},${archFolder}/*,target/${options.arch}/${configuration}}/AppxManifest.xml`;
  const globs = glob.sync(path.join(options.root, appxManifestGlob));
  let appxPath: string;
  if (globs.length === 1 || !projectName) {
    appxPath = globs[0];
  } else {
    const filteredGlobs = globs.filter(x => x.includes(projectName));
    appxPath = filteredGlobs[0];
    if (filteredGlobs.length > 1) {
      newWarn(
        `More than one appxmanifest for ${projectName}: ${filteredGlobs.join(
          ',',
        )}`,
      );
      newWarn(`Choosing ${appxPath}`);
    }
  }

  if (!appxPath) {
    throw new CodedError(
      'NoAppxManifest',
      `Unable to find AppxManifest from "${options.root}", using search path: "${appxManifestGlob}" `,
    );
  }
  return appxPath;
}

function parseAppxManifest(appxManifestPath: string): parse.Document {
  return parse(fs.readFileSync(appxManifestPath, 'utf8'));
}

function getAppxManifest(options: RunWindowsOptions): parse.Document {
  return parseAppxManifest(getAppxManifestPath(options, undefined));
}

function handleResponseError(e: Error): never {
  if (e.message.indexOf('Error code -2146233088')) {
    throw new CodedError(
      'NoDevice',
      `No Windows Mobile device was detected: ${e.message}`,
    );
  } else {
    throw new CodedError(
      'AppDidNotDeploy',
      `Unexpected error deploying app: ${e.message}`,
    );
  }
}

// Errors: 0x80073d10 - bad architecture
export async function deployToDevice(
  options: RunWindowsOptions,
  verbose: boolean,
) {
  const appPackageFolder = getAppPackage(options);

  const deployTarget = options.target
    ? options.target
    : options.emulator
    ? 'emulator'
    : 'device';
  const deployTool = new WinAppDeployTool();
  const appxManifest = getAppxManifest(options);
  const shouldLaunch = shouldLaunchApp(options);
  const identity = appxManifest.root.children.filter(x => {
    return x.name === 'mp:PhoneIdentity';
  })[0];
  const appName = identity.attributes.PhoneProductId;

  const device = deployTool.findDevice(deployTarget);

  try {
    await deployTool.uninstallAppPackage(appName, device, verbose);
  } catch (e) {
    newWarn('Failed to uninstall app from ' + device.name);
  }

  const appxFile = glob.sync(path.join(appPackageFolder, '*.appx'))[0];
  try {
    await deployTool.installAppPackage(
      appxFile,
      device,
      shouldLaunch,
      false,
      verbose,
    );
  } catch (e) {
    if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
      await deployTool.installAppPackage(
        appxFile,
        device,
        shouldLaunch,
        true,
        verbose,
      );
    } else {
      handleResponseError(e);
    }
  }
}

export async function deployToDesktop(
  options: RunWindowsOptions,
  verbose: boolean,
  config: Config,
  buildTools: MSBuildTools,
) {
  const windowsConfig: Partial<WindowsProjectConfig> | undefined =
    config.project.windows;
  const slnFile =
    windowsConfig && windowsConfig.solutionFile && windowsConfig.sourceDir
      ? path.join(windowsConfig.sourceDir, windowsConfig.solutionFile)
      : options.sln!;
  const projectName =
    windowsConfig && windowsConfig.project && windowsConfig.project.projectName
      ? windowsConfig.project.projectName
      : path.parse(options.proj!).name;
  const windowsStoreAppUtils = getWindowsStoreAppUtils(options);
  const appxManifestPath = getAppxManifestPath(options, projectName);
  const appxManifest = parseAppxManifest(appxManifestPath);
  const identity = appxManifest.root.children.filter(x => {
    return x.name === 'Identity';
  })[0];
  const appName = identity.attributes.Name;

  const vsVersion = Version.fromString(buildTools.installationVersion);

  const args = [];
  if (options.remoteDebugging) {
    args.push('--remote-debugging');
  }

  if (options.directDebugging) {
    args.push('--direct-debugging', options.directDebugging.toString());
  }

  await runPowerShellScriptFunction(
    'Enabling Developer Mode',
    windowsStoreAppUtils,
    'EnableDevMode',
    verbose,
    'EnableDevModeFailure',
  );

  const appPackageFolder = getAppPackage(options, projectName);

  if (options.release && !options.deployFromLayout) {
    await runPowerShellScriptFunction(
      'Removing old version of the app',
      windowsStoreAppUtils,
      `Uninstall-App ${appName}`,
      verbose,
      'RemoveOldAppVersionFailure',
    );

    const script = glob.sync(
      path.join(appPackageFolder, 'Add-AppDevPackage.ps1'),
    )[0];

    await runPowerShellScriptFunction(
      'Installing new version of the app',
      windowsStoreAppUtils,
      `Install-App "${script}" -Force`,
      verbose,
      'InstallAppFailure',
    );
  } else {
    // If we have DeployAppRecipe.exe, use it (start in 16.8.4, earlier 16.8 versions have bugs)

    // const configWindows = config.platforms.windows as ConfigWindows;
    // const deployProjectName = useWinUI3
    //   ? `${projectName} (Package)`
    //   : projectName;

    console.log(JSON.stringify(config));

    const deployProjectName = projectName;
    const appxRecipe = path.join(
      path.dirname(appxManifestPath),
      `${deployProjectName}.build.appxrecipe`,
    );
    const ideFolder = `${buildTools.installationPath}\\Common7\\IDE`;
    const deployAppxRecipeExePath = `${ideFolder}\\DeployAppRecipe.exe`;
    if (
      vsVersion.gte(Version.fromString('16.8.30906.45')) &&
      fs.existsSync(deployAppxRecipeExePath)
    ) {
      await commandWithProgress(
        newSpinner('Deploying'),
        `Deploying ${appxRecipe}`,
        deployAppxRecipeExePath,
        [appxRecipe],
        verbose,
        'DeployRecipeFailure',
      );
    } else {
      // Install the app package's dependencies before attempting to deploy.
      await runPowerShellScriptFunction(
        'Installing dependent framework packages',
        windowsStoreAppUtils,
        `Install-AppDependencies ${appxManifestPath} ${appPackageFolder} ${options.arch}`,
        verbose,
        'InstallAppDependenciesFailure',
      );
      await build.buildSolution(
        buildTools,
        slnFile,
        /* options.release ? 'Release' : */ 'Debug',
        options.arch,
        {DeployLayout: 'true'},
        verbose,
        'deploy',
        options.buildLogDirectory,
      );
    }
  }

  const appFamilyName = execSync(
    `powershell -NoProfile -c $(Get-AppxPackage -Name ${appName}).PackageFamilyName`,
  )
    .toString()
    .trim();

  if (!appFamilyName) {
    throw new CodedError(
      'AppDidNotDeploy',
      'Fail to check the installed app, maybe developer mode is off on Windows',
    );
  }

  const loopbackText = 'Verifying loopbackExempt';
  const loopbackSpinner = newSpinner(loopbackText);

  await commandWithProgress(
    loopbackSpinner,
    loopbackText,
    'CheckNetIsolation',
    `LoopbackExempt -a -n=${appFamilyName}`.split(' '),
    verbose,
    'CheckNetIsolationFailure',
  );

  if (shouldLaunchApp(options)) {
    await runPowerShellScriptFunction(
      'Starting the app',
      windowsStoreAppUtils,
      `Start-Locally ${appName} ${args}`,
      verbose,
      'AppStartupFailure',
    );
  } else {
    newInfo('Skip the step to start the app');
  }
}

export function startServerInNewWindow(
  options: RunWindowsOptions,
  verbose: boolean,
): Promise<void> {
  return new Promise(resolve => {
    if (options.packager) {
      http
        .get('http://localhost:8081/status', res => {
          if (res.statusCode === 200) {
            newSuccess('React-Native Server already started');
          } else {
            newError('React-Native Server not responding');
          }
          resolve();
        })
        .on('error', () => {
          launchServer(options, verbose);
          resolve();
        });
    } else {
      resolve();
    }
  });
}

function launchServer(options: RunWindowsOptions, verbose: boolean) {
  newSuccess('Starting the React-Native Server');
  const opts: SpawnOptions = {
    cwd: options.root,
    detached: true,
    stdio: verbose ? 'inherit' : 'ignore',
  };

  spawn('cmd.exe', ['/C', 'start npx --no-install react-native start'], opts);
}
