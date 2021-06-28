/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

// Types in this file are inaccurate compared to usage in terms of falsiness.
// We should try to rewrite some of this to do automated schema validation to
// guarantee correct types
/* eslint-disable @typescript-eslint/no-unnecessary-condition */

import {platform} from 'os';
import path from 'path';

import * as configUtils from './configUtils';

/*

react-native config will generate the following JSON for each native module dependency
under node_modules that has a Windows implementation, in order to support auto-linking.
This is done heuristically, so if the result isn't quite correct, native module developers
can provide a manual override file: react-native.config.js.

Schema for dependencies:

Tags:
auto - Item is always calculated by config. An override file should NEVER provide it.
req  - Item is required. If an override file exists, it MUST provide it. If no override file exists, config will try to calculate it.
opt  - Item is optional. If an override file exists, it MAY provide it. If no override file exists, config may try to calculate it.

{
  folder: string,       // (auto) Absolute path to the module root folder, determined by react-native config, ex: 'c:\path\to\app-name\node_modules\my-module'
  sourceDir: string,    // (opt, req if projects defined) Relative path to the Windows implementation under folder, ex: 'windows'
  solutionFile: string, // (opt) Relative path to the module's VS solution file under sourceDir, ex: 'MyModule.sln'
  projects: [ // (opt) Array of VS projects that must be added to the consuming app's solution file, so they are built
    {
      projectFile: string,     // (req) Relative path to the VS project file under sourceDir, ex: 'MyModule\MyModule.vcxproj' for 'c:\path\to\app-name\node_modules\my-module\windows\MyModule\MyModule.vcxproj'
      directDependency: bool,  // (req) Whether to add the project file as a dependency to the consuming app's project file. true for projects that provide native modules
      projectName: string,     // (auto) Name of the project, determined from projectFile, ex: 'MyModule'
      projectLang: string,     // (auto) Language of the project, cpp or cs, determined from projectFile
      projectGuid: string,     // (auto) Project identifier, determined from projectFile
      cppHeaders: [],          // (opt) Array of cpp header include lines, ie: 'winrt/MyModule.h', to be transformed into '#include <winrt/MyModule.h>'
      cppPackageProviders: [], // (opt) Array of fully qualified cpp IReactPackageProviders, ie: 'MyModule::ReactPackageProvider'
      csNamespaces: [],        // (opt) Array of cs namespaces, ie: 'MyModule', to be transformed into 'using MyModule;'
      csPackageProviders: [],  // (opt) Array of fully qualified cs IReactPackageProviders, ie: 'MyModule.ReactPackageProvider'
    },
  ],
  nugetPackages: [ // (opt) Array of nuget packages including native modules that must be added as a dependency to the consuming app. It can be empty, but by its nature it can't be calculated
    {
      packageName: string,     // (req) Name of the nuget package to install
      packageVersion: string,  // (req) Version of the nuget package to install
      cppHeaders: [],          // (req) Array of cpp header include lines, ie: 'winrt/NugetModule.h', to be transformed into '#include <winrt/NugetModule.h>'
      cppPackageProviders: [], // (req) Array of fully qualified cpp IReactPackageProviders, ie: 'NugetModule::ReactPackageProvider'
      csNamespaces: [],        // (req) Array of cs namespaces, ie: 'NugetModule', to be transformed into 'using NugetModule;'
      csPackageProviders: [],  // (req) Array of fully qualified cs IReactPackageProviders, ie: 'NugetModule.ReactPackageProvider'
    },
  ],
}

Example react-native.config.js for a 'MyModule':

module.exports = {
  dependency: {
    platforms: {
      windows: {
        sourceDir: 'windows',
        solutionFile: 'MyModule.sln',
        projects: [
          {
            projectFile: 'MyModule\\MyModule.vcxproj',
            directDependency: true,
          }
        ],
      },
    },
  },
};

*/

