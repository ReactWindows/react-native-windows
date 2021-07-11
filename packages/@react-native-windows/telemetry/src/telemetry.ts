/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';
import {randomBytes} from 'crypto';
import * as appInsights from 'applicationinsights';
import {execSync} from 'child_process';
import {CodedError} from './CodedError';

export class Telemetry {
  static client?: appInsights.TelemetryClient | undefined = undefined;

  static disable() {
    if (Telemetry.client) {
      Telemetry.client.config.disableAppInsights = true;
    }
    Telemetry.shouldDisable = true;
  }

  static setup(preserveMessages?: boolean) {
    if (Telemetry.isCI()) {
      this.disable();
      return;
    }
    if (Telemetry.client) {
      return;
    }
    if (!process.env.RNW_CLI_TEST) {
      appInsights.Configuration.setInternalLogging(false, false);
    }
    appInsights.setup('8a1a1649-99ea-41ed-9d7f-b3748b9d64d3');
    Telemetry.client = appInsights.defaultClient;

    if (Telemetry.shouldDisable) {
      Telemetry.disable();
    }
    Telemetry.preserveMessages = preserveMessages ? true : false;

    if (process.env.RNW_CLI_TEST) {
      Telemetry.client.commonProperties.isTest = process.env.RNW_CLI_TEST;
    }
    if (!Telemetry.client.commonProperties.sessionId) {
      Telemetry.client.commonProperties.sessionId = randomBytes(16).toString(
        'hex',
      );
      Telemetry.client.addTelemetryProcessor(sanitizeEnvelope);
    }
  }

  static isCI(): boolean {
    return (
      process.env.AGENT_NAME !== undefined || // Azure DevOps
      process.env.CIRCLECI === 'true' || // CircleCI
      process.env.TRAVIS === 'true' || // Travis
      process.env.CI === 'true' // other CIs
    );
  }

  static trackException(e: Error, properties?: Record<string, any>) {
    const props: Record<string, any> = {};
    if (e instanceof CodedError) {
      Object.assign(props, (e as CodedError).data);
    }

    const syscallExceptionFieldsToCopy = ['errno', 'syscall', 'code'];
    for (const f of syscallExceptionFieldsToCopy) {
      if ((e as any)[f]) {
        props[f] = [f];
      }
    }
    Object.assign(props, props, properties);
    Telemetry.client?.trackException({
      exception: e,
      properties: props,
    });
  }

  static shouldDisable: boolean = false;
  static preserveMessages: boolean = false;
}

function getAnonymizedPath(filepath: string): string {
  const projectRoot = process.cwd().toLowerCase();
  filepath = filepath.replace(/\//g, '\\');
  const knownPathsVars = ['AppData', 'LocalAppData', 'UserProfile'];
  if (filepath.toLowerCase().startsWith(projectRoot)) {
    const ext = path.extname(filepath);
    const rest = filepath.slice(projectRoot.length);
    const nodeModules = '\\node_modules\\';
    // this is in the project dir but not under node_modules
    if (rest.toLowerCase().startsWith('\\windows\\')) {
      return `[windows]\\???${ext}(${filepath.length})`;
    } else if (rest.toLowerCase().startsWith(nodeModules)) {
      return 'node_modules' + rest.slice(nodeModules.length - 1);
    } else {
      return `[project_dir]\\???${ext}(${filepath.length})`;
    }
  } else {
    for (const knownPath of knownPathsVars) {
      if (
        process.env[knownPath] &&
        filepath.toLowerCase().startsWith(process.env[knownPath]!.toLowerCase())
      ) {
        return `[${knownPath}]\\???(${filepath.length})`;
      }
    }
  }
  return '[path]';
}
/**
 * Sanitize any paths that appear between quotes (''), brackets ([]), or double quotes ("").
 * @param msg the string to sanitize
 */
export function sanitizeMessage(msg: string): string {
  const cpuThreadId = /^\d+(:\d+)?>/g;
  msg = msg.replace(cpuThreadId, '');
  const parts = msg.split(/['[\]"]/g);
  const clean = [];
  const pathRegEx = /([A-Za-z]:|\\)[\\/]([^<>:;,?"*\t\r\n|/\\]+[\\/])+([^<>:;,?"*\t\r\n|]+\/?)/gi;
  for (const part of parts) {
    if (pathRegEx.test(part)) {
      pathRegEx.lastIndex = -1;
      let matches: RegExpExecArray | null;
      let noPath = '';
      let last = 0;
      while ((matches = pathRegEx.exec(part))) {
        noPath +=
          part.substr(last, matches!.index - last) +
          getAnonymizedPath(matches[0]);
        last = matches!.index + matches![0].length;
      }
      clean.push(noPath);
    } else if (part !== '') {
      clean.push(part);
    }
  }
  return clean.join(' ').trim();
}

export function sanitizeFrame(frame: any): void {
  const parens = frame.method.indexOf('(');
  if (parens !== -1) {
    // case 1: method === 'methodName (rootOfThePath'
    frame.method = frame.method.substr(0, parens).trim();
  } else {
    // case 2: method === <no_method> or something without '(', fileName is full path
  }
  // preserve only the last_directory/filename
  frame.fileName = path.join(
    path.basename(path.dirname(frame.fileName)),
    path.basename(frame.fileName),
  );
  frame.assembly = '';
}

export function tryGetErrorCode(msg: string): string | undefined {
  const errorRegEx = /error (\w+\d+):/gi;
  const m = errorRegEx.exec(msg);
  return m ? m[1] : undefined;
}

/**
 * Remove PII from exceptions' stack traces and messages
 * @param envelope the telemetry envelope. Provided by AppInsights.
 */
export function sanitizeEnvelope(envelope: any /*context: any*/): boolean {
  if (envelope.data.baseType === 'ExceptionData') {
    const data = envelope.data.baseData;
    for (const exception of data.exceptions || []) {
      for (const frame of exception.parsedStack) {
        sanitizeFrame(frame);
      }
      const errorCode = tryGetErrorCode(exception.message);
      data.properties.errorCode = errorCode;
      // CodedError has non-PII information in its 'type' member, plus optionally some more info in its 'data'.
      // The message may contain PII information. This can be sanitized, but for now delete it.
      // Note that the type of data.exceptions[0] is always going to be ExceptionDetails. It is not the original thrown exception.
      // https://github.com/microsoft/ApplicationInsights-node.js/issues/707
      if (Telemetry.preserveMessages) {
        exception.message = sanitizeMessage(exception.message);
      } else {
        delete exception.message;
      }
    }
  }
  delete envelope.tags['ai.cloud.roleInstance'];
  return true;
}

export function isMSFTInternal(): boolean {
  return (
    process.env.UserDNSDomain !== undefined &&
    process.env.UserDNSDomain.toLowerCase().endsWith('.microsoft.com')
  );
}

export function getDiskFreeSpace(drivePath: string | null): number {
  const out = execSync(`dir /-C ${drivePath}`)
    .toString()
    .split('\r\n');
  const line = out[out.length - 2];
  const result = line.match(/(\d+) [^\d]+(\d+) /);
  if (result && result.length > 2) {
    return Number(result[2]);
  }
  return -1;
}
