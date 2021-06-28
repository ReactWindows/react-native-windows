/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import type {TurboModule} from '../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';
import type {Quirks} from './Quirks';

export interface Spec extends TurboModule {
  +getConstants: () => Quirks;
}

export default (TurboModuleRegistry.get<Spec>('QuirkSettings'): ?Spec);
