/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {WebhookEvent} from '@octokit/webhooks';

import {ActorEventEmitter} from './ActorEvents';
import {ActorInstance} from './ActorInstance';

/**
 * ActorsHandle allows external interaction with the collection of initialized
 * actors, such as triggering events
 */
export type ActorsHandle = {
  emitEvent: ActorEventEmitter['emit'];
  receiveWebhook: (event: WebhookEvent) => Promise<void>;
};

export class ActorsHandleImpl implements ActorsHandle {
  constructor(private readonly instances: ActorInstance[]) {}

  /**
   * Emits an event to subscribing actors
   */
  emitEvent: ActorEventEmitter['emit'] = async (eventName, ...args) => {
    let eventReceived = false;
    for (const instance of this.instances) {
      eventReceived =
        eventReceived || (await instance.emitEvent(eventName, ...args));
    }

    return eventReceived;
  };

  /**
   * Dispatch a webhook event to subscribing actors. It is assumed the payload
   * has already been validated as signed.
   */
  receiveWebhook = async (event: WebhookEvent) => {
    for (const instance of this.instances) {
      await instance.receiveWebhook(event);
    }
  };
}
