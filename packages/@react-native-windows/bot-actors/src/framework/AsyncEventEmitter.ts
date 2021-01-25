/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Mapping of allowable events to listener functions (with parameters)
 */
export type EventCollection = Record<string, (...args: any[]) => Promise<void>>;

/**
 * Minimal Node EventEmitter-like interface which allows emitting events to
 * registered listeners. Emitting returns a promise that is fulfilled when
 * every handler has completed.
 */
export class AsyncEventEmitter<TEventCollection extends EventCollection> {
  private readonly listeners: {
    [TEventName in keyof TEventCollection]?: Array<
      TEventCollection[TEventName]
    >;
  } = {};

  on: OnAsyncEmitterEvent<TEventCollection> = (eventName, listener) => {
    if (!this.listeners[eventName]) {
      this.listeners[eventName] = [];
    }

    this.listeners[eventName]!.push(listener);
    return this;
  };

  emit: EmitAsyncEmitterEvent<TEventCollection> = async (
    eventName,
    ...args
  ) => {
    const listeners = this.listeners[eventName];
    if (!listeners || listeners.length === 0) {
      return false;
    }

    for (const listener of listeners!) {
      await listener(...args);
    }

    return true;
  };
}

export type OnAsyncEmitterEvent<TEventCollection extends EventCollection> = <
  TEventName extends keyof TEventCollection
>(
  eventName: TEventName,
  listener: TEventCollection[TEventName],
) => AsyncEmitterSource<TEventCollection>;

export type EmitAsyncEmitterEvent<TEventCollection extends EventCollection> = <
  TEventName extends keyof TEventCollection
>(
  eventName: TEventName,
  ...args: Parameters<TEventCollection[TEventName]>
) => Promise<boolean>;

export type AsyncEmitterSource<TEventCollection extends EventCollection> = {
  on: OnAsyncEmitterEvent<TEventCollection>;
};

export type AsyncEmitterSink<TEventCollection extends EventCollection> = {
  emit: EmitAsyncEmitterEvent<TEventCollection>;
};
