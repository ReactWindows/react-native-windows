/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as t from 'io-ts';

import {AsyncEventEmitter} from './AsyncEventEmitter';
import {actorEvents} from '../Events';
export {actorEvents};

export type ActorEventName = keyof typeof actorEvents;

type EventType<T extends ActorEventName> = t.TypeOf<typeof actorEvents[T]>;

export type ActorEventCallbacks = {
  [E in keyof typeof actorEvents]: EventType<E> extends undefined
    ? () => Promise<void>
    : (args: EventType<E>) => Promise<void>;
};

export type ActorEventEmitter = AsyncEventEmitter<ActorEventCallbacks>;
