/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

// Types in this file are inaccurate compared to usage in terms of falsiness.
// We should try to rewrite some of this to do automated schema validation to
// guarantee correct types
/* eslint-disable @typescript-eslint/no-unnecessary-condition */

import fs from 'fs';
import path from 'path';
import chalk from 'chalk';
import {performance} from 'perf_hooks';

import {newSpinner} from './commandWithProgress';
import * as vstools from './vstools';
import * as generatorCommon from '../../generator-common';
import * as configUtils from '../../config/configUtils';

import {
  Command,
  Config,
  Dependency,
  ProjectConfig,
} from '@react-native-community/cli-types';
import {
  WindowsDependencyConfig,
  ProjectDependency,
} from '../../config/dependencyConfig';
import {Project, WindowsProjectConfig} from '../../config/projectConfig';
import {CodedError} from '@react-native-windows/telemetry';
import {XMLSerializer} from 'xmldom';
import {Ora} from 'ora';
const formatter = require('xml-formatter');

export class AutolinkWindows {
  private changesNecessary: boolean;
  protected windowsAppConfig: WindowsProjectConfig;

  public areChangesNeeded() {
    return this.changesNecessary;
  }

  private getWindowsConfig() {
    return this.windowsAppConfig;
  }

  private getSolutionFile() {
    return path.join(
      this.getWindowsConfig().folder,
      this.getWindowsConfig().sourceDir,
      this.getWindowsConfig().solutionFile,
    );
  }

  constructor(
    readonly projectConfig: ProjectConfig,
    readonly dependenciesConfig: {[key: string]: Dependency},
    readonly options: AutoLinkOptions,
  ) {
    this.changesNecessary = false;
    if (
      !('windows' in this.projectConfig) ||
      this.projectConfig.windows === null
    ) {
      throw new CodedError(
        'NoWindowsConfig',
        'Windows auto-link only supported on Windows app projects',
      );
    }
    this.windowsAppConfig = projectConfig.windows;
  }

  public async run(spinner: Ora) {
    const verbose = this.options.logging;

    verboseMessage('', verbose);
    verboseMessage('Parsing project...', verbose);

    const rnwRoot = resolveRnwRoot(this.windowsAppConfig);
    const templateRoot = resolveTemplateRoot(this.windowsAppConfig);

    this.fixUpForSlnOption();
    this.fixUpForProjOption();

    verboseMessage('Found Windows app project, config:', verbose);
    verboseMessage(this.windowsAppConfig, verbose);

    this.validateRequiredAppProperties();

    const solutionFile = this.getSolutionFile();

    const windowsAppProjectConfig = this.windowsAppConfig.project;

    this.validateRequiredProjectProperties();

    const projectFile = this.getProjectFile();

    const projectDir = path.dirname(projectFile);
    const projectLang = windowsAppProjectConfig.projectLang!;

    verboseMessage('Parsing dependencies...', verbose);

    this.changesNecessary =
      (await this.ensureXAMLDialect()) || this.changesNecessary;

    // Generating cs/cpp files for app code consumption
    if (projectLang === 'cs') {
      this.changesNecessary =
        (await this.generateCSAutolinking(
          templateRoot,
          projectLang,
          projectDir,
        )) || this.changesNecessary;
    } else if (projectLang === 'cpp') {
      this.changesNecessary =
        (await this.generateCppAutolinking(
          templateRoot,
          projectLang,
          projectDir,
        )) || this.changesNecessary;
    }

    // Generating props for app project consumption
    let propertiesForProps = '';
    let csModuleNames: string[] = [];

    if (projectLang === 'cpp') {
      csModuleNames = this.getCSModules();

      if (csModuleNames.length > 0) {
        propertiesForProps += `
    <!-- Set due to dependency on C# module(s): ${csModuleNames.join()} -->
    <ConsumeCSharpModules Condition="'$(ConsumeCSharpModules)'==''">true</ConsumeCSharpModules>`;
      }
    }

    this.changesNecessary =
      (await this.generateAutolinkProps(
        templateRoot,
        projectDir,
        propertiesForProps,
      )) || this.changesNecessary;

    // Generating targets for app project consumption
    this.changesNecessary =
      (await this.generateAutolinkTargets(projectDir, templateRoot)) ||
      this.changesNecessary;

    // Generating project entries for solution
    this.changesNecessary =
      this.updateSolution(rnwRoot, solutionFile) || this.changesNecessary;

    spinner.succeed();
  }

