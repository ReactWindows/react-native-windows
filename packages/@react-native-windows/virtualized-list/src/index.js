/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict-local
 */

import typeof VirtualizedList from './VirtualizedList';
import typeof ExperimentalVirtualizedList from './ExperimentalVirtualizedList';

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
