/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Context} from './Context';

/**
 * Actor functions allow creating flows that react to external events, such as
 * GitHub events or function triggers
 */
export type ActorDefinition = (context: Context) => Promise<void>;

const globallyRegisteredActors: Record<string, ActorDefinition> = {};

/**
 * Public access point to globally register actors
 */
export const ActorRegistry = {
  register: (actorName: string, actor: ActorDefinition): ActorDefinition => {
    if (globallyRegisteredActors.hasOwnProperty(actorName)) {
      throw new Error(`Actor "${actorName}" has already been registered`);
    }

    globallyRegisteredActors[actorName] = actor;
    return actor;
  },

  getActors: (): Record<string, ActorDefinition> => {
    return {...globallyRegisteredActors};
  },
};
