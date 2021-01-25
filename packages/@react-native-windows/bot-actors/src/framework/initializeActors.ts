/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
import * as globby from 'globby';

import {ActorInstance} from './ActorInstance';
import {ActorRegistry} from './ActorRegistry';
import {ActorsHandle, ActorsHandleImpl} from './ActorsHandle';

/**
 * Log output for the actors system
 */
export type Logger = {
  verbose: (...args: any) => void;
  info: (...args: any) => void;
  warn: (...args: any) => void;
  error: (...args: any) => void;
};

/**
 * Secrets required by actor services to function
 */
export type Secrets = {
  githubAuthToken: string;
  githubWebhookSecret: string;
};

/**
 * Attempts to perform global registration of all actors present, then runs
 * them to create event subscriptions. Returns the ActorsHandle to allow
 * functions to trigger an event to actors.
 */
export async function initializeActors(
  logger: Logger,
  secrets: Secrets,
): Promise<ActorsHandle> {
  const actorSources = await globby(['*.js'], {
    cwd: `${__dirname}/..`,
    absolute: true,
  });
  for (const sourceFile of actorSources) {
    logger.verbose(`Loading "${sourceFile}"`);
    require(sourceFile);
  }

  const actorInstances: ActorInstance[] = [];
  for (const [actorName, actor] of Object.entries(ActorRegistry.getActors())) {
    actorInstances.push(
      await ActorInstance.initialize(actorName, actor, logger, secrets),
    );
  }

  return new ActorsHandleImpl(actorInstances);
}
