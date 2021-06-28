# Change Log - react-native-platform-override

This log was last generated on Fri, 11 Jun 2021 05:08:55 GMT and should not be manually modified.

<!-- Start content -->

## 1.4.17

Fri, 11 Jun 2021 05:08:55 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.2.0 (ngerlem@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.20 (ngerlem@microsoft.com)

## 1.4.16

Wed, 09 Jun 2021 05:10:07 GMT

### Patches

- Integrate May 19th (igklemen@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.19 (igklemen@microsoft.com)

## 1.4.15

Thu, 03 Jun 2021 05:09:47 GMT

### Patches

- Enable esModuleInterop Repo Wide (ngerlem@microsoft.com)
- Check for missing dependencies during lint (ngerlem@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.18 (ngerlem@microsoft.com)

## 1.4.14

Fri, 21 May 2021 05:17:18 GMT

### Patches

- Integrate Apr 28 (base of 0.65) Nightly RN Build (ngerlem@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.17 (ngerlem@microsoft.com)

## 1.4.13

Thu, 04 Mar 2021 05:05:53 GMT

### Patches

- Update yargs version used and patch y18n version 4.0.0 to 4.0.1 as used by detox of transitive dependencies to address https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2020-7774 (dannyvv@microsoft.com)

## 1.4.12

Thu, 11 Feb 2021 05:08:29 GMT

### Patches

- Bump @rnw-scripts/just-task to v2.1.0 (ngerlem@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.16 (ngerlem@microsoft.com)

## 1.4.11

Tue, 09 Feb 2021 05:07:37 GMT

### Patches

- Bump just-scripts from 1.3.2 to 1.3.3 (dannyvv@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.15 (dannyvv@microsoft.com)

## 1.4.10

Thu, 28 Jan 2021 05:06:47 GMT

### Patches

- Leverage default config support in Just library and factor out common tasks (dannyvv@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.14 (dannyvv@microsoft.com)

## 1.4.9

Wed, 27 Jan 2021 05:06:36 GMT

### Patches

- Show TS Stack Traces on Uncaught CLI Exception (ngerlem@microsoft.com)

## 1.4.8

Tue, 26 Jan 2021 05:06:39 GMT

### Patches

- Update @types package versions in package.json from dependabot updates to yarn.lock (dannyvv@microsoft.com)

### Changes

- Bump @react-native-windows/package-utils to v0.0.0-canary.13 (dannyvv@microsoft.com)

## 0.4.7

Fri, 15 Jan 2021 05:07:01 GMT

### Patches

- Update code to new packages for isUtf8 and yargs (canli@microsoft.com)

## 0.4.6

Wed, 06 Jan 2021 05:06:51 GMT

### Patches

- Move some packages from @rnw-scripts to @react-native-windows (ngerlem@microsoft.com)

## 0.4.5

Fri, 18 Dec 2020 19:23:58 GMT

### Patches

- Fix missing dependency / race condition in package.json (dannyvv@microsoft.com)

## 0.4.4

Tue, 08 Dec 2020 03:20:37 GMT

### Patches

- Use whitespace insensitive compare for indentical override detection (ngerlem@microsoft.com)
- Bump yargs to 15.4.1 (ngerlem@microsoft.com)

## 0.4.3

Tue, 24 Nov 2020 05:08:17 GMT

### Patches

- Add more eslint rules (ngerlem@microsoft.com)

## 0.4.1

Thu, 29 Oct 2020 05:05:49 GMT

### Patches

- Better Override Validation Messages When Errors Exist in Multiple Packages (ngerlem@microsoft.com)

## 0.4.0

Wed, 21 Oct 2020 05:05:24 GMT

### Minor changes

- Allow Multiple Manifests in Monorepos (ngerlem@microsoft.com)
- Drop Issue Requirement from Override Manifest Schema Validation (ngerlem@microsoft.com)

## 0.3.3

Sat, 03 Oct 2020 05:04:45 GMT

### Patches

- Allow Default baseVersion in Override Manfests (ngerlem@microsoft.com)
- Add command to diff overrides (ngerlem@microsoft.com)

## 0.3.2

Fri, 02 Oct 2020 05:05:22 GMT

### Patches

- Add some extra error handling to GitReactFileRepository (ngerlem@microsoft.com)

## 0.3.1

Thu, 01 Oct 2020 05:08:11 GMT

### Patches

- ESLint Fixup (ngerlem@microsoft.com)
- Fix applying patches when case is mismatched (ngerlem@microsoft.com)

## 0.3.0

Wed, 30 Sep 2020 05:06:49 GMT

### Minor changes

- macOS Support (nick@nickgerleman.com)

## 0.2.8

Thu, 24 Sep 2020 05:06:21 GMT

### Patches

- Ignore filemode for Scratch Repository (ngerlem@microsoft.com)

## 0.2.7

Tue, 22 Sep 2020 04:46:10 GMT

### Patches

- Fix react-native-platform-override CLI (ngerlem@microsoft.com)

## 0.2.6

Mon, 21 Sep 2020 05:06:22 GMT

### Patches

- Fixup eslint config (ngerlem@microsoft.com)

## 0.2.5

Mon, 14 Sep 2020 22:03:37 GMT

### Patches

- Reconcile package versions (ngerlem@microsoft.com)

## 0.2.4

Sat, 12 Sep 2020 05:08:52 GMT

### Patches

- Start Sharing Test Configuration Between Node Projects (ngerlem@microsoft.com)

## 0.2.3

Wed, 09 Sep 2020 05:06:35 GMT

### Patches

- Enable @typescript-eslint/no-floating-promises (ngerlem@microsoft.com)

## 0.2.1

Tue, 14 Jul 2020 05:05:25 GMT

### Patches

- Add Validation That Patch/Derived Overrides Aren't Identical to Their Originals (ngerlem@microsoft.com)

## 0.2.0

Sat, 11 Jul 2020 05:09:38 GMT

### Minor changes

- Improve Override Upgrade Performance (ngerlem@microsoft.com)
- Support Directory Copy Overrides (ngerlem@microsoft.com)

## 0.1.1

Wed, 08 Jul 2020 01:27:37 GMT

### Patches

- Normalize override manifests to unix paths (ngerlem@microsoft.com)

## 0.1.0

Mon, 06 Jul 2020 23:11:46 GMT

### Minor changes

- Add Support for Binary Overrides to react-native-platform-override (ngerlem@microsoft.com)

### Patches

- Create a Programmatic API for react-native-platform-override (ngerlem@microsoft.com)

## 0.0.8

Wed, 01 Jul 2020 05:06:19 GMT

### Patches

- Share eslint and Typescript configs across packages (ngerlem@microsoft.com)

## 0.0.7

Sat, 27 Jun 2020 03:29:52 GMT

### Patches

- Convert Repo Scripts To Published TypeScript Packages (ngerlem@microsoft.com)

## 0.0.6

Fri, 26 Jun 2020 23:59:27 GMT

### Patches

- Upgrade React Native to 3/22 Nightly Build (ngerlem@microsoft.com)

## 0.0.5

Fri, 26 Jun 2020 20:17:03 GMT

### Patches

- Ensure lib-commonjs is packaged into react-native-platform-override (ngerlem@microsoft.com)

## 0.0.4

Sat, 06 Jun 2020 00:05:20 GMT

### Patches

- Revise Architecture of react-native-platform-override (ngerlem@microsoft.com)

## 0.0.3

Tue, 02 Jun 2020 00:07:29 GMT

### Patches

- Allow Override Tooling to Use Nightly Builds (ngerlem@microsoft.com)

## 0.0.2

Tue, 26 May 2020 00:06:43 GMT

### Patches

- Rename override-tools to react-native-platform-override (ngerlem@microsoft.com)
