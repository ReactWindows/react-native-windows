/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const fs = require('fs');
const path = require('path');
const os = require('os');

const {
  argv,
  cleanTask,
  eslintTask,
  logger,
  jestTask,
  option,
  series,
  task,
  tscTask,
  tscWatchTask,
  parallel
} = require('just-scripts');

const depcheck = require('depcheck');

option('updateSnapshot', {alias: 'u', boolean: true})

task('clean', cleanTask(['lib-commonjs']));

task('ts', tscTask({outDir: 'lib-commonjs'}));

task('jest', jestTask());
task('jest:watch', jestTask({watch: true}));

task('build', series('ts'));

task('rebuild', series('clean', 'build'));

task('depcheck', async () => {
  const depcheckConfig = path.join(process.cwd(), 'depcheck.config.js');
  const userDepcheckOptions = fs.existsSync(depcheckConfig) ? require(depcheckConfig) : {};
  const depcheckOptions = {
    ...userDepcheckOptions,
    specials: [
      depcheck.special.eslint,
      depcheck.special.jest,
    ],
  }

  const result = await depcheck(process.cwd(), depcheckOptions);
  if (Object.keys(result.missing).length !== 0) {
    logger.error(
      `The following dependencies are used, but not declared in "package.json": ` +
      `${JSON.stringify(result.missing, null, 2)}`
    );
    
    process.exit(1);
  }
});

task('eslint', eslintTask());
task('eslint:fix', eslintTask({fix: true}));

task('lint', parallel('eslint', 'depcheck'));
task('lint:fix', series('eslint:fix'));

task('watch', tscWatchTask({outDir: 'lib-commonjs'}));

task(
  'unitTest',
  jestTask({
    config: require.resolve('@rnw-scripts/jest-unittest-config'),
    updateSnapshot: argv().updateSnapshot
  }),
);
task(
  'endToEndTest',
  jestTask({
    config: require.resolve('@rnw-scripts/jest-e2e-config'),
    runInBand: true,
    updateSnapshot: argv().updateSnapshot
  }),
);

const windowsOnly = JSON.parse(fs.readFileSync(path.join(process.cwd(), 'package.json'))).windowsOnly;
if (os.platform() !== 'win32' && windowsOnly === true) {
  task('test', () => logger.warn('Skipping tests since "package.json" has set "windowsOnly"'));
} else {
  const hasE2eTests = fs.existsSync(path.join(process.cwd(), 'src', 'e2etest'));
  const hasUnitTests = fs.existsSync(path.join(process.cwd(), 'src', 'test'));

  if (hasE2eTests && hasUnitTests) {
    task('test', series('unitTest', 'endToEndTest'));
  } else if (hasE2eTests) {
    task('test', 'endToEndTest');
  } else if (hasUnitTests) {
    task('test', 'unitTest');
  } else {
    task('test', () => logger.info('No tests found'));
  }
}
