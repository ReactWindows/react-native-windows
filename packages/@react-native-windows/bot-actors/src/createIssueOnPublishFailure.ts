/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {ActorDefinition, ActorRegistry} from './framework';

/**
 * Actor to create a GitHub issue if publish pipelines see a failure
 */
const actor: ActorDefinition = async ({events, octokit}) => {
  events.on('publish-pipeline-failed', async ({buildNumber, buildUri}) => {
    await octokit.issues.create({
      owner: 'microsoft',
      repo: 'react-native-windows',
      title: `Failed to publish "${buildNumber}"`,
      body: `See ${buildUri} for more details`,
    });
  });
};

export default ActorRegistry.register('createIssueOnPublishFailure', actor);
