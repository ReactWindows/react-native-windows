/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

const {task, parallel} = require('just-scripts');

// Use the shared base configuration
require('@rnw-scripts/just-task');
require('@rnw-scripts/just-task/react-native-tasks');

task('lint', parallel('eslint', 'flow-check'));
