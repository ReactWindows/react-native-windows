/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * This script automatically changes files to prepare to move a release from
 * one stage to another. E.g. going from a release in our master branch to a
 * preview release in a stable branch, promoting a preview to latest, or moving
 * latest to legacy.
 *
 * See "How to promote a release" in our wiki for a detailed guide.
 *
 * @format
 */

import * as _ from 'lodash';
import * as chalk from 'chalk';
import * as child_process from 'child_process';
import * as fs from 'fs';
import * as path from 'path';
import * as simplegit from 'simple-git/promise';
import * as util from 'util';
import * as yaml from 'js-yaml';
import * as yargs from 'yargs';

import findRepoRoot from '@rnw-scripts/find-repo-root';

const glob = util.promisify(require('glob').glob);

type ReleaseType = 'preview' | 'latest' | 'legacy';

(async () => {
  const argv = collectArgs();
  const git = simplegit();
  const branchName = `${argv.rnVersion}-stable`;
  const commitMessage = `Promote ${argv.rnVersion} to ${argv.release}`;

  if (argv.release === 'preview') {
    console.log(`Creating branch ${branchName}...`);
    await git.checkoutBranch(branchName, 'HEAD');
  }

  console.log('Updating Beachball configuration...');
  await updateBeachballConfigs(argv.release as ReleaseType, argv.rnVersion);

  console.log('Updating CI variables...');
  await writeAdoVariables({
    npmTag: distTag(argv.release as ReleaseType, argv.rnVersion),
    extraPublishArgs: '',
    githubReleaseNpxArgs: '--ignore-existing',
  });

  if (argv.release === 'preview') {
    console.log('Updating root change script...');
    await updatePackage('react-native-windows-repo', {
      scripts: {change: `beachball change --branch ${branchName}`},
    });

    console.log('Updating package versions...');
    await updatePackageVersions(`${argv.rnVersion}-preview.0`);
  }

  console.log('Committing changes...');
  await git.add('--all');
  await git.commit(commitMessage);

  console.log('Generating change files...');
  if (argv.release === 'preview') {
    await createChangeFiles('prerelease', commitMessage);
  } else {
    await createChangeFiles('patch', commitMessage);
  }

  console.log(chalk.green('All done! Please check locally commited changes.'));
})();

/**
 * Parse and validate program arguments
 */
function collectArgs() {
  const argv = yargs
    .options({
      release: {
        describe: 'What release channel to promote to',
        choices: ['preview', 'latest', 'legacy'],
        demandOption: true,
      },
      rnVersion: {
        describe: 'The semver major + minor version of the release (e.g. 0.62)',
        type: 'string',
        demandOption: true,
      },
    })
    .wrap(120)
    .version(false).argv;

  if (!argv.rnVersion.match(/\d+\.\d+/)) {
    console.error(chalk.red('Unexpected format for version (expected x.y)'));
    process.exit(1);
  }

  return argv;
}

/**
 * Write release variables to be used by CI
 *
 * @param vars object describing CI variables
 */
async function writeAdoVariables(vars: {[key: string]: any}) {
  const adoPath = path.join(await findRepoRoot(), '.ado');
  const releaseVariablesPath = path.join(adoPath, 'variables', 'release.yml');

  const releaseVariablesContent =
    '# Warning: This file is autogenerated by @rnw-scripts/promote-release\n' +
    yaml.dump({variables: vars});
  await fs.promises.writeFile(releaseVariablesPath, releaseVariablesContent);
}

/**
 * Modifies beachball configurations to the right npm tag and version bump
 * restrictions
 *
 * @param release the release type
 * @param version major + minor version
 */
async function updateBeachballConfigs(release: ReleaseType, version: string) {
  for (const packageName of [
    '@office-iss/react-native-win32',
    'react-native-windows',
  ]) {
    await updateBeachballConfig(packageName, release, version);
  }
}

/**
 * Modifies beachball config to the right npm tag and version bump restrictions
 *
 * @param packageName name of the package to update
 * @param release the release type
 * @param version major + minor version
 */
async function updateBeachballConfig(
  packageName: string,
  release: ReleaseType,
  version: string,
) {
  switch (release) {
    case 'preview':
      return updatePackage(packageName, {
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'patch'],
        },
      });

    case 'latest':
      return updatePackage(packageName, {
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'prerelease'],
        },
      });

    case 'legacy':
      return updatePackage(packageName, {
        beachball: {
          defaultNpmTag: distTag(release, version),
          disallowedChangeTypes: ['major', 'minor', 'prerelease'],
        },
      });
  }
}

/**
 * Assign properties to the npm package of the given name
 *
 * @param packageName
 * @param props
 */
async function updatePackage(packageName: string, props: {[key: string]: any}) {
  const repoRoot = await findRepoRoot();
  const packages = await glob('**/package.json', {
    cwd: repoRoot,
    ignore: '**/node_modules/**',
  });

  for (const packageFile of packages) {
    const fullPath = path.join(repoRoot, packageFile);
    const packageJson = JSON.parse(
      (await fs.promises.readFile(fullPath)).toString(),
    );
    if (packageJson.name !== packageName) {
      continue;
    }

    _.merge(packageJson, props);
    await fs.promises.writeFile(
      fullPath,
      JSON.stringify(packageJson, null /*replacer*/, 2 /*space*/) + '\n',
    );
    return;
  }

  console.error(chalk.red(`Unable to find package ${packageName}`));
  process.exit(1);
}

/**
 * Get the npm distribution tag for a given version
 *
 * @param release
 * @param version
 */
function distTag(release: ReleaseType, version: string): string {
  if (release === 'legacy') {
    return `v${version}-stable`;
  } else {
    return release;
  }
}

/**
 * Change the version of main packages to the given string
 * @param packageVersion
 */
async function updatePackageVersions(packageVersion: string) {
  for (const packageName of [
    '@office-iss/react-native-win32',
    'react-native-windows',
  ]) {
    await updatePackage(packageName, {version: packageVersion});
  }
}

/**
 * Create change files to do a version bump
 *
 * @param changeType prerelease or patch
 * @param message changelog message
 */
async function createChangeFiles(
  changeType: 'prerelease' | 'patch',
  message: string,
) {
  const repoRoot = await findRepoRoot();
  child_process.execSync(
    `npx beachball change --type ${changeType} --message "${message}"`,
    {cwd: repoRoot, stdio: 'ignore'},
  );
}
