/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Webhooks, WebhookEvent} from '@octokit/webhooks';
import {Octokit} from '@octokit/rest';
import {retry} from '@octokit/plugin-retry';
import {throttling} from '@octokit/plugin-throttling';

import {Logger, Secrets} from './initializeActors';
import {ActorDefinition} from './ActorRegistry';
import {AsyncEventEmitter} from './AsyncEventEmitter';
import {ActorEventEmitter} from './ActorEvents';
import {Context} from './Context';
export class ActorInstance {
  private constructor(
    private readonly logger: Logger,
    private readonly octokit: Octokit,
    private readonly webhooks: Webhooks,
  ) {}

  eventEmitter: ActorEventEmitter = new AsyncEventEmitter();

  public static async initialize(
    actorName: string,
    actor: ActorDefinition,
    logger: Logger,
    secrets: Secrets,
  ) {
    const decoratedLogger = decorateWithPrefix(logger, actorName);
    const octokit = createOctokit(decoratedLogger, secrets);
    const webhooks = new Webhooks({secret: secrets.githubWebhookSecret});

    const instance = new ActorInstance(decoratedLogger, octokit, webhooks);

    await actor(instance.context());
    return instance;
  }

  context(): Context {
    return {
      log: this.logger,
      octokit: this.octokit,
      gitHooks: this.webhooks,
      events: this.eventEmitter,
    };
  }

  emitEvent: ActorEventEmitter['emit'] = async (eventName, ...args) => {
    this.logger.verbose(`Evaluating event "${eventName}"`);
    if (await this.eventEmitter.emit(eventName, ...args)) {
      this.logger.info(`Finished event "${eventName}"`);
      return true;
    } else {
      this.logger.verbose(`Rejected event "${eventName}"`);
      return false;
    }
  };

  receiveWebhook: (event: WebhookEvent) => Promise<void> = async event => {
    this.logger.verbose(`Evaluating webhook "${webhookToString(event)}"`);
    await this.webhooks.receive(event);
  };
}

function webhookToString(event: WebhookEvent): string {
  if (event.payload.action) {
    return `${event.name}.${event.payload.action}`;
  } else {
    return event.name;
  }
}

function decorateWithPrefix(log: Logger, prefix: string): Logger {
  return {
    error: (...args: any[]) => log.error(`[${prefix}]`, ...args),
    warn: (...args: any[]) => log.warn(`[${prefix}]`, ...args),
    info: (...args: any[]) => log.info(`[${prefix}]`, ...args),
    verbose: (...args: any[]) => log.verbose(`[${prefix}]`, ...args),
  };
}

type OctokitThrottleOptions = {
  request: {retryCount: number};
  method: string;
  url: string;
};

function createOctokit(logger: Logger, secrets: Secrets): Octokit {
  const ActorOctokit = Octokit.plugin(retry, throttling);

  return new ActorOctokit({
    auth: secrets.githubAuthToken,
    log: {
      ...logger,
      debug: logger.verbose,
    },
    throttle: {
      onRateLimit: (retryAfter: number, options: OctokitThrottleOptions) => {
        logger.warn(
          `Request quota exhausted for request ${options.method} ${options.url}`,
        );

        if (options.request.retryCount < 3) {
          logger.info(`Retrying after ${retryAfter} seconds!`);
          return true;
        } else {
          logger.error(`Maximum retries exceeded`);
          return false;
        }
      },
      onAbuseLimit: (_retryAfter: number, options: OctokitThrottleOptions) => {
        logger.error(
          `Abuse limit hit for request ${options.method} ${options.url}`,
        );
        return false;
      },
    },
  });
}