  /**
   * Handles the --proj command-line option by consuming its value into the windowsAppConfig
   */
  public fixUpForProjOption() {
    if (this.options.proj) {
      const projFile = path.join(
        this.windowsAppConfig.folder,
        this.options.proj,
      );

      const projectContents = configUtils.readProjectFile(projFile);

      this.windowsAppConfig.project = {
        projectFile: path.relative(
          path.join(
            this.windowsAppConfig.folder,
            this.windowsAppConfig.sourceDir,
          ),
          projFile,
        ),
        projectName: configUtils.getProjectName(projFile, projectContents),
        projectLang: configUtils.getProjectLanguage(projFile),
        projectGuid: configUtils.getProjectGuid(projectContents),
      };
    }
  }

  /**
   * Handles the --sln command-line option by consuming its value into the windowsAppConfig
   */
  public fixUpForSlnOption() {
    if (this.options.sln) {
      const slnFile = path.join(this.windowsAppConfig.folder, this.options.sln);
      this.windowsAppConfig.solutionFile = path.relative(
        path.join(
          this.windowsAppConfig.folder,
          this.windowsAppConfig.sourceDir,
        ),
        slnFile,
      );
    }
  }

  /** Validates the all of the required app (solution) properties are present and valid */
  public validateRequiredAppProperties() {
    const alwaysRequired: Array<keyof WindowsProjectConfig> = [
      'folder',
      'sourceDir',
      'solutionFile',
      'project',
    ];

    alwaysRequired.forEach(item => {
      if (
        !(item in this.windowsAppConfig) ||
        this.windowsAppConfig[item] === null
      ) {
        throw new CodedError(
          'IncompleteConfig',
          `${item} is required but not specified by react-native config`,
          {item: item},
        );
      } else if (
        typeof this.windowsAppConfig[item] === 'string' &&
        (this.windowsAppConfig[item] as string).startsWith('Error: ')
      ) {
        throw new CodedError(
          'InvalidConfig',
          `${item} invalid. ${this.windowsAppConfig[item]}`,
          {item: item},
        );
      }
    });
  }

