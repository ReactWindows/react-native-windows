/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import fs from 'fs';
import path from 'path';
import glob from 'glob';

import {DOMParser} from 'xmldom';
import xpath from 'xpath';
import {CodedError} from '@react-native-windows/telemetry';

const msbuildSelect = xpath.useNamespaces({
  msbuild: 'http://schemas.microsoft.com/developer/msbuild/2003',
});

/**
 * Search for files matching the pattern under the target folder.
 * @param folder The absolute path to target folder.
 * @param filenamePattern The pattern to search for.
 * @return  Return the array of relative file paths.
 */
export function findFiles(folder: string, filenamePattern: string): string[] {
  const files = glob.sync(path.join('**', filenamePattern), {
    cwd: folder,
    ignore: [
      'node_modules/**',
      '**/Debug/**',
      '**/Release/**',
      '**/Generated Files/**',
      '**/packages/**',
    ],
  });

  return files;
}

/**
 * Search for the windows sub-folder under the target folder.
 * @param folder The absolute path to the target folder.
 * @return The absolute path to the windows folder, if it exists.
 */
export function findWindowsFolder(folder: string): string | null {
  const winDir = 'windows';
  const joinedDir = path.join(folder, winDir);
  if (fs.existsSync(joinedDir)) {
    return joinedDir;
  }

  return null;
}

/**
 * Checks if the target file path is a RNW solution file by checking if it contains the string "ReactNative".
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW solution file.
 */
export function isRnwSolution(filePath: string): boolean {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/ReactNative/) > 0
  );
}

/**
 * Search for the RNW solution files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
 */
export function findSolutionFiles(winFolder: string): string[] {
  // First search for all potential solution files
  const allSolutions = findFiles(winFolder, '*.sln');

  if (allSolutions.length === 0) {
    // If there're no solution files, return 0
    return [];
  } else if (allSolutions.length === 1) {
    // If there is exactly one solution file, assume it's it
    return [allSolutions[0]];
  }

  const solutionFiles = [];

  // Try to find any solution file that appears to be a React Native solution
  for (const solutionFile of allSolutions) {
    if (isRnwSolution(path.join(winFolder, solutionFile))) {
      solutionFiles.push(path.normalize(solutionFile));
    }
  }

  return solutionFiles;
}

/**
 * Checks if the target file path is a RNW lib project file.
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW lib project file.
 */
export function isRnwDependencyProject(filePath: string): boolean {
  const projectContents = readProjectFile(filePath);

  const projectLang = getProjectLanguage(filePath);
  if (projectLang === 'cs') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CSharpLib.targets',
    );
  } else if (projectLang === 'cpp') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CppLib.targets',
    );
  }

  return false;
}

/**
 * Search for the RNW lib project files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
 */
export function findDependencyProjectFiles(winFolder: string): string[] {
  // First, search for all potential project files
  const allCppProj = findFiles(winFolder, '*.vcxproj');
  const allCsProj = findFiles(winFolder, '*.csproj');

  const allProjects = allCppProj.concat(allCsProj);

  if (allProjects.length === 0) {
    // If there're no project files, return 0
    return [];
  }

  const dependencyProjectFiles = [];

  // Try to find any project file that appears to be a dependency project
  for (const projectFile of allProjects) {
    // A project is marked as a RNW dependency iff either:
    // - If the project has the standard native module imports, or
    // - If we only have a single project (and it doesn't have the standard native module imports),
    // pick it and hope for the best. This enables autolinking for modules that were written
    // before the standard native module template existed.
    if (
      allProjects.length === 1 ||
      isRnwDependencyProject(path.join(winFolder, projectFile))
    ) {
      dependencyProjectFiles.push(path.normalize(projectFile));
    }
  }

  return dependencyProjectFiles;
}

type ReactNativeProjectType = 'unknown' | 'App-Reunion';

function getReactNativeProjectType(
  value: string | null,
): ReactNativeProjectType {
  switch (value) {
    case 'App-Reunion':
      return value;

    default:
      return 'unknown';
  }
}

/**
 * Checks if the target file path is a RNW app project file.
 * @param filePath The absolute file path to check.
 * @return Whether the path is to a RNW app project file.
 */
function isRnwAppProject(filePath: string): boolean {
  const projectContents = readProjectFile(filePath);

  const rnProjectType = getReactNativeProjectType(
    tryFindPropertyValue(projectContents, 'ReactNativeProjectType'),
  );

  if (rnProjectType !== 'unknown') {
    return true;
  }

  const projectLang = getProjectLanguage(filePath);
  if (projectLang === 'cs') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CSharpApp.targets',
    );
  } else if (projectLang === 'cpp') {
    return importProjectExists(
      projectContents,
      'Microsoft.ReactNative.Uwp.CppApp.targets',
    );
  }

  return false;
}

/**
 * Search for the RNW app project files under the target folder.
 * @param winFolder The absolute path to target folder.
 * @return Return the array of relative file paths.
 */
export function findAppProjectFiles(winFolder: string): string[] {
  // First, search for all potential project files
  const allCppProj = findFiles(winFolder, '*.vcxproj');
  const allCsProj = findFiles(winFolder, '*.csproj');

  const allProjects = allCppProj.concat(allCsProj);

  if (allProjects.length === 0) {
    // If there're no project files, return 0
    return [];
  }

  const appProjectFiles = [];

  // Try to find any project file that appears to be an app project
  for (const projectFile of allProjects) {
    if (isRnwAppProject(path.join(winFolder, projectFile))) {
      appProjectFiles.push(path.normalize(projectFile));
    }
  }

  return appProjectFiles;
}

