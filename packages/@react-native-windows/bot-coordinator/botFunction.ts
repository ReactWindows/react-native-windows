/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as crypto from 'crypto';

import {
  ActorsHandle,
  initializeActors,
  Secrets as ActorSecrets,
} from '@react-native-windows/bot-actors';
import {AzureFunction, Context, HttpRequest} from '@azure/functions';

export type ActorFunctionDelegate = (opts: {
  context: Context;
  secrets: Secrets;
  actors: ActorsHandle;
}) => Promise<void>;

export type Secrets = {
  /** PAT used to authenticate to GitHub's REST APIs */
  githubAuthToken: string;
  /** HMAC SHA-256 secret used to validate incoming messages */
  httpSharedSecret: string;
};

/**
 * Wrapper to do verification and initialization for parameterless
 */
export function botFunction(delegate: ActorFunctionDelegate): AzureFunction {
  return async (context: Context) => {
    const secrets = getSecrets();

    const actorsHandle = await initializeActors(context.log, secrets);
    await delegate({context, secrets, actors: actorsHandle});
  };
}

export type HttpFunctionDelegate = (opts: {
  context: Context;
  secrets: Secrets;
  actors: ActorsHandle;
  req: HttpRequest;
}) => Promise<void>;

/**
 * Wrapper to do verification and initialization for HTTP Azure functions
 */
export function httpBotFunction(delegate: HttpFunctionDelegate) {
  return async (context: Context, req: HttpRequest) => {
    const secrets = getSecrets();
    const enableSignatureVerification =
      process.env.ENABLE_SIGNATURE_VERIFICATION !== 'false';

    try {
      if (
        enableSignatureVerification &&
        !verifyMessageSignature(context, req, secrets)
      ) {
        context.log.error('Signature verification failed');
        context.res = {status: 403, body: 'Signature verification failed'};
        return;
      }

      const actors = await initializeActors(context.log, secrets);
      await delegate({context, secrets, actors, req});
    } catch (ex) {
      context.res = {status: 500};
      throw ex;
    }

    if (!context.res) {
      context.log.info('Request OK');
      context.res = {status: 200};
    }
  };
}

function getSecrets(): Secrets & ActorSecrets {
  const githubAuthToken = process.env.GITHUB_AUTH_TOKEN;
  if (githubAuthToken === undefined) {
    throw new Error('"GITHUB_AUTH_TOKEN" env variable must be set');
  }

  const httpSharedSecret = process.env.HTTP_SHARED_SECRET;
  if (httpSharedSecret === undefined) {
    throw new Error('"HTTP_SHARED_SECRET" env variable must be set');
  }

  return {
    githubAuthToken,
    httpSharedSecret,
    githubWebhookSecret: httpSharedSecret,
  };
}

/**
 * Verifies whether an HTTP request has been correctly signed with the
 * pre-shared key.
 */
function verifyMessageSignature(
  context: Context,
  req: HttpRequest,
  secrets: Secrets,
) {
  if (!req.rawBody) {
    context.log.error('No body present in HTTP request');
    return false;
  }

  const givenSignature =
    req.headers['X-Hub-Signature-256'] || req.headers['X-Bot-Signature-256'];
  if (!givenSignature) {
    context.log.error('No signature header present in HTTP request');
    return false;
  }

  const hmac = crypto.createHmac('sha256', secrets.httpSharedSecret);
  hmac.update(req.rawBody);
  const expectedSignature = hmac.digest('hex');

  if (givenSignature.toLowerCase() !== expectedSignature.toLowerCase()) {
    context.log.error('Payload signature did not match expectations');
    return false;
  }

  return true;
}
