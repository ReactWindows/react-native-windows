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

react-native config will generate the following JSON for app projects that have a
windows implementation, as a target for auto-linking. This is done heuristically,
so if the result isn't quite correct, app developers can provide a manual override
file: react-native.config.js.

Schema for app projects:

Tags:
auto - Item is always calculated by config. An override file should NEVER provide it.
req  - Item is required. If an override file exists, it MUST provide it. If no override file exists, config will try to calculate it.
opt  - Item is optional. If an override file exists, it MAY provide it. If no override file exists, config may try to calculate it.

{
  folder: string,       // (auto) Absolute path to the app root folder, determined by react-native config, ex: 'c:\path\to\my-app'
  sourceDir: string,    // (req) Relative path to the Windows implementation under folder, ex: 'windows'
  solutionFile: string, // (req) Relative path to the app's VS solution file under sourceDir, ex: 'MyApp.sln'
  useWinUI3: boolean    // (opt) If true, use WinUI 3. If false, use Windows XAML and WinUI 2.x. If missing, the value from rnwRoot\PropertySheets\ExperimentalFeatures.props will be used.
  project: { // (req)
    projectFile: string, // (req) Relative path to the VS project file under sourceDir, ex: 'MyApp\MyApp.vcxproj' for 'c:\path\to\my-app\windows\MyApp\MyApp.vcxproj'
    projectName: string, // (auto) Name of the project, determined from projectFile, ex: 'MyApp'
    projectLang: string, // (auto) Language of the project, cpp or cs, determined from projectFile
    projectGuid: string, // (auto) Project identifier, determined from projectFile
  },
}

Example react-native.config.js for a 'MyApp':

module.exports = {
  project: {
    windows: {
      sourceDir: 'windows',
      solutionFile: 'MyApp.sln',
      project: {
        projectFile: 'MyApp\\MyApp.vcxproj',
      },
    },
  },
};

*/

export interface Project {
  projectFile: string;
  projectName: string;
  projectLang: 'cpp' | 'cs' | null;
  projectGuid: string | null;
  projectTypeGuid?: string;
}

export interface WindowsProjectConfig {
  folder: string;
  sourceDir: string;
  solutionFile: string;
  project: Project;
  useWinUI3?: boolean;
}

type DeepPartial<T> = {[P in keyof T]?: DeepPartial<T[P]>};

/**
 * Gets the config of any RNW apps under the target folder.
 * @param folder The absolute path to the target folder.
 * @param userConfig A manually specified override config.
 * @return The config if any RNW apps exist.
 */
export function projectConfigWindows(
  folder: string,
  userConfig: Partial<WindowsProjectConfig> | null = {},
): WindowsProjectConfig | null {
  if (platform() !== 'win32') {
    return null;
  }

  if (userConfig === null) {
    return null;
  }

  const usingManualOverride = 'sourceDir' in userConfig;

  const sourceDir = usingManualOverride
    ? path.join(folder, userConfig.sourceDir!)
    : configUtils.findWindowsFolder(folder);

  if (sourceDir === null) {
    // Nothing to look for here, bail
    return null;
  }

  const result: DeepPartial<WindowsProjectConfig> = {
    folder: folder,
    sourceDir: path.relative(folder, sourceDir),
  };

  let validSolution = false;
  let validProject = false;

  if (usingManualOverride) {
    // Manual override, try to use it for solutionFile
    if (!('solutionFile' in userConfig)) {
      result.solutionFile =
        'Error: Solution file is required but not specified in react-native.config.';
    } else if (userConfig.solutionFile === null) {
      result.solutionFile =
        'Error: Solution file is null in react-native.config.';
    } else {
      result.solutionFile = path.normalize(userConfig.solutionFile!);
      validSolution = true;
    }

    // Manual override, try to use it for project
    if (!('project' in userConfig)) {
      result.project = {
        projectFile:
          'Error: Project is required but not specified in react-native.config.',
      };
    } else if (!userConfig.project) {
      result.project = {
        projectFile: 'Error: Project is null in react-native.config.',
      };
    } else {
      if (!('projectFile' in userConfig.project)) {
        result.project = {
          projectFile:
            'Error: Project file is required for project in react-native.config.',
        };
      } else if (userConfig.project.projectFile === null) {
        result.project = {
          projectFile: 'Error: Project file is null in react-native.config.',
        };
      } else {
        result.project = {
          projectFile: path.normalize(userConfig.project.projectFile),
        };
        validProject = true;
      }
    }

    if ('useWinUI3' in userConfig) {
      result.useWinUI3 = userConfig.useWinUI3;
    }
  } else {
    // No manually provided solutionFile, try to find it
    const foundSolutions = configUtils.findSolutionFiles(sourceDir);
    if (foundSolutions.length === 0) {
      result.solutionFile =
        'Error: No app solution file found, please specify in react-native.config.';
    } else if (foundSolutions.length > 1) {
      result.solutionFile =
        'Error: Too many app solution files found, please specify in react-native.config.';
    } else {
      result.solutionFile = path.normalize(foundSolutions[0]);
      validSolution = true;
    }

    // No manually provided project, try to find it
    const foundProjects = configUtils.findAppProjectFiles(sourceDir);
    if (foundProjects.length === 0) {
      result.project = {
        projectFile:
          'Error: No app project file found, please specify in react-native.config.',
      };
    } else if (foundProjects.length > 1) {
      result.project = {
        projectFile:
          'Error: Too many app project files found, please specify in react-native.config.',
      };
    } else {
      result.project = {
        projectFile: path.normalize(foundProjects[0]),
      };
      validProject = true;
    }
  }

  if (validSolution) {
    result.solutionFile = path.relative(
      sourceDir,
      path.join(sourceDir, result.solutionFile),
    );
  }

  if (validProject) {
    const projectFile = path.join(sourceDir, result.project.projectFile!);
    const projectContents = configUtils.readProjectFile(projectFile);

    result.project.projectFile = path.relative(sourceDir, projectFile);

    // Add missing (auto) items
    result.project.projectName = configUtils.getProjectName(
      projectFile,
      projectContents,
    );
    result.project.projectLang = configUtils.getProjectLanguage(projectFile);
    result.project.projectGuid = configUtils.getProjectGuid(projectContents);
  }

  return result as WindowsProjectConfig;
}
