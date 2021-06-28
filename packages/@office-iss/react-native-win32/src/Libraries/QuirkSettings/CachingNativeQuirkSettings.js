/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import NativeQuirkStorage from './NativeQuirkSettings';
import type {Spec as NativeQuirkSettingsSpec} from './NativeQuirkSettings';
import type {Quirks} from './Quirks';
import {DefaultQuirks} from './Quirks';

let cachedQuirks: ?Quirks = null;

const CachingNativeQuirkSettings: NativeQuirkSettingsSpec = {
  getConstants: () => {
    if (cachedQuirks != null) {
      return cachedQuirks;
    } else if (NativeQuirkStorage) {
      cachedQuirks = NativeQuirkStorage.getConstants();
      return cachedQuirks;
    } else {
      cachedQuirks = DefaultQuirks;
      return cachedQuirks;
    }
  },
};

export default CachingNativeQuirkSettings;
