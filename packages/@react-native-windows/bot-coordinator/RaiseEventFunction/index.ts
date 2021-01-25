/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as _ from 'lodash';

import {Logger} from '@azure/functions';
import {PathReporter, success} from 'io-ts/lib/PathReporter';
import {actorEvents, ActorEventName} from '@react-native-windows/bot-actors';
import {httpBotFunction} from '../botFunction';

export default httpBotFunction(async ({context, req, actors}) => {
  const bodyJson =
    typeof req.body === 'string' ? JSON.parse(req.body) : req.body;
  const event = parseEvent(bodyJson, context.log);
  if (!event) {
    context.log.error('JSON schema validation failed');
    context.res = {status: 400, body: 'Malformed event'};
    return;
  }

  await actors.emitEvent(event.name, event.payload);
});

/**
 * Parse a JSON body into event emitter params
 */
function parseEvent(
  bodyJson: any,
  log: Logger,
): {name: ActorEventName; payload?: any} | null {
  if (typeof bodyJson.event !== 'string') {
    log.error('Missing event name');
    return null;
  }

  if (!actorEvents.hasOwnProperty(bodyJson.event)) {
    log.error('Unrecognized event name');
    return null;
  }

  // Check that JSON conforms to schema expectations
  const expectedPayload = actorEvents[bodyJson.event as ActorEventName];
  const result = PathReporter.report(expectedPayload.decode(bodyJson.payload));
  if (!_.isEqual(result, success())) {
    log.error('Event payload did not match expectations: ', ...result);
    return null;
  }

  return {name: bodyJson.event, payload: bodyJson.payload};
}