  /** @return the full path to the project file (.vcxproj or .csproj) */
  private getProjectFile() {
    const windowsAppConfig = this.getWindowsConfig();
    return path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      windowsAppConfig.project.projectFile,
    );
  }

  /** Validates that all of the required app _project_ properties are present and valid */
  public validateRequiredProjectProperties() {
    const windowsAppProjectConfig = this.windowsAppConfig.project;
    const projectRequired: Array<keyof Project> = [
      'projectFile',
      'projectName',
      'projectLang',
      'projectGuid',
    ];

    projectRequired.forEach(item => {
      if (
        !(item in windowsAppProjectConfig) ||
        windowsAppProjectConfig[item] === null
      ) {
        throw new CodedError(
          'IncompleteConfig',
          `project.${item} is required but not specified by react-native config`,
          {item: item},
        );
      } else if (
        typeof windowsAppProjectConfig[item] === 'string' &&
        windowsAppProjectConfig[item]!.startsWith('Error: ')
      ) {
        throw new CodedError(
          'InvalidConfig',
          `project.${item} invalid. ${windowsAppProjectConfig[item]}`,
          {item: item},
        );
      }
    });
  }

  private async generateCppAutolinking(
    templateRoot: string,
    projectLang: string,
    projectDir: string,
  ) {
    const {cppPackageProviders, cppIncludes} = this.getCppReplacements();

    const cppFileName = 'AutolinkedNativeModules.g.cpp';

    const srcCppFile = path.join(
      templateRoot,
      `${projectLang}-app`,
      'src',
      cppFileName,
    );

    const destCppFile = path.join(projectDir, cppFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destCppFile))}...`,
      this.options.logging,
    );

    const cppContents = generatorCommon.resolveContents(srcCppFile, {
      useMustache: true,
      autolinkCppIncludes: cppIncludes,
      autolinkCppPackageProviders: cppPackageProviders,
    });

    return await this.updateFile(destCppFile, cppContents);
  }

  public getCppReplacements() {
    let cppIncludes = '';
    let cppPackageProviders = '';
    const windowsDependencies = this.getWindowsDependencies();

    for (const dependencyName of Object.keys(windowsDependencies)) {
      windowsDependencies[dependencyName].projects.forEach(project => {
        if (project.directDependency) {
          cppIncludes += `\n\n// Includes from ${dependencyName}`;
          project.cppHeaders.forEach(header => {
            cppIncludes += `\n#include <${header}>`;
          });

          cppPackageProviders += `\n    // IReactPackageProviders from ${dependencyName}`;
          project.cppPackageProviders.forEach(packageProvider => {
            cppPackageProviders += `\n    packageProviders.Append(winrt::${packageProvider}());`;
          });
        }
      });
    }

    if (cppPackageProviders === '') {
      // There are no windows dependencies, this would result in warning. C4100: 'packageProviders': unreferenced formal parameter.
      // therefore add a usage.
      cppPackageProviders = '\n    UNREFERENCED_PARAMETER(packageProviders);'; // CODESYNC: vnext\local-cli\generator-windows\index.js
    }
    return {cppPackageProviders, cppIncludes};
  }

  private generateCSAutolinking(
    templateRoot: string,
    projectLang: string,
    projectDir: string,
  ) {
    const {
      csUsingNamespaces,
      csReactPackageProviders,
    } = this.getCsReplacements();

    const csFileName = 'AutolinkedNativeModules.g.cs';

    const srcCsFile = path.join(
      templateRoot,
      `${projectLang}-app`,
      'src',
      csFileName,
    );

    const destCsFile = path.join(projectDir, csFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destCsFile))}...`,
      this.options.logging,
    );

    const csContents = generatorCommon.resolveContents(srcCsFile, {
      useMustache: true,
      autolinkCsUsingNamespaces: csUsingNamespaces,
      autolinkCsReactPackageProviders: csReactPackageProviders,
    });

    return this.updateFile(destCsFile, csContents);
  }

  public getCsReplacements() {
    let csUsingNamespaces = '';
    let csReactPackageProviders = '';

    const windowsDependencies = this.getWindowsDependencies();
    for (const dependencyName of Object.keys(windowsDependencies)) {
      windowsDependencies[dependencyName].projects.forEach(project => {
        if (project.directDependency) {
          csUsingNamespaces += `\n\n// Namespaces from ${dependencyName}`;
          project.csNamespaces.forEach(namespace => {
            csUsingNamespaces += `\nusing ${namespace};`;
          });

          csReactPackageProviders += `\n            // IReactPackageProviders from ${dependencyName}`;
          project.csPackageProviders.forEach(packageProvider => {
            csReactPackageProviders += `\n            packageProviders.Add(new ${packageProvider}());`;
          });
        }
      });
    }
    return {csUsingNamespaces, csReactPackageProviders};
  }

  /** Cache of dependencies */
  private readonly windowsDependencies: Record<
    string,
    WindowsDependencyConfig
  > = {};

  private getWindowsDependencies() {
    if (Object.keys(this.windowsDependencies).length === 0) {
      for (const dependencyName of Object.keys(this.dependenciesConfig)) {
        const windowsDependency: WindowsDependencyConfig | undefined = this
          .dependenciesConfig[dependencyName].platforms.windows;

        if (windowsDependency) {
          verboseMessage(
            `${chalk.bold(dependencyName)} has Windows implementation, config:`,
            this.options.logging,
          );
          verboseMessage(windowsDependency, this.options.logging);

          let dependencyIsValid = true;

          dependencyIsValid = !!(
            dependencyIsValid &&
            'sourceDir' in windowsDependency &&
            windowsDependency.sourceDir &&
            !windowsDependency.sourceDir.startsWith('Error: ')
          );

          if (
            'projects' in windowsDependency &&
            Array.isArray(windowsDependency.projects)
          ) {
            if (
              windowsDependency.projects.length === 0 &&
              dependencyName.includes('react-native')
            ) {
              // the dependency is probably a react native module, but we didn't find a module project
              throw new CodedError(
                'Autolinking',
                `Found a Windows solution for ${dependencyName} but no React Native for Windows native module projects`,
              );
            }
            windowsDependency.projects.forEach(project => {
              const itemsToCheck: Array<keyof ProjectDependency> = [
                'projectFile',
                'directDependency',
              ];
              itemsToCheck.forEach(item => {
                dependencyIsValid = !!(
                  dependencyIsValid &&
                  item in project &&
                  project[item] !== '' &&
                  !project[item]!.toString().startsWith('Error: ')
                );
              });
            });
          }

          if (dependencyIsValid) {
            verboseMessage(
              `Adding ${chalk.bold(dependencyName)}.`,
              this.options.logging,
            );
            this.windowsDependencies[dependencyName] = windowsDependency;
          } else {
            verboseMessage(
              `Invalid dependency configuration for dependency ${dependencyName}`,
              this.options.logging,
            );
          }
        }
      }
    }
    return this.windowsDependencies;
  }

  /**
   * Updates the target file with the expected contents if it's different.
   * @param filePath Path to the target file to update.
   * @param expectedContents The expected contents of the file.
   * @return Whether any changes were necessary.
   */
  protected async updateFile(filePath: string, expectedContents: string) {
    const fileName = chalk.bold(path.basename(filePath));
    verboseMessage(`Reading ${fileName}...`, this.options.logging);
    const actualContents = fs.existsSync(filePath)
      ? (await fs.promises.readFile(filePath)).toString()
      : '';

    const contentsChanged = expectedContents !== actualContents;

    if (contentsChanged) {
      verboseMessage(
        chalk.yellow(`${fileName} needs to be updated.`),
        this.options.logging,
      );
      if (!this.options.check) {
        verboseMessage(`Writing ${fileName}...`, this.options.logging);
        await fs.promises.writeFile(filePath, expectedContents, {
          encoding: 'utf8',
          flag: 'w',
        });
      }
    } else {
      verboseMessage(`No changes to ${fileName}.`, this.options.logging);
    }

    return contentsChanged;
  }

  private generateAutolinkTargets(projectDir: string, templateRoot: string) {
    let projectReferencesForTargets = '';

    const windowsDependencies = this.getWindowsDependencies();
    for (const dependencyName of Object.keys(windowsDependencies)) {
      windowsDependencies[dependencyName].projects.forEach(project => {
        if (project.directDependency) {
          const dependencyProjectFile = path.join(
            windowsDependencies[dependencyName].folder,
            windowsDependencies[dependencyName].sourceDir!,
            project.projectFile,
          );

          const relDependencyProjectFile = path.relative(
            projectDir,
            dependencyProjectFile,
          );

          projectReferencesForTargets += `
    <!-- Projects from ${dependencyName} -->
    <ProjectReference Include="$(ProjectDir)${relDependencyProjectFile}">
      <Project>${project.projectGuid}</Project>
    </ProjectReference>`;
        }
      });
    }

    const targetFileName = 'AutolinkedNativeModules.g.targets';

    const srcTargetFile = path.join(
      templateRoot,
      `shared-app`,
      'src',
      targetFileName,
    );

    const destTargetFile = path.join(projectDir, targetFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destTargetFile))}...`,
      this.options.logging,
    );

    const targetContents = generatorCommon.resolveContents(srcTargetFile, {
      useMustache: true,
      autolinkProjectReferencesForTargets: projectReferencesForTargets,
    });

    return this.updateFile(destTargetFile, targetContents);
  }

  private generateAutolinkProps(
    templateRoot: string,
    projectDir: string,
    propertiesForProps: string,
  ) {
    const propsFileName = 'AutolinkedNativeModules.g.props';

    const srcPropsFile = path.join(
      templateRoot,
      `shared-app`,
      'src',
      propsFileName,
    );

    const destPropsFile = path.join(projectDir, propsFileName);

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(destPropsFile))}...`,
      this.options.logging,
    );

    const propsContents = generatorCommon.resolveContents(srcPropsFile, {
      useMustache: true,
      autolinkPropertiesForProps: propertiesForProps,
    });

    return this.updateFile(destPropsFile, propsContents);
  }

  private getCSModules() {
    const csModuleNames: string[] = [];
    const windowsDependencies = this.getWindowsDependencies();
    for (const dependencyName of Object.keys(windowsDependencies)) {
      windowsDependencies[dependencyName].projects.forEach(project => {
        if (project.directDependency && project.projectLang === 'cs') {
          csModuleNames.push(project.projectName);
        }
      });
    }
    return csModuleNames;
  }

  private updateSolution(rnwRoot: string, solutionFile: string) {
    const projectsForSolution: Project[] = [];
    const windowsDependencies = this.getWindowsDependencies();

    for (const dependencyName of Object.keys(windowsDependencies)) {
      // Process dependency projects
      windowsDependencies[dependencyName].projects.forEach(project => {
        const dependencyProjectFile = path.join(
          windowsDependencies[dependencyName].folder,
          windowsDependencies[dependencyName].sourceDir!,
          project.projectFile,
        );

        projectsForSolution.push({
          projectFile: dependencyProjectFile,
          projectName: project.projectName,
          projectLang: project.projectLang,
          projectGuid: project.projectGuid,
        });
      });
    }

    const csModuleNames = this.getCSModules();
    if (csModuleNames.length > 0) {
      // Add managed projects
      projectsForSolution.push({
        projectFile: path.join(
          rnwRoot,
          'Microsoft.ReactNative.Managed/Microsoft.ReactNative.Managed.csproj',
        ),
        projectName: 'Microsoft.ReactNative.Managed',
        projectLang: 'cs',
        projectGuid: '{F2824844-CE15-4242-9420-308923CD76C3}',
      });
      projectsForSolution.push({
        projectFile: path.join(
          rnwRoot,
          'Microsoft.ReactNative.Managed.CodeGen//Microsoft.ReactNative.Managed.CodeGen.csproj',
        ),
        projectName: 'Microsoft.ReactNative.Managed.CodeGen',
        projectLang: 'cs',
        projectGuid: '{ADED4FBE-887D-4271-AF24-F0823BCE7961}',
        projectTypeGuid: vstools.dotNetCoreProjectTypeGuid,
      });
    }

    verboseMessage(
      `Calculating ${chalk.bold(path.basename(solutionFile))} changes...`,
      this.options.logging,
    );

    let changesNecessary = false;
    projectsForSolution.forEach(project => {
      const contentsChanged = vstools.addProjectToSolution(
        solutionFile,
        project,
        this.options.logging,
        this.options.check,
      );
      changesNecessary = changesNecessary || contentsChanged;
    });
    return changesNecessary;
  }

  protected getExperimentalFeaturesPropsXml() {
    const experimentalFeaturesProps = path.join(
      path.dirname(this.getSolutionFile()),
      'ExperimentalFeatures.props',
    );
    if (fs.existsSync(experimentalFeaturesProps)) {
      const experimentalFeaturesContents = configUtils.readProjectFile(
        experimentalFeaturesProps,
      );
      return {
        path: experimentalFeaturesProps,
        content: experimentalFeaturesContents,
      };
    }
    return undefined;
  }

  public async ensureXAMLDialect() {
    let changesNeeded = false;
    const useWinUI3FromConfig = this.getWindowsConfig().useWinUI3;
    const experimentalFeatures = this.getExperimentalFeaturesPropsXml();
    if (experimentalFeatures) {
      const useWinUI3FromExperimentalFeatures =
        configUtils
          .tryFindPropertyValue(experimentalFeatures.content, 'UseWinUI3')
          ?.toLowerCase() === 'true';
      // use the UseWinUI3 value in react-native.config.js, or if not present, the value from ExperimentalFeatures.props
      changesNeeded = await this.updatePackagesConfigXAMLDialect(
        useWinUI3FromConfig !== undefined
          ? useWinUI3FromConfig
          : useWinUI3FromExperimentalFeatures,
      );
      if (useWinUI3FromConfig !== undefined) {
        // Make sure ExperimentalFeatures.props matches the value that comes from react-native.config.js
        const node = experimentalFeatures.content.getElementsByTagName(
          'UseWinUI3',
        );
        const newValue = useWinUI3FromConfig ? 'true' : 'false';
        changesNeeded = node.item(0)?.textContent !== newValue || changesNeeded;
        if (!this.options.check && changesNeeded) {
          node.item(0)!.textContent = newValue;
          const experimentalFeaturesOutput = new XMLSerializer().serializeToString(
            experimentalFeatures.content,
          );
          await this.updateFile(
            experimentalFeatures.path,
            experimentalFeaturesOutput,
          );
        }
      }
    }
    return changesNeeded;
  }

  protected getPackagesConfigXml() {
    const projectFile = this.getProjectFile();
    const packagesConfig = path.join(
      path.dirname(projectFile),
      'packages.config',
    );

    if (fs.existsSync(packagesConfig)) {
      return {
        path: packagesConfig,
        content: configUtils.readProjectFile(packagesConfig),
      };
    }
    return undefined;
  }

  private async updatePackagesConfigXAMLDialect(useWinUI3: boolean) {
    let changed = false;
    const packagesConfig = this.getPackagesConfigXml();
    if (packagesConfig) {
      // if we don't have a packages.config, then this is a C# project, in which case we use <PackageReference> and dynamically pick the right XAML package.
      const project = this.getWindowsConfig();

      const winUIPropsPath = path.join(
        resolveRnwRoot(project),
        'PropertySheets/WinUI.props',
      );
      const winuiPropsContents = configUtils.readProjectFile(winUIPropsPath);
      const winui2xVersion = configUtils.tryFindPropertyValue(
        winuiPropsContents,
        'WinUI2xVersion',
      );
      const winui3Version = configUtils.tryFindPropertyValue(
        winuiPropsContents,
        'WinUI3Version',
      );

      const dialects = [
        {id: 'Microsoft.ProjectReunion.WinUI', version: winui3Version!},
        {id: 'Microsoft.UI.Xaml', version: winui2xVersion!},
      ];
      const keepPkg = useWinUI3 ? dialects[0] : dialects[1];
      const removePkg = useWinUI3 ? dialects[1] : dialects[0];

      changed = this.updatePackagesConfig(
        packagesConfig,
        [removePkg],
        [keepPkg],
      );

      if (!this.options.check && changed) {
        const serializer = new XMLSerializer();
        const output = serializer.serializeToString(packagesConfig.content);
        const formattedXml = formatter(output, {indentation: '  '});
        await this.updateFile(packagesConfig.path, formattedXml);
      }
    }
    return changed;
  }

  private updatePackagesConfig(
    packagesConfig: {path: string; content: Document},
    removePkgs: {id: string; version: string}[],
    keepPkgs: {id: string; version: string}[],
  ) {
    let changed = false;
    const packageElements = packagesConfig.content.documentElement.getElementsByTagName(
      'package',
    );

    const nodesToRemove: Element[] = [];

    for (let i = 0; i < packageElements.length; i++) {
      const packageElement = packageElements.item(i)!;
      const idAttr = packageElement!.getAttributeNode('id');
      const id = idAttr!.value;
      const keepPkg = keepPkgs.find(pkg => pkg.id === id);
      if (removePkgs.find(pkg => pkg.id === id)) {
        nodesToRemove.push(packageElement);
        changed = true;
      } else if (keepPkg) {
        changed =
          changed || keepPkg.version !== packageElement.getAttribute('version');
        packageElement.setAttribute('version', keepPkg.version!);
        keepPkgs = keepPkgs.filter(pkg => pkg.id !== keepPkg.id);
      }
    }

    nodesToRemove.forEach(pkg =>
      packagesConfig.content.documentElement.removeChild(pkg),
    );

    keepPkgs.forEach(keepPkg => {
      const newPkg = packagesConfig.content.createElement('package');

      Object.entries(keepPkg).forEach(([attr, value]) => {
        newPkg.setAttribute(attr, value as string);
      });
      newPkg.setAttribute('targetFramework', 'native');
      packagesConfig.content.documentElement.appendChild(newPkg);
      changed = true;
    });
    return changed;
  }

  /** @return The CLI command to invoke autolink-windows independently */
  public getAutolinkWindowsCommand() {
    const folder = this.windowsAppConfig.folder;

    const autolinkCommand = 'npx react-native autolink-windows';
    const autolinkArgs = `--sln "${path.relative(
      folder,
      this.getSolutionFile(),
    )}" --proj "${path.relative(folder, this.getProjectFile())}"`;
    return `${autolinkCommand} ${autolinkArgs}`;
  }
}

