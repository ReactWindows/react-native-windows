/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

export type Quirks = {|
  enableFocusAwareRealization: boolean,
  enableCellRenderMask: boolean,
|};

export const DefaultQuirks: Quirks = {
  enableFocusAwareRealization: true,
  enableCellRenderMask: true,
};
