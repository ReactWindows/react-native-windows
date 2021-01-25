# @react-native-windows/bot-actors

The bot actors system allows creation of workflows that respond to GitHub or other external events. E.g.

```ts
import {ActorDefinition, ActorRegistry} from './framework';

const actor: ActorDefinition = async ({log, webhooks, octokit}) => {
  webhooks.on('pull_request.closed', async (event => {
    log.info('Responding to event');

    const pull = event.payload.pull_request;
    await octokit.git.deleteRef({
      owner: pull.head.repo.owner.login,
      repo: pull.head.repo.name,
      ref: pull.head.ref,
    });
  }));

export default ActorRegistry.register('closeBranchOnPullClosed', actor);
```

## Differences with other bot tools
Frameworks like [`probot`](https://probot.github.io/) aim to make it easy to develop publicly consumed GitHub
applications. While these are capable, deploying a public GitHub application with our automation limits the ability to
iterate quickly on our specific use-cases.

Fabric Bot/MSFTBot aims for a set of defined, customizable rules to meet generic repository needs. There is not a
documented model for extensibility.

Some solutions that are not yet usable in an OSS environment (e.g. MerlinBot).

Bot actors differentiates by being "code-first", allowing full extensibility. The system is owned and kept OSS in react-native-windows, allowing us to quickly iterate on bot capabilities to meet our needs.

## Creating an actor
Actors exist as a function accepting an `ActorContext`, which contains resources to subscribe to events or perform
external actions. This context parameter may be destructured, such as in the above example, for brevity. Actors added
to the root of the `src` directory will be automatically imported, so long as the function is registered to the
`ActorRegistry`.

## Listening to events
GitHub events may be subscribed to using the context `githubHooks` object. This is provided by `@octokit/webhooks` and
provides a type-safe collection of GitHub events and associated data.

Custom events may be provided by Azure functions in `@react-native-windows/bot-coordinator`. These can include timers,
work queues, or custom HTTP requests. These can be registered along with their associated payload schema in
`./src/Events.ts`. These events may be raised directly with the `/raiseEvent` endpoint with a JSON description of the
event, validated at runtime.

Conveniences exist to fire custom events from Azure Pipelines (apart from PR pipelines). E.g.

```yaml
  - template: template/raise-bot-event.yml
    parameters:
      eventName: publish-failed
      payload: {...}
```

We hope to add built-in support for existing Pipelines webhooks in the future.

## Responding to events
The `ActorContext` provides an `octokit` object, which is an instance of `@octokit/rest`. This object allows
interfacing with GitHub services, with builtin logging, rate-limit handling, and retry.

In the future, this Context object will include other services, such as running Azure pipelines, or performing
arbitrary HTTP requests.

## Testing
Actors may be tested using Jest tests in `./src/test`, along with a `MockContext`. The mock may be used to fire events
or webhooks to the actor. In the likely case your actor requires interfacing with GitHub services, `@octokit/fixtures`
may be used to simulate service responses.

TBD more here(Before PR completion)

## Deployment
The underlying Azure functions application is deployed nightly along with other artifacts published by the repo.
