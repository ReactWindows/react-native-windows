/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as fs from 'fs';
import * as path from 'path';
import * as findUp from 'find-up';

import * as appInsights from 'applicationinsights';

appInsights.setup('795006ca-cf54-40ee-8bc6-03deb91401c3');
const telClient = appInsights.defaultClient;

import {
  copyProjectTemplateAndReplace,
  installDependencies,
} from './generator-windows';

import {autoLinkCommand} from './runWindows/utils/autolink';
import {runWindowsCommand} from './runWindows/runWindows';
import {dependencyConfigWindows} from './config/dependencyConfig';
import {projectConfigWindows} from './config/projectConfig';


/**
 * Project generation options
 *
 *      _
 *     | |
 *   __| | __ _ _ __   __ _  ___ _ __
 *  / _` |/ _` | '_ \ / _` |/ _ \ '__|
 * | (_| | (_| | | | | (_| |  __/ |
 *  \__,_|\__,_|_| |_|\__, |\___|_|
 *                     __/ |
 *                    |___/
 *
 *
 * Properties on this interface must remain stable, as new versions of
 * react-native-windows-init may be used with local-cli dating back to 0.61.
 * All existing arguments must work correctly when changing this interface.
 */
export interface GenerateOptions {
  overwrite: boolean;
  language: 'cpp' | 'cs';
  projectType: 'app' | 'lib';
  experimentalNuGetDependency: boolean;
  nuGetTestVersion?: string;
  nuGetTestFeed?: string;
  useWinUI3: boolean;
  useHermes: boolean;
  verbose: boolean;
  noTelemetry: boolean;
}

function scrubOptions(opt: GenerateOptions) {
  return {
    overwrite: opt.overwrite,
    language: opt.language,
    projectType: opt.projectType,
    experimentalNuGetDependency: opt.experimentalNuGetDependency,
    nuGetTestFeed: opt.nuGetTestFeed ? true : false,
    nuGetTestVersion: opt.nuGetTestVersion ? true : false,
    useWinUI3: opt.useWinUI3,
    useHermes: opt.useHermes,
    verbose: opt.verbose,
  };
}

/**
 * Simple utility for running the Windows generator.
 *
 * @param  projectDir root project directory (i.e. contains index.js)
 * @param  name       name of the root JS module for this app
 * @param  ns         namespace for the project
 * @param  options    command line options container
 */
export async function generateWindows(
  projectDir: string,
  name: string,
  ns: string,
  options: GenerateOptions,
) {
  let error;
  try {
    if (!fs.existsSync(projectDir)) {
      fs.mkdirSync(projectDir);
    }

    installDependencies(options);

    const rnwPackage = path.dirname(
      require.resolve('react-native-windows/package.json', {
        paths: [projectDir],
      }),
    );
    const templateRoot = path.join(rnwPackage, 'template');
    await copyProjectTemplateAndReplace(
      templateRoot,
      projectDir,
      name,
      ns,
      options,
    );
  } catch (e) {
    error = e;
    throw e;
  } finally {
    if (!options.noTelemetry && !process.env.AGENT_NAME) {
      const cwd = process.cwd();
      const pkgJsonPath = findUp.sync('package.json', {cwd});
      let rnVersion = '';
      if (pkgJsonPath) {
        const pkgJson = require(pkgJsonPath);
        // check how react-native is installed
        if (
          'dependencies' in pkgJson &&
          'react-native' in pkgJson.dependencies
        ) {
          // regular dependency (probably an app), inject into json and run install
          rnVersion = pkgJson.dependencies['react-native'];
        }
      }

      const rnwPkgJson = require('../package.json');

      const optScrubbed = scrubOptions(options);
      telClient.trackEvent({
        name: 'generate-windows',
        properties: {
          error: error,
          ...optScrubbed,
          'react-native': rnVersion,
          'cli-version': rnwPkgJson.version,
        },
      });

      telClient.flush();
    }
  }
}

// Assert the interface here doesn't change for the reasons above
const assertStableInterface: typeof generateWindows extends (
  projectDir: string,
  name: string,
  ns: string,
  options: GenerateOptions,
) => Promise<void>
  ? true
  : never = true;
assertStableInterface;

export const commands = [autoLinkCommand, runWindowsCommand];
export const dependencyConfig = dependencyConfigWindows;
export const projectConfig = projectConfigWindows;
