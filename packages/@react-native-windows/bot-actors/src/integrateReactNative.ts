/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {ActorDefinition, ActorRegistry} from './framework';

/**
 * Actor to trigger automated integration of newly published react-native builds
 */
const actor: ActorDefinition = async ({events}) => {
  events.on('integration-timer-fired', async () => {
    // Not yet implemented
  });
};

export default ActorRegistry.register('integrateReactNative', actor);
