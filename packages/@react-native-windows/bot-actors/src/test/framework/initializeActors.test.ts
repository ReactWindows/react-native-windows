/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {initializeActors} from '../../framework';
import {failOnErrorLogger, fakeSecrets} from '../Mocks';

test('Initialization succeeds', async () => {
  await initializeActors(failOnErrorLogger, fakeSecrets);
});
