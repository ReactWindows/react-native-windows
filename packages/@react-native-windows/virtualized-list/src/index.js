/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict-local
 */

module.exports = {
  // $FlowFixMe[unsafe-getters-setters]
  get VirtualizedList(): VirtualizedList {
    return require('./VirtualizedList');
  },

  // $FlowFixMe[unsafe-getters-setters]
  get ExperimentalVirtualizedList(): ExperimentalVirtualizedList {
    return require('./ExperimentalVirtualizedList');
  },
};