export interface ProjectDependency {
  projectFile: string;
  directDependency: boolean;
  projectName: string;
  projectLang: 'cpp' | 'cs' | null;
  projectGuid: string | null;
  cppHeaders: string[];
  cppPackageProviders: string[];
  csNamespaces: string[];
  csPackageProviders: string[];
}

export interface NuGetPackageDependency {
  packageName: string;
  packageVersion: string;
  cppHeaders: string[];
  cppPackageProviders: string[];
  csNamespaces: string[];
  csPackageProviders: string[];
}

export interface WindowsDependencyConfig {
  folder: string;
  sourceDir?: string;
  solutionFile?: string | null;
  projects: ProjectDependency[];
  nugetPackages: NuGetPackageDependency[];
}

/**
 * Gets the config of any RNW native modules under the target folder.
 * @param userConfig A manually specified override config.
 * @return The config if any RNW native modules exist.
 */
// Disabled due to existing high cyclomatic complexity
// eslint-disable-next-line complexity
export function dependencyConfigWindows(
  folder: string,
  userConfig: Partial<WindowsDependencyConfig> | null = {},
): WindowsDependencyConfig | null {
  if (platform() !== 'win32') {
    return null;
  }

  if (userConfig === null) {
    return null;
  }

  const usingManualProjectsOverride =
    'projects' in userConfig && Array.isArray(userConfig.projects);

  const usingManualNugetPackagesOverride =
    'nugetPackages' in userConfig && Array.isArray(userConfig.nugetPackages);

  const result: WindowsDependencyConfig = {
    folder,
    projects: usingManualProjectsOverride ? userConfig.projects! : [],
    solutionFile: null,
    nugetPackages: usingManualNugetPackagesOverride
      ? userConfig.nugetPackages!
      : [],
  };

  let sourceDir: string | null = null;
  if (usingManualProjectsOverride && result.projects.length > 0) {
    // Manually provided projects, so extract the sourceDir
    if (!('sourceDir' in userConfig)) {
      sourceDir =
        'Error: Source dir is required if projects are specified, but it is not specified in react-native.config.';
    } else if (userConfig.sourceDir === null) {
      sourceDir =
        'Error: Source dir is required if projects are specified, but it is null in react-native.config.';
    } else {
      sourceDir = path.join(folder, userConfig.sourceDir!);
    }
  } else if (!usingManualProjectsOverride) {
    // No manually provided projects, try to find sourceDir
    if ('sourceDir' in userConfig && userConfig.sourceDir !== null) {
      sourceDir = path.join(folder, userConfig.sourceDir!);
    } else {
      sourceDir = configUtils.findWindowsFolder(folder);
    }
  }

  if (sourceDir === null) {
    // Try to salvage the missing sourceDir
    if (result.projects.length === 0 && result.nugetPackages.length > 0) {
      // Only nuget packages, no sourceDir required
      return result;
    } else if (result.projects.length > 0) {
      // Projects overridden but no sourceDir, assume the sourceDir === folder
      sourceDir = folder;
    }
  } else if (sourceDir.startsWith('Error: ')) {
    // Source dir error, bail with error
    result.sourceDir = sourceDir;
    return result;
  }

  if (sourceDir === null) {
    // After everything above, if sourceDir is still null,
    // there's nothing more to look for here, bail
    return null;
  }

  result.sourceDir = path.relative(folder, sourceDir);

  const usingManualSolutionFile = 'solutionFile' in userConfig;

  let solutionFile = null;
  if (usingManualSolutionFile && userConfig.solutionFile !== null) {
    // Manually provided solutionFile, so extract it
    solutionFile = path.join(sourceDir, userConfig.solutionFile!);
  } else if (!usingManualSolutionFile) {
    // No manually provided solutionFile, try to find it
    const foundSolutions = configUtils.findSolutionFiles(sourceDir);
    if (foundSolutions.length === 1) {
      solutionFile = path.join(sourceDir, foundSolutions[0]);
    }
  }

  result.solutionFile =
    solutionFile !== null ? path.relative(sourceDir, solutionFile) : null;

  if (usingManualProjectsOverride) {
    // react-native.config used, fill out (auto) items for each provided project, verify (req) items are present

    const alwaysRequired: Array<keyof ProjectDependency> = [
      'projectFile',
      'directDependency',
    ];

    for (const project of result.projects) {
      // Verifying (req) items
      let errorFound = false;

      alwaysRequired.forEach(item => {
        if (!(item in project)) {
          (project[
            item
          ] as string) = `Error: ${item} is required for each project in react-native.config`;
          errorFound = true;
        }
      });

      if (errorFound) {
        break;
      }

      const projectFile = path.join(sourceDir, project.projectFile);

      const projectContents = configUtils.readProjectFile(projectFile);

      project.projectFile = path.relative(sourceDir, projectFile);

      // Calculating (auto) items
      project.projectName = configUtils.getProjectName(
        projectFile,
        projectContents,
      );
      project.projectLang = configUtils.getProjectLanguage(projectFile);
      project.projectGuid = configUtils.getProjectGuid(projectContents);

      if (project.directDependency) {
        // Calculating more (auto) items

        const projectNamespace = configUtils.getProjectNamespace(
          projectContents,
        );

        if (projectNamespace !== null) {
          const cppNamespace = projectNamespace!.replace(/\./g, '::');
          const csNamespace = projectNamespace!.replace(/::/g, '.');

          project.cppHeaders = project.cppHeaders || [`winrt/${csNamespace}.h`];
          project.cppPackageProviders = project.cppPackageProviders || [
            `${cppNamespace}::ReactPackageProvider`,
          ];
          project.csNamespaces = project.csNamespaces || [`${csNamespace}`];
          project.csPackageProviders = project.csPackageProviders || [
            `${csNamespace}.ReactPackageProvider`,
          ];
        }
      }
    }
  } else {
    // No react-native.config, try to heuristically find any projects

    const foundProjects = configUtils.findDependencyProjectFiles(sourceDir);

    for (const foundProject of foundProjects) {
      const projectFile = path.join(sourceDir, foundProject);

      const projectContents = configUtils.readProjectFile(projectFile);

      const projectType = configUtils.getProjectType(
        projectFile,
        projectContents,
      );

      if (projectType === 'dynamiclibrary' || projectType === 'winmdobj') {
        const projectLang = configUtils.getProjectLanguage(projectFile);

        const projectName = configUtils.getProjectName(
          projectFile,
          projectContents,
        );

        const projectGuid = configUtils.getProjectGuid(projectContents);

        const projectNamespace = configUtils.getProjectNamespace(
          projectContents,
        );

        const directDependency = true;

        const cppHeaders: string[] = [];
        const cppPackageProviders: string[] = [];
        const csNamespaces: string[] = [];
        const csPackageProviders: string[] = [];

        if (projectNamespace !== null) {
          const cppNamespace = projectNamespace.replace(/\./g, '::');
          const csNamespace = projectNamespace.replace(/::/g, '.');

          cppHeaders.push(`winrt/${csNamespace}.h`);
          cppPackageProviders.push(`${cppNamespace}::ReactPackageProvider`);
          csNamespaces.push(`${csNamespace}`);
          csPackageProviders.push(`${csNamespace}.ReactPackageProvider`);
        }

        result.projects.push({
          projectFile: path.relative(sourceDir, projectFile),
          projectName,
          projectLang,
          projectGuid,
          directDependency,
          cppHeaders,
          cppPackageProviders,
          csNamespaces,
          csPackageProviders,
        });
      } else {
        const projectPath = path.relative(sourceDir, projectFile);
        result.projects.push({
          projectFile: `Error: ${projectPath} is type '${projectType}'`,
          directDependency: false,
          projectName: '',
          projectLang: null,
          projectGuid: null,
          cppHeaders: [],
          cppPackageProviders: [],
          csNamespaces: [],
          csPackageProviders: [],
        });
      }
    }
  }

  return result;
}
