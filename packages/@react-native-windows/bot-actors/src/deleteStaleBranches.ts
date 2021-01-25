/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {EventPayloads, WebhookEvent} from '@octokit/webhooks';

import {ActorDefinition, ActorRegistry} from './framework';

/**
 * Actor to delete bot-created branches once an associated pull request is
 * closed or merged
 */
const actor: ActorDefinition = async ({log, gitHooks, octokit}) => {
  gitHooks.on('pull_request.closed', onPullClosedOrMerged);
  gitHooks.on('pull_request.merged', onPullClosedOrMerged);

  async function onPullClosedOrMerged(
    event: WebhookEvent<EventPayloads.WebhookPayloadPullRequest>,
  ) {
    const pull = event.payload.pull_request;
    const shouldDeleteBranch =
      pull.user.login === 'rnbot' &&
      (pull.head.user.login === 'microsoft' ||
        pull.head.user.login === 'rnbot');

    if (shouldDeleteBranch) {
      log.info(`Deleting stale branch "${pull.head.label}"`);
      await octokit.git.deleteRef({
        owner: pull.head.repo.owner.login,
        repo: pull.head.repo.name,
        ref: pull.head.ref,
      });
    } else {
      log.verbose(
        `Branch "${pull.head.label}" is not a candidate for deletion`,
      );
    }
  }
};

export default ActorRegistry.register('deleteStaleBranches', actor);