/**
 * Returns the programming language of the project file.
 * @param projectPath The project file path to check.
 * @return The language string: cpp, cs, or null if unknown.
 */
export function getProjectLanguage(projectPath: string): 'cpp' | 'cs' | null {
  if (projectPath.endsWith('.vcxproj')) {
    return 'cpp';
  } else if (projectPath.endsWith('.csproj')) {
    return 'cs';
  }

  return null;
}

/**
 * Reads in the contents of the target project file.
 * @param projectPath The target project file path.
 * @return The project file contents.
 */
export function readProjectFile(projectPath: string) {
  const projectContents = fs.readFileSync(projectPath, 'utf8').toString();
  return new DOMParser().parseFromString(projectContents, 'application/xml');
}

/**
 * Search for the given property in the project contents and return its value.
 * @param projectContents The XML project contents.
 * @param propertyName The property to look for.
 * @return The value of the tag if it exists.
 */
export function tryFindPropertyValue(
  projectContents: Node,
  propertyName: string,
): string | null {
  const nodes = msbuildSelect(
    `//msbuild:PropertyGroup/msbuild:${propertyName}`,
    projectContents,
  );

  if (nodes.length > 0) {
    // Take the last one
    return (nodes[nodes.length - 1] as Node).textContent;
  } else {
    const noNamespaceNodes = xpath.select(
      `//PropertyGroup/${propertyName}`,
      projectContents,
    );
    if (noNamespaceNodes.length > 0) {
      return (noNamespaceNodes[noNamespaceNodes.length - 1] as Node)
        .textContent;
    }
  }

  return null;
}

export function findPropertyValue(
  projectContents: Node,
  propertyName: string,
  filePath: string,
): string {
  const res = tryFindPropertyValue(projectContents, propertyName);
  if (!res) {
    throw new CodedError(
      'NoPropertyInProject',
      `Couldn't find property ${propertyName} from ${filePath}`,
      {propertyName: propertyName},
    );
  }
  return res;
}

/**
 * Search for the given import project in the project contents and return if it exists.
 * @param projectContents The XML project contents.
 * @param projectName The project to look for.
 * @return If the target exists.
 */
export function importProjectExists(
  projectContents: Node,
  projectName: string,
): boolean {
  const nodes = msbuildSelect(
    `//msbuild:Import[contains(@Project,'${projectName}')]`,
    projectContents,
  );

  return nodes.length > 0;
}

export type ConfigurationType =
  | 'application'
  | 'dynamiclibrary'
  | 'generic'
  | 'staticlibrary'
  | 'unknown';

/**
 * Gets the configuration type of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project configuration type.
 */
export function getConfigurationType(projectContents: Node): ConfigurationType {
  const configurationType = tryFindPropertyValue(
    projectContents,
    'ConfigurationType',
  )?.toLowerCase();

  switch (configurationType) {
    case 'application':
    case 'dynamiclibrary':
    case 'generic':
    case 'staticlibrary':
      return configurationType;

    default:
      return 'unknown';
  }
}

export type OutputType =
  | 'appcontainerexe'
  | 'exe'
  | 'library'
  | 'module'
  | 'unknown'
  | 'winexe'
  | 'winmdobj';

/**
 * Gets the output type of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project output type.
 */
export function getOutputType(projectContents: Node): OutputType {
  const outputType = tryFindPropertyValue(
    projectContents,
    'OutputType',
  )?.toLowerCase();

  switch (outputType) {
    case 'appcontainerexe':
    case 'exe':
    case 'library':
    case 'module':
    case 'winexe':
    case 'winmdobj':
      return outputType;

    default:
      return 'unknown';
  }
}

/**
 * Gets the type of the project from the project contents.
 * @param projectPath The project file path to check.
 * @param projectContents The XML project contents.
 * @return The project type.
 */
export function getProjectType(
  projectPath: string,
  projectContents: Node,
): ConfigurationType | OutputType {
  switch (getProjectLanguage(projectPath)) {
    case 'cpp':
      return getConfigurationType(projectContents);

    case 'cs':
      return getOutputType(projectContents);

    default:
      return 'unknown';
  }
}

/**
 * Gets the name of the project from the project contents.
 * @param projectPath The project file path to check.
 * @param projectContents The XML project contents.
 * @return The project name.
 */
export function getProjectName(
  projectPath: string,
  projectContents: Node,
): string {
  const name =
    tryFindPropertyValue(projectContents, 'ProjectName') ||
    path.parse(projectPath).name ||
    '';

  return name;
}

/**
 * Gets the namespace of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project namespace.
 */
export function getProjectNamespace(projectContents: Node): string | null {
  return tryFindPropertyValue(projectContents, 'RootNamespace');
}

/**
 * Gets the guid of the project from the project contents.
 * @param projectContents The XML project contents.
 * @return The project guid.
 */
export function getProjectGuid(projectContents: Node): string | null {
  return tryFindPropertyValue(projectContents, 'ProjectGuid');
}
