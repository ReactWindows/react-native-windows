/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');
const path = require('path');
const sanitizeFilename = require('sanitize-filename');
const {LogBox} = require('react-native');

const screenshotDir = './errorShots';
fs.mkdirSync(screenshotDir, {recursive: true});

// Register to screenshot on each test failure
global.jasmine.getEnv().addReporter({
  specDone: async result => {
    if (result.status === 'failed') {
      const friendlySpecName = sanitizeFilename(
        `${result.fullName.replace(/\s/g, '-')}.png`,
      );

      const filename = path.join(screenshotDir, friendlySpecName);
      await browser.saveScreenshot(filename);
    }
  },
});

LogBox.ignoreAllLogs(true);
