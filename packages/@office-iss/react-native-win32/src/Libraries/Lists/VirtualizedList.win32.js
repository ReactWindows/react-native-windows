/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict-local
 */

import QuirkSettings from '../QuirkSettings/QuirkSettings';

module.exports = QuirkSettings.enableCellRenderMask
  ? require('@react-native-windows/virtualized-list')
      .ExperimentalVirtualizedList
  : require('@react-native-windows/virtualized-list').VirtualizedList;
