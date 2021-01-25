/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as t from 'io-ts';

/**
 * Events that map be fired from functions to actors, including the expected
 * event payload
 */
export const actorEvents = {
  'integration-timer-fired': t.undefined,
  'publish-pipeline-failed': t.type({
    buildNumber: t.string,
    buildUri: t.string,
  }),
};
