/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import type {Quirks} from './Quirks';
import {DefaultQuirks} from './Quirks';
import NativeQuirkSettings from './CachingNativeQuirkSettings';

const QuirkSettings: Quirks = {
  // Enable VirtualizedList using refactored cell representation
  // $FlowFixMe[unsafe-getters-setters]
  get enableCellRenderMask(): boolean {
    return (
      NativeQuirkSettings.getConstants().enableCellRenderMask ??
      DefaultQuirks.enableCellRenderMask
    );
  },

  // Keeps VirtualizedList cells realized if last focused and
  // enableCellRenderMask is also enabled.
  // $FlowFixMe[unsafe-getters-setters]
  get enableFocusAwareRealization(): boolean {
    return (
      NativeQuirkSettings.getConstants().enableFocusAwareRealization ??
      DefaultQuirks.enableFocusAwareRealization
    );
  },
};

export default QuirkSettings;
