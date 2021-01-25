/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {ActorDefinition, ActorInstance, Logger} from '../../framework';
import {failOnErrorLogger, fakeSecrets} from '../Mocks';

const emptyActor: ActorDefinition = async () => {};

test('Logger is decorated', async () => {
  const testLogger: Logger = {
    verbose: jest.fn(),
    info: jest.fn(),
    warn: jest.fn(),
    error: jest.fn(),
  };

  const actorInstance = await ActorInstance.initialize(
    'testActor',
    emptyActor,
    testLogger,
    fakeSecrets,
  );

  const {log} = actorInstance.context();

  log.verbose('verbose');
  expect(testLogger.verbose).toHaveBeenCalledWith('[testActor]', 'verbose');

  log.info('info');
  expect(testLogger.info).toHaveBeenCalledWith('[testActor]', 'info');

  log.warn('warn');
  expect(testLogger.warn).toHaveBeenCalledWith('[testActor]', 'warn');

  log.error('error');
  expect(testLogger.error).toHaveBeenCalledWith('[testActor]', 'error');
});

test('emitEvent - No Listeners', async () => {
  const actorInstance = await ActorInstance.initialize(
    'testActor',
    emptyActor,
    failOnErrorLogger,
    fakeSecrets,
  );

  expect(await actorInstance.emitEvent('integration-timer-fired')).toBe(false);
});

test('emitEvent - Multiple listeners', async () => {
  const mockFn1 = jest.fn();
  const mockFn2 = jest.fn();

  const actorInstance = await ActorInstance.initialize(
    'testActor',
    async context => {
      context.events.on('integration-timer-fired', () => mockFn1());
      context.events.on('publish-pipeline-failed', () => mockFn2());
    },
    failOnErrorLogger,
    fakeSecrets,
  );

  expect(await actorInstance.emitEvent('integration-timer-fired')).toBe(true);
  expect(mockFn1).toHaveBeenCalled();
  expect(mockFn2).not.toHaveBeenCalled();
});

const pullOpenedPayload = require('../collateral/pull-opened-hook-payload.json');

test('receiveWebhook - No listeners', async () => {
  const actorInstance = await ActorInstance.initialize(
    'testActor',
    emptyActor,
    failOnErrorLogger,
    fakeSecrets,
  );

  await actorInstance.receiveWebhook({
    id: '1234',
    name: 'pull_request',
    payload: pullOpenedPayload,
  });
});

test('receiveWebhook - Multiple listeners', async () => {
  const mockFn1 = jest.fn();
  const mockFn2 = jest.fn();

  const actorInstance = await ActorInstance.initialize(
    'testActor',
    async ({gitHooks}) => {
      gitHooks.on('pull_request.opened', () => mockFn1());
      gitHooks.on('pull_request.closed', () => mockFn2());
    },
    failOnErrorLogger,
    fakeSecrets,
  );

  await actorInstance.receiveWebhook({
    id: '1234',
    name: 'pull_request',
    payload: pullOpenedPayload,
  });

  expect(mockFn1).toHaveBeenCalled();
  expect(mockFn2).not.toHaveBeenCalled();
});
