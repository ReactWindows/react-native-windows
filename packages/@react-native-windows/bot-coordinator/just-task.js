/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const _ = require('lodash');
const fs = require('fs');

const {task, parallel} = require('just-scripts');

require('@rnw-scripts/just-task');

// We generate local.settings.json instead of leaving the standard
// local.settings.example.json to reduce the chances of accidentally editing a
// tracked file to add credentials.
const settingsShape = {
  secrets: ['GITHUB_AUTH_TOKEN', 'HTTP_SHARED_SECRET'],
  properties: {
    AzureWebJobsStorage: 'UseDevelopmentStorage=true',
    FUNCTIONS_WORKER_RUNTIME: 'node',
    ENABLE_SIGNATURE_VERIFICATION: false,
  },
};

task('generateLocalSettings', () => {
  const defaultSettings = {
    IsEncrypted: false,
    Values: {...settingsShape.properties},
  };

  settingsShape.secrets.forEach(
    secret => (defaultSettings.Values[secret] = '<Secret Here>'),
  );

  // Keep any changes that were made to local settings
  const existingSettings = fs.existsSync('./local.settings.json')
    ? JSON.parse(fs.readFileSync('./local.settings.json'))
    : {};

  const deepMergedSettings = _.merge(defaultSettings, existingSettings);
  fs.writeFileSync(
    './local.settings.json',
    JSON.stringify(deepMergedSettings, null, 2),
  );
});

task('build', parallel('ts', 'generateLocalSettings'));
