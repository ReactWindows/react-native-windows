/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {CommandOption} from '@react-native-community/cli-types';

export type BuildArch = 'x86' | 'x64' | 'ARM64';
export type BuildConfig = 'Debug' | 'DebugBundle' | 'Release' | 'ReleaseBundle';

/**
 *  * Options are the following:
 *    release: Boolean - Specifies release build
 *    root: String - The root of the application
 *    arch: String - The build architecture (ARM64, x86, x64)
 *    singleproc: Boolean - opt out of multi-proc builds
 *    emulator: Boolean - Deploy to the emulator
 *    device: Boolean - Deploy to a device
 *    target: String - Device GUID to deploy to
 *    remote-debugging: Boolean - Run using remote JS proxy
 *    logging: Boolean - Enables logging
 *    no-packager: Boolean - Do not launch packager while building
 *    bundle: Boolean - Enable Bundle configuration.
 *    no-launch: Boolean - Do not launch the app after deployment
 *    no-build: Boolean - Do not build the solution
 *    no-deploy: Boolean - Do not deploy the app
 *    deploy-from-layout: Force deploy from layout, even in release builds
 *    sln: String - Solution file to build
 *    msbuildprops: String - Comma separated props to pass to msbuild, eg: prop1=value1,prop2=value2
 *    direct-debugging: Number - Enable direct debugging on specified port
 *    no-telemetry: Boolean - Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI
 */
export interface RunWindowsOptions {
  release?: boolean;
  root: string;
  arch: BuildArch;
  singleproc?: boolean;
  emulator?: boolean;
  device?: boolean;
  target?: string;
  remoteDebugging?: string;
  logging: boolean;
  packager: boolean;
  bundle: boolean;
  launch: boolean;
  autolink: boolean;
  build: boolean;
  deploy: boolean;
  deployFromLayout?: boolean;
  sln?: string;
  proj?: string;
  msbuildprops?: string;
  buildLogDirectory?: string;
  info: boolean;
  directDebugging?: number;
  telemetry?: boolean;
}

export const runWindowsOptions: CommandOption[] = [
  {
    name: '--release',
    description: 'Specifies a release build',
  },
  {
    name: '--root [string]',
    description:
      'Override the root directory for the windows build which contains the windows folder.',
    default: config => config.root,
  },
  {
    name: '--arch [string]',
    description: 'The build architecture (ARM64, x86, x64)',
    default: 'x86',
    parse: parseBuildArch,
  },
  {
    name: '--singleproc',
    description: 'Disables multi-proc build',
  },
  {
    name: '--emulator',
    description: 'Deploys the app to an emulator',
  },
  {
    name: '--device',
    description: 'Deploys the app to a connected device',
  },
  {
    name: '--target [string]',
    description: 'Deploys the app to the specified GUID for a device.',
  },
  {
    name: '--remote-debugging',
    description: 'Deploys the app in remote debugging mode.',
  },
  {
    name: '--logging',
    description: 'Enables logging',
    default: false,
  },
  {
    name: '--no-packager',
    description: 'Do not launch packager while building',
    default: false,
  },
  {
    name: '--bundle',
    description:
      'Enable Bundle configuration and it would be ReleaseBundle/DebugBundle other than Release/Debug',
    default: false,
  },
  {
    name: '--no-launch',
    description: 'Do not launch the app after deployment',
    default: false,
  },
  {
    name: '--no-autolink',
    description: 'Do not run autolinking',
    default: false,
  },
  {
    name: '--no-build',
    description: 'Do not build the solution',
    default: false,
  },
  {
    name: '--no-deploy',
    description: 'Do not deploy the app',
    default: false,
  },
  {
    name: '--deploy-from-layout',
    description: 'Force deploy from layout, even in release builds',
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
  {
    name: '--msbuildprops [string]',
    description:
      'Comma separated props to pass to msbuild, eg: prop1=value1,prop2=value2',
  },
  {
    name: '--buildLogDirectory [string]',
    description: 'Optional directory where msbuild log files should be stored',
  },
  {
    name: '--info',
    description: 'Dump environment information',
    default: false,
  },
  {
    name: '--direct-debugging [number]',
    description: 'Enable direct debugging on specified port',
    parse: parseDirectDebuggingPort,
  },
  {
    name: '--no-telemetry',
    description:
      'Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI',
    default: false,
  },
];

function parseBuildArch(arg: string): BuildArch {
  const supportedArches: BuildArch[] = ['x86', 'x64', 'ARM64'];
  for (const supported of supportedArches) {
    if (arg.toLowerCase() === supported.toLowerCase()) {
      return supported;
    }
  }

  errorOut(`Unrecognized --arch '${arg}'. Expected one of ${supportedArches}`);
}

function parseDirectDebuggingPort(arg: string): number {
  const num = parseInt(arg, 10);

  if (!Number.isInteger(num)) {
    errorOut(`Expected argument '--direct-debugging' to be a number`);
  }
  if (num < 1024 || num >= 65535) {
    errorOut('Direct debugging port it out of range');
  }

  return num;
}

function errorOut(arg: string): never {
  console.error(arg);
  process.exit(1);
}
