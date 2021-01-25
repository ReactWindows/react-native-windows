/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {botFunction} from '../botFunction';

export default botFunction(async ({actors}) => {
  await actors.emitEvent('integration-timer-fired');
});