/**
 * Locates the react-native-windows directory
 * @param config project configuration
 */
function resolveRnwRoot(projectConfig: WindowsProjectConfig) {
  const rnwPackage = path.dirname(
    require.resolve('react-native-windows/package.json', {
      paths: [projectConfig.folder],
    }),
  );
  return rnwPackage;
}

/**
 * Locates the react-native-windows directory containing template files
 * @param config project configuration
 */
function resolveTemplateRoot(projectConfig: WindowsProjectConfig) {
  const rnwPackage = resolveRnwRoot(projectConfig);
  return path.join(rnwPackage, 'template');
}

/**
 * Logs the given message if verbose is True.
 * @param message The message to log.
 * @param verbose Whether or not verbose logging is enabled.
 */
function verboseMessage(message: any, verbose: boolean) {
  if (verbose) {
    console.log(message);
  }
}

/**
 * Performs auto-linking for RNW native modules and apps.
 * @param args Unprocessed args passed from react-native CLI.
 * @param config Config passed from react-native CLI.
 * @param options Options passed from react-native CLI.
 */
async function updateAutoLink(
  args: string[],
  config: Config,
  options: AutoLinkOptions,
) {
  const startTime = performance.now();
  const spinner = newSpinner(
    options.check ? 'Checking auto-linked files...' : 'Auto-linking...',
  );
  try {
    const autolink = new AutolinkWindows(
      config.project,
      config.dependencies,
      options,
    );
    await autolink.run(spinner);
    const endTime = performance.now();

    if (!autolink.areChangesNeeded()) {
      console.log(
        `${chalk.green(
          'Success:',
        )} No auto-linking changes necessary. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    } else if (options.check) {
      const autolinkCommand = autolink.getAutolinkWindowsCommand();
      console.log(
        `${chalk.yellow(
          'Warning:',
        )} Auto-linking changes were necessary but ${chalk.bold(
          '--check',
        )} specified. Run '${chalk.bold(
          `${autolinkCommand}`,
        )}' to apply the changes. (${Math.round(endTime - startTime)}ms)`,
      );
      throw new CodedError(
        'NeedAutolinking',
        `Auto-linking changes were necessary but --check was specified. Run '${autolinkCommand}' to apply the changes`,
      );
    } else {
      console.log(
        `${chalk.green(
          'Success:',
        )} Auto-linking changes completed. (${Math.round(
          endTime - startTime,
        )}ms)`,
      );
    }
  } catch (e) {
    spinner.fail();
    const endTime = performance.now();
    console.log(
      `${chalk.red('Error:')} ${e.toString()}. (${Math.round(
        endTime - startTime,
      )}ms)`,
    );
    throw e;
  }
}

interface AutoLinkOptions {
  logging: boolean;
  check: boolean;
  sln?: string;
  proj?: string;
}

export const autoLinkCommand: Command = {
  name: 'autolink-windows',
  description: 'performs autolinking',
  func: updateAutoLink,
  options: [
    {
      name: '--logging',
      description: 'Verbose output logging',
      default: false,
    },
    {
      name: '--check',
      description: 'Only check whether any autolinked files need to change',
      default: false,
    },
    {
      name: '--sln [string]',
      description:
        "Override the app solution file determined by 'react-native config', e.g. windows\\myApp.sln",
      default: undefined,
    },
    {
      name: '--proj [string]',
      description:
        "Override the app project file determined by 'react-native config', e.g. windows\\myApp\\myApp.vcxproj",
      default: undefined,
    },
  ],
};
