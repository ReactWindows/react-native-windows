# Change Log - react-native-windows

This log was last generated on Sun, 27 Jun 2021 05:08:11 GMT and should not be manually modified.

<!-- Start content -->

## 0.0.0-canary.333

Sun, 27 Jun 2021 05:08:11 GMT

### Changes

- Removed Microsoft.ReactNative.Manager.ReactPackageProvider registration from C# template (matteo.pagani@microsoft.com)

## 0.0.0-canary.332

Sat, 26 Jun 2021 05:06:48 GMT

### Changes

- Upgrade to boost 1.76.0.0 (julio.rocha@microsoft.com)

## 0.0.0-canary.331

Fri, 25 Jun 2021 05:09:48 GMT

### Changes

- Run RN List Jest Tests (ngerlem@microsoft.com)

## 0.0.0-canary.330

Thu, 24 Jun 2021 05:08:43 GMT

### Changes

- Use older SDK for the purpose of Reunion projection (asklar@microsoft.com)
- Update V8 package version (tudor.mihai@microsoft.com)

## 0.0.0-canary.329

Wed, 23 Jun 2021 00:22:52 GMT

### Changes

- Create cswinrt projection nuget project (asklar@microsoft.com)

## 0.0.0-canary.328

Tue, 22 Jun 2021 05:07:16 GMT

### Changes

- Have script store return nullptr on facebook::jsi::JSINativeException (julio.rocha@microsoft.com)

## 0.0.0-canary.327

Fri, 18 Jun 2021 05:06:48 GMT

### Changes

- FacadeType extended with None to avoid crash for unsupported values (Bartosz.Klonowski@callstack.com)
- More cases of *Undefined* are popping up in MsBuild (dannyvv@microsoft.com)

## 0.0.0-canary.326

Thu, 17 Jun 2021 05:07:57 GMT

### Changes

- Fix crash when adding a ReactRootView while reloading a ReactHost (30809111+acoates-ms@users.noreply.github.com)
- Integrate 6/2 (34109996+chiaramooney@users.noreply.github.com)

## 0.0.0-canary.325

Wed, 16 Jun 2021 05:08:34 GMT

### Changes

- Fix deadlock when rapidly reloading instance (30809111+acoates-ms@users.noreply.github.com)
- TurboModules might be kept alive by RNH when instance is shutdown (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.324

Tue, 15 Jun 2021 05:06:39 GMT

### Changes

- Remove JavaScriptMainModuleName and DebugHost (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.323

Sat, 12 Jun 2021 05:06:29 GMT

### Changes

- Better reporting of failures to load the bundle file (30809111+acoates-ms@users.noreply.github.com)
- Reduce string conversions when reporting hresult_error (julio.rocha@microsoft.com)

## 0.0.0-canary.322

Fri, 11 Jun 2021 05:08:55 GMT

### Patches

- Bump react-native-platform-override to v1.4.17 (ngerlem@microsoft.com)

### Changes

- fix pipeline casing of x64/x86 as yaml string compare is case sensitive (dannyvv@microsoft.com)
- Disable test that seems to hang on PR validaton (dannyvv@microsoft.com)

## 0.0.0-canary.321

Thu, 10 Jun 2021 05:10:10 GMT

### Changes

- Fix Microsoft.ReactNative unit tests (vmorozov@microsoft.com)

## 0.0.0-canary.320

Wed, 09 Jun 2021 05:10:07 GMT

### Changes

- Integrate May 19th (igklemen@microsoft.com)

## 0.0.0-canary.319

Sat, 05 Jun 2021 05:09:53 GMT

### Changes

- Condition using memory mapped buffer to runtime option JSI.MemoryMappedScriptStore (julio.rocha@microsoft.com)
- Mark QuirkSettings as Experimental (30809111+acoates-ms@users.noreply.github.com)
- Emit `onSubmitEditing` before clearing text (erozell@outlook.com)
- Update Button Hover (34109996+chiaramooney@users.noreply.github.com)
- Integrate May 12th RN nightly build. (igklemen@microsoft.com)

## 0.0.0-canary.318

Thu, 03 Jun 2021 05:09:47 GMT

### Patches

- Bump react-native-platform-override to v1.4.15 (ngerlem@microsoft.com)
- Bump @rnw-scripts/eslint-config to v1.1.7 (ngerlem@microsoft.com)

### Changes

- Upgrade Windows SDK to 10.0.19041.0 (julio.rocha@microsoft.com)
- Update V8 package version (tudor.mihai@microsoft.com)
- Allow propagating WinRT WS resource constructor exceptions (julio.rocha@microsoft.com)
- Do not mark instance as IsProfiling when using fastrefresh (30809111+acoates-ms@users.noreply.github.com)
- Adds event coalescing to TouchEventHandler (erozell@outlook.com)
- Enable esModuleInterop Repo Wide (ngerlem@microsoft.com)
- I18n.AllowRTL should default to true (30809111+acoates-ms@users.noreply.github.com)
- Add uwp version 2.2.12 (agnel@microsoft.com)
- Add error message when an exception happens in a native module (in Debug) (asklar@microsoft.com)
- Make private constructor certExceptions r-value (julio.rocha@microsoft.com)
- Check for missing dependencies during lint (ngerlem@microsoft.com)
- Hermes inspector was broken on Hermes v0.8 package as some changes were not correctly migrated from 0.7 branch. (email not defined)
- Integrate RN nightly May 5. (igklemen@microsoft.com)
- Native build logic shouldn't delete PCH's on all CI machines (ngerlem@microsoft.com)
- Make Fabric a per RootView setting (30809111+acoates-ms@users.noreply.github.com)
- Fix JSValue memory leak in move constructor (vmorozov@microsoft.com)

## 0.0.0-canary.317

Tue, 25 May 2021 19:36:43 GMT

### Changes

- Prepare files for default branch rename (dannyvv@microsoft.com)
- add forked ToAscii.h unblocking CI (agnel@microsoft.com)

## 0.0.0-canary.316

Fri, 21 May 2021 21:40:00 GMT

### Changes

- [Fabric] Add support for PlatformColor (30809111+acoates-ms@users.noreply.github.com)
- Fixes bug in BatchingEventEmitter (erozell@outlook.com)
- folly update (agnel@microsoft.com)

## 0.0.0-canary.315

Fri, 21 May 2021 05:17:18 GMT

### Patches

- Bump react-native-platform-override to v1.4.14 (ngerlem@microsoft.com)

### Changes

- Integrate Apr 28 (base of 0.65) Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.314

Wed, 19 May 2021 05:06:54 GMT

### Changes

- Fix transitive dependencies for C# nuget-based apps (jthysell@microsoft.com)

## 0.0.0-canary.313

Tue, 18 May 2021 05:05:13 GMT

### Changes

- Fabric: Add support view View.opacity and View.borderRadius (30809111+acoates-ms@users.noreply.github.com)
- Fabric: Add support for Image events, blurRadius and tintColor (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.312

Mon, 17 May 2021 05:11:32 GMT

### Changes

- Exit early during text transform upward tree traversal. (igklemen@microsoft.com)

## 0.0.0-canary.311

Sat, 15 May 2021 05:06:21 GMT

### Changes

- Enabling Hermes Inspector (anandrag@microsoft.com)

## 0.0.0-canary.310

Fri, 14 May 2021 05:06:39 GMT

### Changes

- Use memory mapping for ScriptStore::tryGetPreparedScript (julio.rocha@microsoft.com)

## 0.0.0-canary.309

Thu, 13 May 2021 05:11:48 GMT

### Changes

- Hook up view component codegen (30809111+acoates-ms@users.noreply.github.com)
- Emit TextInput onChangeText before onSelectionChange (erozell@outlook.com)

## 0.0.0-canary.308

Tue, 11 May 2021 05:06:47 GMT

### Changes

- Enable C++20 builds (asklar@microsoft.com)
- Generate cgmanifest.json for downloaded source dependencies (jthysell@microsoft.com)
- Combine ReactRootView and ReactRootControl (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.307

Sun, 09 May 2021 05:06:22 GMT

### Changes

- Fix warnings and improve C++ includes (vmorozov@microsoft.com)
- Move fmt project into 'Dependencies (External)' folder (vmorozov@microsoft.com)

## 0.0.0-canary.306

Sat, 08 May 2021 05:05:56 GMT

### Changes

- Update Button Styling for Windows (34109996+chiaramooney@users.noreply.github.com)
- Remove legacy UWP WebSocket module (julio.rocha@microsoft.com)

## 0.0.0-canary.305

Fri, 07 May 2021 05:07:30 GMT

### Changes

- Fix ReactContext::DispatchEvent (vmorozov@microsoft.com)

## 0.0.0-canary.304

Wed, 05 May 2021 16:27:31 GMT

### Changes

- Rename remaining react::uwp namespaces to Microsoft::ReactNative (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.303

Tue, 04 May 2021 05:10:45 GMT

### Changes

- Fix Text foreground overwritten by backgroundColor. (igklemen@microsoft.com)
- If initial value is set, updated TextInput.value changes will not be reflected (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.302

Sat, 01 May 2021 05:05:57 GMT

### Changes

- Forked folly for SDL compliance (agnel@microsoft.com)
- Update brush cache when theme changes (asklar@microsoft.com)

## 0.0.0-canary.301

Fri, 30 Apr 2021 05:06:16 GMT

### Changes

- Allow `lineHeight` settings less than font height (erozell@outlook.com)
- Remove incorrect mapping from virtual key 192 (erozell@outlook.com)

## 0.0.0-canary.300

Thu, 29 Apr 2021 05:07:20 GMT

### Changes

- Adds 'autoFocus' prop to TextInput (erozell@outlook.com)
- Integrate April 22 RN Nightly Build (ngerlem@microsoft.com)
- Fix MSBuild Bundling When Path Contains Spaces (ngerlem@microsoft.com)
- Remove Forking Due to "acceptsKeyboardFocus" Invariant Checks (ngerlem@microsoft.com)
- Remove ARM32 Support (ngerlem@microsoft.com)
- Update to latest @types/react-native (ngerlem@microsoft.com)
- Remove version from ExperimentalFeatures.props links (ngerlem@microsoft.com)

## 0.0.0-canary.299

Wed, 28 Apr 2021 05:07:02 GMT

### Changes

- Changes required to run in a Reunion app (asklar@microsoft.com)
- Very early fabric implementation (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.298

Tue, 27 Apr 2021 05:09:48 GMT

### Changes

- Update Microsoft.ChakraCore to 1.11.24 (jthysell@microsoft.com)
- Fix CI after change to Format library (dannyvv@microsoft.com)

## 0.0.0-canary.297

Sat, 24 Apr 2021 05:06:52 GMT

### Changes

- Replace fmt stub with real implementation (30809111+acoates-ms@users.noreply.github.com)
- Adds declarative props to clear/submit TextInput (erozell@outlook.com)
- Integration Apr 1 - Apr 15 (34109996+chiaramooney@users.noreply.github.com)
- Remove unused ChakraNativeModules files (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.296

Fri, 23 Apr 2021 05:08:07 GMT

### Changes

- restore some SDL warnings (agnel@microsoft.com)

## 0.0.0-canary.295

Wed, 21 Apr 2021 05:06:31 GMT

### Changes

- restore some SDL warnings (agnel@microsoft.com)

## 0.0.0-canary.294

Tue, 20 Apr 2021 05:06:21 GMT

### Changes

- fix filter project merge (asklar@microsoft.com)

## 0.0.0-canary.293

Sat, 17 Apr 2021 05:08:13 GMT

### Changes

- Layout wrongly overwrites width/height coming from view manager (asklar@microsoft.com)

## 0.0.0-canary.292

Fri, 16 Apr 2021 05:08:38 GMT

### Changes

- Integrate 4/1 nightly RN build. (igklemen@microsoft.com)
- Calling UIManager.dispatchViewManagerCommand should trigger updates (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.291

Thu, 15 Apr 2021 05:09:36 GMT

### Changes

- Fix XAML Visibility for `display:none` (agnel@microsoft.com)
- re-enabling 4267 & 4100 (agnel@microsoft.com)

## 0.0.0-canary.290

Wed, 14 Apr 2021 05:08:14 GMT

### Changes

- Enable any platform color to be used in PlatformColor() (asklar@microsoft.com)

## 0.0.0-canary.289

Tue, 13 Apr 2021 05:07:37 GMT

### Changes

- Fixes bug where raw text update overwrites all text (erozell@outlook.com)
- Add functionality to setAccessibilityFocus method (34109996+chiaramooney@users.noreply.github.com)

## 0.0.0-canary.288

Sat, 10 Apr 2021 05:06:27 GMT

### Changes

- Allow swapping to System Chakra at runtime. (julio.rocha@microsoft.com)
- Fixes textTransform recursion (erozell@outlook.com)
- Integrate 3/18 nightly RN build. (igklemen@microsoft.com)

## 0.0.0-canary.287

Thu, 08 Apr 2021 05:07:22 GMT

### Changes

- Fix onKeyDown/onKeyUp events not firing after reload (stecrain@microsoft.com)

## 0.0.0-canary.286

Tue, 06 Apr 2021 05:05:03 GMT

### Changes

- Revert "Fix touch event crash (#6692)" (igklemen@microsoft.com)

## 0.0.0-canary.285

Fri, 02 Apr 2021 05:07:19 GMT

### Changes

- Add onKeyDown/onKeyUp to Pressable. (igklemen@microsoft.com)

## 0.0.0-canary.284

Thu, 01 Apr 2021 05:07:27 GMT

### Changes

- Update to new Folly version (asklar@microsoft.com)

## 0.0.0-canary.283

Sun, 28 Mar 2021 05:06:13 GMT

### Changes

- Implement JSI getOwnPropertyDescriptor (asklar@microsoft.com)

## 0.0.0-canary.282

Fri, 26 Mar 2021 21:27:59 GMT

### Changes

- Fix some doc typos, generate docs in CI (asklar@microsoft.com)

## 0.0.0-canary.281

Fri, 26 Mar 2021 05:06:37 GMT

### Changes

- Enable libraries to use NuGet too (avoid mdmerge MDM2009 error) (asklar@microsoft.com)
- Suppress onLayout event when layout has not changed (erozell@outlook.com)

## 0.0.0-canary.280

Thu, 25 Mar 2021 05:06:57 GMT

### Changes

- Fix app template to use app.json's name for main component name (jthysell@microsoft.com)

## 0.0.0-canary.279

Tue, 23 Mar 2021 05:06:08 GMT

### Changes

- Fix c++20 issues (asklar@microsoft.com)

## 0.0.0-canary.278

Fri, 19 Mar 2021 05:12:09 GMT

### Changes

- Add retries to Folly download since it sometimes fails in CI (asklar@microsoft.com)
- Make BaseWebSocketResource destructor noexcept (julio.rocha@microsoft.com)
- Enable overriding WinUI and SDK versions via ExperimentalFeatures.props (asklar@microsoft.com)

## 0.0.0-canary.277

Thu, 18 Mar 2021 05:08:24 GMT

### Changes

- Add doc comments to IDL files (vmorozov@microsoft.com)

## 0.0.0-canary.276

Wed, 17 Mar 2021 05:18:22 GMT

### Changes

- Fix redbox on boot when using web debugging (30809111+acoates-ms@users.noreply.github.com)
- Minor docs update for ReactDispatcherHelper (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.275

Tue, 16 Mar 2021 05:07:08 GMT

### Changes

- AzDO image now installs uwp package 6.2.11, not 6.2.10; Update Codegen UnitTest (asklar@microsoft.com)

## 0.0.0-canary.274

Sun, 14 Mar 2021 05:06:38 GMT

### Changes

- update to newer cppwinrt, 2-3% reduction in binary size (asklar@microsoft.com)

## 0.0.0-canary.273

Sat, 13 Mar 2021 05:08:04 GMT

### Changes

- Add method support for Xaml Islands (34109996+chiaramooney@users.noreply.github.com)
- Change TextInput cursor behavior (erozell@outlook.com)

## 0.0.0-canary.272

Fri, 12 Mar 2021 05:07:46 GMT

### Changes

- Include Hermes NuGet Package By Default (ngerlem@microsoft.com)
- Raise Instance notifications in JSDispatcher thread (vmorozov@microsoft.com)

## 0.0.0-canary.271

Thu, 11 Mar 2021 05:07:32 GMT

### Changes

- Enable view managers to listen for JS event attach/detach (asklar@microsoft.com)
- Fixes case where focus commands may be ignored (erozell@outlook.com)
- Enabling codesigning via ESRP (jthysell@microsoft.com)

## 0.0.0-canary.270

Wed, 10 Mar 2021 05:10:47 GMT

### Changes

- More doc formatting (asklar@microsoft.com)

## 0.0.0-canary.269

Tue, 09 Mar 2021 22:34:34 GMT

### Changes

- beachball fixes (jthysell@microsoft.com)

## 0.0.0-canary.268

Tue, 09 Mar 2021 21:16:42 GMT

### Changes

- beachball sync (jthysell@microsoft.com)
- Fix rnw-dependencies.ps1: VS Enterprise, require 18362 sdk, add cppwinrt vsix for rnw devs (asklar@microsoft.com)

## 0.0.0-canary.265

Tue, 09 Mar 2021 18:00:37 GMT

### Changes

- Integrate 6/3 RN nigtly build. (igklemen@microsoft.com)
- Clarify IJSValueWriter/IJSValueReader docs (asklar@microsoft.com)
- fix bad merge (asklar@microsoft.com)
- Disable warning 4103 in MemoryMappedBufferTests.cpp (julio.rocha@microsoft.com)
- Clarify IJSValueWriter/IJSValueReader docs (asklar@microsoft.com)
- Integrate 2/20 (igklemen@microsoft.com)

## 0.0.0-canary.264

Mon, 08 Mar 2021 20:49:18 GMT

### Changes

- Avoid re-registering render callbacks when coalescing single element queue (ngerlem@microsoft.com)

## 0.0.0-canary.263

Fri, 05 Mar 2021 16:31:03 GMT

### Changes

- remove stale appium comment (igklemen@microsoft.com)
- Fix right click warning. (igklemen@microsoft.com)
- Remove unnecessary exceptions to unblock some partners (asklar@microsoft.com)
- Coalesce ScrollView Messages (ngerlem@microsoft.com)
- Remove ChakraExecutor (non-JSI) (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.262

Thu, 04 Mar 2021 05:05:53 GMT

### Patches

- Bump react-native-platform-override to v1.4.13 (dannyvv@microsoft.com)

### Changes

- Some XAML events do not have a sender (e.g. Loaded) (asklar@microsoft.com)
- Update WinAppDriver version requirement (asklar@microsoft.com)
- Fixes registration name for TextInput onScroll (erozell@outlook.com)
- Integrate 2/9 nightly RN build. (igklemen@microsoft.com)
- Ensure root views are properly unloaded (#7221) (skyle@fb.com)
- Narrow fix for tintColor and repeat images (erozell@outlook.com)
- Fix RN runtimeExecutor use of std::thread (asklar@microsoft.com)

## 0.0.0-canary.261

Tue, 02 Mar 2021 05:06:32 GMT

### Changes

- Fix announceForAccessibility (34109996+chiaramooney@users.noreply.github.com)

## 0.0.0-canary.260

Fri, 26 Feb 2021 05:06:35 GMT

### Changes

- Revert "Fix DispatchEvent for events that don't have a sender (#7207)" (asklar@microsoft.com)

## 0.0.0-canary.259

Thu, 25 Feb 2021 05:05:52 GMT

### Changes

- Some XAML events do not have a sender (e.g. Loaded) (asklar@microsoft.com)
- [Image] add svg data uri support (email not defined)

## 0.0.0-canary.258

Tue, 23 Feb 2021 05:06:42 GMT

### Changes

- Fixes race conditions for ReactImage (erozell@outlook.com)
- Fix websocket crash when bundler goes away (asklar@microsoft.com)

## 0.0.0-canary.257

Sat, 20 Feb 2021 05:06:12 GMT

### Changes

- Include RuntimeExecutor.h in nuget (30809111+acoates-ms@users.noreply.github.com)
- [Perf] Make DesktopWindowMessage a struct (asklar@microsoft.com)
- Drop WebSocketModule integration tests (julio.rocha@microsoft.com)

## 0.0.0-canary.256

Thu, 18 Feb 2021 05:07:11 GMT

### Changes

- Improve Microsoft.ReactNative compilation speed (vmorozov@microsoft.com)
- Fix DevMenuControl.xbf copy conflict (vmorozov@microsoft.com)
- Set up mechanism to forward window messages from [WinUI 3] xaml islands. Update AppTheme and DeviceInfo module. (asklar@microsoft.com)
- Integrate 1/31 Nightly RN Build (ngerlem@microsoft.com)
- Add IViewManagerCreateWithProperties to allow VMs to change behavior at create time (asklar@microsoft.com)
- Integrate rnw-dependencies into react-native doctor (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.255

Sun, 14 Feb 2021 05:06:10 GMT

### Changes

- Update WinUI 3 package to Preview 4 (asklar@microsoft.com)

## 0.0.0-canary.254

Fri, 12 Feb 2021 05:19:35 GMT

### Changes

- Fixed typo in template which breaks C# autolinking (jthysell@microsoft.com)
- Fix all package typos (jthysell@microsoft.com)
- Fix cursor resetting in TextInput. (igklemen@microsoft.com)

## 0.0.0-canary.253

Thu, 11 Feb 2021 05:08:29 GMT

### Patches

- Bump react-native-platform-override to v1.4.12 (ngerlem@microsoft.com)

### Changes

- Minor doc update (30809111+acoates-ms@users.noreply.github.com)
- Make modules use the app's buildFlags.props (asklar@microsoft.com)
- Improve compile time by increasing paralelism (dannyvv@microsoft.com)
- Rename "BuildFlags.props" to "ExperimentalFeatures.props" (ngerlem@microsoft.com)

## 0.0.0-canary.252

Wed, 10 Feb 2021 05:06:37 GMT

### Changes

- Build apps for either/or WinUI 3 or XAML (asklar@microsoft.com)

## 0.0.0-canary.251

Tue, 09 Feb 2021 05:07:37 GMT

### Changes

- [Fix] ScrollView.ScrollToEnd not working for vertical lists (igklemen@microsoft.com)
- Bump just-scripts from 1.3.2 to 1.3.3 (dannyvv@microsoft.com)
- Doxysaurus tool to generate docs for  C++ (vmorozov@microsoft.com)

## 0.0.0-canary.250

Mon, 08 Feb 2021 05:05:41 GMT

### Changes

- Fix Switch.onChange (igklemen@microsoft.com)

## 0.0.0-canary.249

Thu, 04 Feb 2021 05:05:58 GMT

### Changes

- Initialize ETW provider only once (asklar@microsoft.com)

## 0.0.0-canary.248

Wed, 03 Feb 2021 05:05:12 GMT

### Changes

- Fix template for WinUI3 C# App by expanding namespace of argument in AppOnActivated (dannyvv@microsoft.com)
- AllAnimations only exists on RS5 (asklar@microsoft.com)
- Fix deep linking activation (vmorozov@microsoft.com)

## 0.0.0-canary.247

Tue, 02 Feb 2021 05:05:37 GMT

### Changes

- Allow custom view managers to be able to trigger our logic to run native layout (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.246

Sat, 30 Jan 2021 05:06:38 GMT

### Changes

- App crashes at startup on RS3 because of RS4+ API usage (asklar@microsoft.com)

## 0.0.0-canary.245

Fri, 29 Jan 2021 05:05:32 GMT

### Changes

- Remove disabling 4018 4055 4146 4242 (asklar@microsoft.com)

## 0.0.0-canary.244

Thu, 28 Jan 2021 05:06:47 GMT

### Patches

- Bump react-native-platform-override to v1.4.10 (dannyvv@microsoft.com)
- Bump react-native-windows-codegen to v1.1.14 (dannyvv@microsoft.com)

### Changes

- Leverage default config support in Just library and factor out common tasks (dannyvv@microsoft.com)

## 0.0.0-canary.243

Wed, 27 Jan 2021 05:06:36 GMT

### Patches

- Bump react-native-windows-codegen to v1.1.13 (ngerlem@microsoft.com)
- Bump react-native-platform-override to v1.4.9 (ngerlem@microsoft.com)

### Changes

- Update comments for IJSValueReader and IJSValueWriter (vmorozov@microsoft.com)
- use lifted DispatcherQueue (asklar@microsoft.com)
- Show TS Stack Traces on Uncaught CLI Exception (ngerlem@microsoft.com)
- Remove usage of deprecated API (asklar@microsoft.com)

## 0.0.0-canary.242

Tue, 26 Jan 2021 05:06:39 GMT

### Patches

- Bump react-native-platform-override to v1.4.8 (dannyvv@microsoft.com)

### Changes

- Update @types package versions in package.json from dependabot updates to yarn.lock (dannyvv@microsoft.com)
- Background/Foreground app state handlers should be registered from the xaml thread (asklar@microsoft.com)
- Rewrite E2ETest Specs (Removes Python Dependency) (ngerlem@microsoft.com)
- Integrate 1/21 Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.241

Sat, 23 Jan 2021 05:08:20 GMT

### Changes

- react-native-windows package should include jest preset/setup files (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.240

Fri, 22 Jan 2021 05:07:27 GMT

### Changes

- Add new .net native and UWP runtime versions (asklar@microsoft.com)

## 0.0.0-canary.239

Thu, 21 Jan 2021 18:54:54 GMT

### Changes

- Update V8 and Hermes package versions (tudor.mihai@microsoft.com)

## 0.0.0-canary.238

Mon, 18 Jan 2021 05:08:29 GMT

### Changes

- Last year, we had plumbed the Systrace sections and events in the core react native to windows code and fired ETW events. But, it was hard for people to use because we used manifest based ETW events. This change converts the event triggering code to use Tracelogging APIs (aka self describing events) which doesn't require the manifest to be registered on the host machine to inspect traces.
 Post this change, the systrace events can be inspected on any windows machines where the RNW based apps are running by issuing the following commands, 
wpr.exe -start rnw.wprp
wpr.exe -stop rnw.etl
wpa.exe rnw.etl

This works for Win32 apps too, say Office. (mganandraj@outlook.com)

## 0.0.0-canary.237

Sun, 17 Jan 2021 05:08:07 GMT

### Changes

- Install .net core 3.1 for CodeGen project (asklar@microsoft.com)

## 0.0.0-canary.236

Sat, 16 Jan 2021 05:05:46 GMT

### Changes

- Fix missing commit from python dep (asklar@winse.microsoft.com)
- Fix ReactNotificationService for notifications between app and modules (vmorozov@microsoft.com)
- Treat WS protocols as scalar/array (julio.rocha@microsoft.com)
- Reduce number of windesktop forked files. (julio.rocha@microsoft.com)
- Add Python dependency to rnw-dependencies.ps1 (asklar@microsoft.com)

## 0.0.0-canary.235

Fri, 15 Jan 2021 05:07:01 GMT

### Patches

- Bump react-native-platform-override to v0.4.7 (canli@microsoft.com)

### Changes

- Add JSValue constructor for std::optional<T> (vmorozov@microsoft.com)
- Simplify Microsoft.ReactNative.IntegrationTests (vmorozov@microsoft.com)
- Updated doc comments in IReactNotificationService.idl (vmorozov@microsoft.com)
- Update clang-format version to 1.5.0 (vmorozov@microsoft.com)

## 0.0.0-canary.234

Thu, 14 Jan 2021 05:05:22 GMT

### Changes

- Move CallInvoker.h to TurboModule folder (vmorozov@microsoft.com)
- Fix unused variable warning in AbiCallInvoker (vmorozov@microsoft.com)
- Integrate RN nightly 13/1 build. (igklemen@microsoft.com)

## 0.0.0-canary.233

Wed, 13 Jan 2021 05:05:37 GMT

### Changes

- #6715 Add Workaround to Resolve textDecorationLine Bug (34109996+chiaramooney@users.noreply.github.com)

## 0.0.0-canary.232

Tue, 12 Jan 2021 05:07:53 GMT

### Changes

- #6217 : Add Clearer Error Msg for YGValues (34109996+chiaramooney@users.noreply.github.com)
- Integrate 1/6 Nightly RN Build (ngerlem@microsoft.com)
- Integrate 1/7 Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.230

Tue, 05 Jan 2021 05:06:42 GMT

### Changes

- Enable support for C++ TurboModules (vmorozov@microsoft.com)

## 0.0.0-canary.229

Tue, 29 Dec 2020 05:06:55 GMT

### Changes

- Fix newline issues in new cli project generated files (jthysell@microsoft.com)

## 0.0.0-canary.228

Thu, 24 Dec 2020 05:06:00 GMT

### Changes

- Consolidate JSI projects (julio.rocha@microsoft.com)

## 0.0.0-canary.227

Wed, 23 Dec 2020 05:07:22 GMT

### Changes

- Disable JSUpToDateCheck in design builds, ensure projects like e2etest don't spit out the sentinel files to the root of the drive (asklar@winse.microsoft.com)
- Check whether dependencies have been installed (asklar@winse.microsoft.com)
- Update Nuget to 2.5.0 (34109996+chiaramooney@users.noreply.github.com)

## 0.0.0-canary.226

Sat, 19 Dec 2020 05:06:39 GMT

### Changes

- Make package config more strict (dannyvv@microsoft.com)

## 0.0.0-canary.225

Fri, 18 Dec 2020 19:23:58 GMT

### Changes

- Specify full path when bundling (asklar@winse.microsoft.com)
- Fix tabIndex property for Button (igklemen@microsoft.com)
- Show HRESULT code as part of error message (julio.rocha@microsoft.com)
- In Xaml, IsTabStop makes a control be able to receive focus or not so we also need to set it when IsFocusable is set. (lamdoan@microsoft.com)
- Fix RedBox on topMouseLeave event. (igklemen@microsoft.com)
- Add onPressIn / onPressOut events to TextInput (igklemen@microsoft.com)
- Fix facebook::jsi::Runtime lifetime for ABI JSI (vmorozov@microsoft.com)

## 0.0.0-canary.224

Wed, 09 Dec 2020 05:07:19 GMT

### Changes

- Fix getting host objects and functions in JSI ABI (vmorozov@microsoft.com)

## 0.0.0-canary.223

Tue, 08 Dec 2020 03:20:37 GMT

### Changes

- Split out Universal build in Build and Test and have the Build run on the windev build agents (dannyvv@microsoft.com)
- TagProperty only exists on FrameworkElement and hyperlinks aren't FE (asklar@microsoft.com)
- Integrate 12/2 Nightly Build (ngerlem@microsoft.com)
- Fix multicore build, missing dependency on Folly (dannyvv@microsoft.com)
- Upgrade Min WinUI Version to 2.5 (34109996+chiaramooney@users.noreply.github.com)
- Fix destruction of UIManager in UI thread (vmorozov@microsoft.com)

## 0.0.0-canary.221

Thu, 03 Dec 2020 21:43:48 GMT

### Changes

- Fix version (ngerlem@microsoft.com)
- Update V8 package version; pass debuggerBreakOnNextLine property (tudor.mihai@microsoft.com)

## 0.0.0

Thu, 03 Dec 2020 05:06:17 GMT

### Changes

- Fix getting JsiRuntime for host functions (vmorozov@microsoft.com)

## 0.0.0-canary.220

Tue, 01 Dec 2020 05:05:07 GMT

### Changes

- document react_struct (igklemen@microsoft.com)

## 0.0.0-canary.219

Thu, 26 Nov 2020 05:06:06 GMT

### Changes

- Refactor ReactContext API (vmorozov@microsoft.com)

## 0.0.0-canary.218

Wed, 25 Nov 2020 05:06:16 GMT

### Changes

- Add error for users who try to build apps/libs via project instead of solution (jthysell@microsoft.com)

## 0.0.0-canary.217

Tue, 24 Nov 2020 05:08:17 GMT

### Changes

- Fix links in some docs, mark APIs as deprecated or experimental as appropriate (asklar@microsoft.com)
- Don't run bundling when doing intellisense builds (asklar@microsoft.com)
- Add more eslint rules (ngerlem@microsoft.com)

## 0.0.0-canary.216

Sun, 22 Nov 2020 05:06:06 GMT

### Changes

- fix invalidation bug and use Canvas.Top/Left (kmelmon@microsoft.com)
- fix panels with transforms leaking (stecrain@microsoft.com)

## 0.0.0-canary.215

Sat, 21 Nov 2020 05:05:40 GMT

### Changes

- Add ReactContext::ExecuteJsi method to use JSI in NativeModules (vmorozov@microsoft.com)

## 0.0.0-canary.214

Fri, 20 Nov 2020 05:08:43 GMT

### Changes

- WinUI 3 Preview 3 support (asklar@microsoft.com)
- Update lib template to default prefer RNW dir of consuming app (jthysell@microsoft.com)
- Integrate 11/18 RN Nightly Build 🎉 (ngerlem@microsoft.com)

## 0.0.0-canary.213

Thu, 19 Nov 2020 17:13:10 GMT

### Changes

- Update Nuget publish to not overwrite the created anycpu version (dannyvv@microsoft.com)
- Use lessthrow_await_adapter for WebSocketJSExecutor::ConnectAsync (julio.rocha@microsoft.com)

## 0.0.0-canary.212

Thu, 19 Nov 2020 05:06:10 GMT

### Changes

- Exclude Beast WebSocket from build by default. (julio.rocha@microsoft.com)
- fix typo (igklemen@microsoft.com)
- Add Yellowbox warnings for WIndows Variants of Lean Core Components (ngerlem@microsoft.com)

## 0.0.0-canary.211

Wed, 18 Nov 2020 03:10:51 GMT

### Changes

- CornerRadius only exists on RS5, restore the behavior to gracefully degrade on older OS releases (asklar@microsoft.com)
- Check for VS >= 16.5 (ngerlem@microsoft.com)

## 0.0.0-canary.209

Tue, 17 Nov 2020 05:05:00 GMT

### Changes

- Embed documentation of WinRT APIs in the winmd (asklar@microsoft.com)
- JSDispatcher should use JSCallInvoker internally (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.208

Mon, 16 Nov 2020 17:37:03 GMT

### Changes

- Fix publish (dannyvv@microsoft.com)

## 0.0.0-canary.207

Mon, 16 Nov 2020 14:39:19 GMT

### Changes

- Fix Publish pipeline due to moved file (dannyvv@microsoft.com)

## 0.0.0-canary.206

Mon, 16 Nov 2020 05:06:18 GMT

### Changes

- Update Managed ref assemblies to be AnpCpu (dannyvv@microsoft.com)

## 0.0.0-canary.205

Sat, 14 Nov 2020 05:04:43 GMT

### Changes

- Add a custom WinRT attribute to be used to store documentation strings for WinRT types (asklar@microsoft.com)
- Add win32 exports required for internal usage (email not defined)
- Fix appstate module (30809111+acoates-ms@users.noreply.github.com)
- Add Targets to check that developers have run yarn install before building (dannyvv@microsoft.com)

## 0.0.0-canary.203

Thu, 12 Nov 2020 05:05:57 GMT

### Changes

- Add Keyboard Events to TextWin32 (krsiler@microsoft.com)

## 0.0.0-canary.202

Wed, 11 Nov 2020 05:05:29 GMT

### Changes

- Remove Build-Time Native Code Patching (ngerlem@microsoft.com)

## 0.0.0-canary.201

Tue, 10 Nov 2020 05:05:13 GMT

### Changes

- Exclude *projectimports.zip from devmode metro config (asklar@microsoft.com)
- add force_layout to TextInput (igklemen@microsoft.com)

## 0.0.0-canary.200

Sat, 07 Nov 2020 05:04:58 GMT

### Changes

- Fix Bundling With "react-native-windows-init --useDevMode" (ngerlem@microsoft.com)
- Couple of minor cleanups to prep for fabric PR (30809111+acoates-ms@users.noreply.github.com)
- Fix PR validation by removing legacy generated header file (dannyvv@microsoft.com)
- Clear Unused Native Module Specs (ngerlem@microsoft.com)

## 0.0.0-canary.196

Wed, 04 Nov 2020 05:10:08 GMT

### Changes

- Autolinking fixes for C++ / C# interop (jthysell@microsoft.com)

## 0.0.0-canary.195

Tue, 03 Nov 2020 16:52:55 GMT

### Changes

- Update Microsoft.ReactNative NuGet Package to only contain release bits (dannyvv@microsoft.com)
- Add ReactNativeHost::FromContext method (vmorozov@microsoft.com)

## 0.0.0-canary.194

Sun, 01 Nov 2020 05:05:46 GMT

### Changes

- Update V8 and Hermes package versions (tudor.mihai@microsoft.com)

## 0.0.0-canary.193

Sat, 31 Oct 2020 05:06:50 GMT

### Changes

- Initialize UIManager turbomodule eagerly (30809111+acoates-ms@users.noreply.github.com)
- Integrate 10/5 Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.192

Fri, 30 Oct 2020 05:07:51 GMT

### Changes

- Enable Hermes Bytecode Compilation and SourceMap Creation as Part of Build (ngerlem@microsoft.com)
- Update Template Metro Config to Enable Inline Requires By Default (ngerlem@microsoft.com)
- Integrate 9/17 Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.191

Thu, 29 Oct 2020 05:05:49 GMT

### Changes

- Better error handling of Views inside Text components (30809111+acoates-ms@users.noreply.github.com)
- Allow Hermes to be used in C++ projects without modifying source code (ngerlem@microsoft.com)
- Integrate Additional RNTester Refactoring (9/2 Nightly RN Build) (ngerlem@microsoft.com)

## 0.0.0-canary.190

Wed, 28 Oct 2020 05:04:32 GMT

### Changes

- Implement backgroundColor for Text. (igklemen@microsoft.com)
- Move UIManager to JSI (30809111+acoates-ms@users.noreply.github.com)
- Followup from #6289 (30809111+acoates-ms@users.noreply.github.com)
- [Win32] Export createUIManagerModule that takes a uiQueue (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.189

Tue, 27 Oct 2020 05:04:48 GMT

### Changes

- Don't require elevation just to check dev dependencies (asklar@microsoft.com)

## 0.0.0-canary.188

Sat, 24 Oct 2020 05:07:36 GMT

### Changes

- fix tab ordering on ScrollViewer, TextInput, and Picker (kmelmon@microsoft.com)
- Integrate RNTester Redesign (8/26 Nightly Build) (ngerlem@microsoft.com)
- Fix failing unit tests (vmorozov@microsoft.com)
- Ensure that destruction of animation nodes happen on UI thread (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.187

Fri, 23 Oct 2020 05:04:37 GMT

### Changes

- Enable onFocus() and onBlur() callbacks on Pressable. Allow calling .focus() and .blur() on Pressable. (igklemen@microsoft.com)

## 0.0.0-canary.186

Thu, 22 Oct 2020 05:06:28 GMT

### Changes

- Fix ReactContext::EmitJSEvent arg passing (vmorozov@microsoft.com)
- raise change event when dimensions change (kmelmon@microsoft.com)
- Integrate 8/25 Nightly RN Build (ngerlem@microsoft.com)
- Move UIManager to NM2 infra (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.185

Wed, 21 Oct 2020 05:05:24 GMT

### Changes

- Fix Crash when Logbox shown on pre-19H1 (ngerlem@microsoft.com)
- Make native modules to use JS queue (vmorozov@microsoft.com)
- Drop Issue Requirement from Override Manifest Schema Validation (ngerlem@microsoft.com)

## 0.0.0-canary.184

Tue, 20 Oct 2020 05:03:56 GMT

### Changes

- Switch was generating spurious events which could result on two events (on/off) being sent before the first event was fully handled (gill.peacegood@axsy.com)
- Update Chakra JSI implementation (vmorozov@microsoft.com)
- Sync native module functions are wrapping the result in an array, which it shouldnt (30809111+acoates-ms@users.noreply.github.com)

## 0.0.0-canary.181

Sat, 10 Oct 2020 05:04:10 GMT

### Changes

- Fixed missing folly headers on publish (jthysell@microsoft.com)

## 0.0.0-canary.180

Fri, 09 Oct 2020 05:06:32 GMT

### Changes

- removed no longer used polyester controls (chiara.mooney@gmail.com)
- Change order of JavaScript tests to get failures earlier (dannyvv@microsoft.com)
- Integrate 8/20 Nightly RN Build (Extract RNTester) (ngerlem@microsoft.com)
- Update System.Private.Uri version for C# codegen for unittest (dannyvv@microsoft.com)
- Consolidate WebSocketModules (julio.rocha@microsoft.com)
- update folly version to fix folly bug 1454 (asklar@winse.microsoft.com)

## 0.0.0-canary.179

Tue, 06 Oct 2020 05:04:27 GMT

### Changes

- Remove ILegacyReactInstance, and react::uwp::IReactInstance (30809111+acoates-ms@users.noreply.github.com)
- Update System.Private.Uri version for C# codegen (dannyvv@microsoft.com)

## 0.0.0-canary.178

Sun, 04 Oct 2020 05:04:25 GMT

### Changes

- Remove the other TimingModule Assertion (ngerlem@microsoft.com)

## 0.0.0-canary.177

Sat, 03 Oct 2020 05:04:45 GMT

### Changes

- Fix native modules not being re-initialized on reload (tn0502@gmail.com)
- Integrate 8/19 Nightly RN Build (ngerlem@microsoft.com)
- Allow Default baseVersion in Override Manfests (ngerlem@microsoft.com)

## 0.0.0-canary.176

Fri, 02 Oct 2020 05:05:22 GMT

### Changes

- Expose getters for some instance settings on reactcontext (30809111+acoates-ms@users.noreply.github.com)
- Integrate 8/5 Nightly RN Build (ngerlem@microsoft.com)
- Image - fix pre-1903 crash (email not defined)
- Integrate 8/17 Nightly RN Build (ngerlem@microsoft.com)
- Use lessthrow_await_adapter in Desktop WebSocket resource (julio.rocha@microsoft.com)

## 0.0.0-canary.175

Thu, 01 Oct 2020 05:08:11 GMT

### Changes

- Integrate 7/29 Nightly Build (ngerlem@microsoft.com)
- Remove Microsoft user names from sourcecode (dannyvv@microsoft.com)
- ESLint Fixup (ngerlem@microsoft.com)

## 0.0.0-canary.174

Wed, 30 Sep 2020 05:06:49 GMT

### Changes

- Simplify react::uwp::IReactInstance (vmorozov@microsoft.com)
- Initial ABI-safe JSI implementation (vmorozov@microsoft.com)
- Add react-native-platform-override macOS Support (nick@nickgerleman.com)
- integrate RN 7/7 build (igklemen@microsoft.com)

## 0.0.0-canary.173

Tue, 29 Sep 2020 05:05:53 GMT

### Changes

- Remove Special UI When Debugger Attached (ngerlem@microsoft.com)
- Reduce Microsoft.ReactNative PCH By 155MB-260MB (ngerlem@microsoft.com)
- Remove TestMessageQueueThread. (julio.rocha@microsoft.com)
- Don't reload the ReactNativeHost multiple times (ryan.fowler@singlewire.com)

## 0.0.0-canary.172

Sat, 26 Sep 2020 05:04:21 GMT

### Changes

- Use WinRT HTTP module (julio.rocha@microsoft.com)
- resizing the xaml island should dismiss any flyouts in it (asklar@winse.microsoft.com)

## 0.0.0-canary.171

Fri, 25 Sep 2020 05:05:40 GMT

### Changes

- Supply UniversalApiContract for Platform.Version (ngerlem@microsoft.com)
- Build fixes from folly update (30809111+acoates-ms@users.noreply.github.com)
- Add -NoProfile to powershell invocations to ensure deterministic script execution where user profiles do not have the ability to break or change the build outputs (dannyvv@microsoft.com)
- Add -UseBasicParsing to all powershell Invoke-WebRequest calls to avoid dependency on IE (dannyvv@microsoft.com)

## 0.0.0-canary.169

Wed, 23 Sep 2020 05:04:55 GMT

### Changes

- Handle ReactPromise correctly (zihanc@microsoft.com)
- Remove LocationObserverModule (ngerlem@microsoft.com)

## 0.0.0-canary.168

Tue, 22 Sep 2020 04:46:10 GMT

### Changes

- Make Alert track its xamlroot's size changes (asklar@winse.microsoft.com)
- Run Non-UI-Thread Core Modules on the JS Thread (ngerlem@microsoft.com)
- Don't Elongate Frame-Based Timers (ngerlem@microsoft.com)
- Upgrade react-native Typings (ngerlem@microsoft.com)

## 0.0.0-canary.167

Mon, 21 Sep 2020 05:06:22 GMT

### Changes

- Fixup eslint config (ngerlem@microsoft.com)
- Remove Sporaddic Assert on Instance Reload from TimingModule (ngerlem@microsoft.com)

## 0.0.0-canary.166

Sun, 20 Sep 2020 05:05:06 GMT

### Changes

- use string_view in folly (asklar@winse.microsoft.com)

## 0.0.0-canary.165

Sat, 19 Sep 2020 05:05:08 GMT

### Changes

- Adding new native module template support to react-native-windows-init (jthysell@microsoft.com)

## 0.0.0-canary.164

Fri, 18 Sep 2020 05:05:24 GMT

### Changes

- Update Nuget packages (dannyvv@microsoft.com)
- Port REACT_MODULE() from react-native-windows to react-native-macos (2/4) (zihanc@microsoft.com)
- Fix rnw-dependencies for cases where the installer is installed but not VS itself (asklar@winse.microsoft.com)

## 0.0.0-canary.163

Thu, 17 Sep 2020 05:05:02 GMT

### Changes

- Enable Desktop binary WS messages (julio.rocha@microsoft.com)
- Re-enable RNTesterIntegrationTests::Logging (julio.rocha@microsoft.com)

## 0.0.0-canary.162

Wed, 16 Sep 2020 05:05:28 GMT

### Changes

- WinRTWebSocket: Drop concurrent_queue. (julio.rocha@microsoft.com)
- Update folly (asklar@winse.microsoft.com)
- Enable reload while editing of files in vnext/src (acoates-ms@noreply.github.com)

## 0.0.0-canary.161

Tue, 15 Sep 2020 05:05:15 GMT

### Changes

- Fix issue loading offline bundle after failing to load from packager (acoates-ms@noreply.github.com)
- Use x86 program files to locate UWP SDK (asklar@microsoft.com)
- Port REACT_MODULE() from react-native-windows to react-native-macos (1/4) (zihanc@microsoft.com)

## 0.0.0-canary.160

Mon, 14 Sep 2020 22:03:37 GMT

### Changes

- Reconcile package versions (ngerlem@microsoft.com)

## 0.0.0-canary.159

Sat, 12 Sep 2020 05:08:52 GMT

### Changes

- Integrate nightly 7/1 RN build (igklemen@microsoft.com)
- Increase WebSocketJSExecutor Retry Count (ngerlem@microsoft.com)
- Re-enable WebSocket SSL test (julio.rocha@microsoft.com)
- remove deprecated AppTheme code (email not defined)

## 0.0.0-canary.158

Thu, 10 Sep 2020 05:05:10 GMT

### Changes

- Fix Text borders not rendering. (igklemen@microsoft.com)
- Make Chrome optional in rnw-deps (asklar@winse.microsoft.com)
- Fix Closing a UWP Websocket (ngerlem@microsoft.com)

## 0.0.0-canary.157

Wed, 09 Sep 2020 05:06:35 GMT

### Changes

- Enable @typescript-eslint/no-floating-promises (ngerlem@microsoft.com)

## 0.0.0-canary.156

Sat, 05 Sep 2020 05:06:01 GMT

### Changes

- yellowbox AppTheme (email not defined)
- Update V8 and Hermes package versions (tudor.mihai@microsoft.com)

## 0.0.0-canary.155

Thu, 03 Sep 2020 05:05:02 GMT

### Changes

- Initial AccessibilityInfo implementation (acoates-ms@noreply.github.com)

## 0.0.0-canary.154

Wed, 02 Sep 2020 05:05:16 GMT

### Changes

- Keep Templates in react-native-windows Package (ngerlem@microsoft.com)
- Integrate 6-18 Nightly RN Build (ngerlem@microsoft.com)
- Integrate 6/25 Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.153

Tue, 01 Sep 2020 05:05:10 GMT

### Changes

- Fix 5870: Dont check for inheritance, but do explicit type check (dannyvv@microsoft.com)

## 0.0.0-canary.152

Sat, 29 Aug 2020 05:06:29 GMT

### Changes

- Add Desktop ARM64 support (julio.rocha@microsoft.com)
- Fix fast refresh (acoates-ms@noreply.github.com)
- Remove ShadowNodes left in UIManager dtor (email not defined)

## 0.0.0-canary.151

Fri, 28 Aug 2020 05:06:54 GMT

### Changes

- Launch application for all ActivationKinds (ryan.fowler@singlewire.com)
- Fixed templates so C++ apps can consume C# native modules (jthysell@microsoft.com)
- Add verification to ensure nuget and npm version match. (dannyvv@microsoft.com)

## 0.0.0-canary.150

Thu, 27 Aug 2020 05:05:29 GMT

### Changes

- Remove acceptsKeyboardFocus (for real this time) (ngerlem@microsoft.com)
- Harden ReactInstanceWin::NativeUIManager to return null when instance is being torn down (acoates-ms@noreply.github.com)

## 0.0.0-canary.149

Wed, 26 Aug 2020 05:07:06 GMT

### Changes

- Fix C# interop serialization for JsValueObject (dannyvv@microsoft.com)
- Add API to turn off 3D perspective to enable controls that don't work in 3D (asklar@winse.microsoft.com)
- Fix bugs from acceptsKeyboardFocus to focusable deprecation (ngerlem@microsoft.com)

## 0.0.0-canary.147

Mon, 24 Aug 2020 05:04:58 GMT

### Changes

- Ensure install of VS desktop/C++ workload (asklar@winse.microsoft.com)

## 0.0.0-canary.146

Sat, 22 Aug 2020 05:07:27 GMT

### Changes

- Remove dead mousemove code (ngerlem@microsoft.com)
- Integrate 6/11 Nightly Build (ngerlem@microsoft.com)

## 0.0.0-canary.145

Fri, 21 Aug 2020 05:04:58 GMT

### Changes

- Integrate 6/4 Nightly RN Build (ngerlem@microsoft.com)
- Upgrade to OpenSSL 1.0.2p.5 (julio.rocha@microsoft.com)
- Revert "Use explicit calling convention for IWebSocketResource::Make (#5778)" (asklar@winse.microsoft.com)
- Integrate 5/28 Nightly RN Build (ngerlem@microsoft.com)

## 0.0.0-canary.144

Thu, 20 Aug 2020 05:05:23 GMT

### Changes

- Move DevMenu to use XAML markup file (asklar@winse.microsoft.com)
- Integrate 5/16 Nightly RN Build (ngerlem@microsoft.com)
- Export IWebSocketResource::Make as __cdecl (julio.rocha@microsoft.com)

## 0.0.0-canary.143

Wed, 19 Aug 2020 05:04:41 GMT

### Changes

- Update nuspec copyright header to use xml entity as some of the powershell processing we do messed up the encoding (dannyvv@microsoft.com)
- Update V8 package version (tudor.mihai@microsoft.com)

## 0.0.0-canary.142

Tue, 18 Aug 2020 05:06:49 GMT

### Changes

- fix instability in VisitAllPages test (kmelmon@microsoft.com)

## 0.0.0-canary.141

Sat, 15 Aug 2020 05:05:18 GMT

### Changes

- Bump V8 package version (tudor.mihai@microsoft.com)
- remove textbox for tooltips in hyperlinks as that can mess up word-wrapping (asklar@microsoft.com)
- Restore Previous Behavior Allowing onMouseEnter and onMouseLeave on Touchables (ngerlem@microsoft.com)
- Integrate 5/4 nightly build. (igklemen@microsoft.com)

## 0.0.0-canary.140

Fri, 14 Aug 2020 05:05:34 GMT

### Changes

- Integrate 4/29 Nightly Build (ngerlem@microsoft.com)
- Print out Wininet error strings (asklar@microsoft.com)
- Remove reinterpret_cast in DevSupportManager (asklar@winse.microsoft.com)
- Fix crash in ViewViewManager (asklar@winse.microsoft.com)
- Fix teardown crash in WebSocketJSExecutor (asklar@microsoft.com)

## 0.0.0-canary.139

Thu, 13 Aug 2020 05:07:00 GMT

### Changes

- Fix Errant "Nested elements in TextInput are currently unsupported on Windows" Yellowbox Warnings (ngerlem@microsoft.com)
- Add licene metadata to nuget package for nuget.org compliance (dannyvv@microsoft.com)
- Use thread compositor in ReactImage (asklar@microsoft.com)

## 0.0.0-canary.138

Wed, 12 Aug 2020 05:05:16 GMT

### Changes

- Expose instance life cycle events (email not defined)
- Implement textTransform (asklar@microsoft.com)
- Integrate 4/22 Nightly Build (ngerlem@microsoft.com)
- Remove compiler flags disabling FH4 (ngerlem@microsoft.com)

## 0.0.0-canary.137

Tue, 11 Aug 2020 07:36:05 GMT

### Changes

- Add Get/SetRuntimeOptionInt. (julio.rocha@microsoft.com)
- Update ChakraCore to 1.11.20 (jthysell@microsoft.com)
- Make C# CodeGen the default (dannyvv@microsoft.com)
- Keep dependencies in sync with react-native (ngerlem@microsoft.com)
- add transparent background to fix hit-testing issue (kmelmon@microsoft.com)
- change to member as static DOs aren't safe in multi window apps (kmelmon@microsoft.com)
- Fix label of RedBox reload button (email not defined)
- Disable Hermes by default (tudor.mihai@microsoft.com)

## 0.0.0-canary.136

Thu, 06 Aug 2020 05:05:16 GMT

### Changes

- Split IViewManager into core and ui-dependent parts to remove dependency on XAML from Core projects (asklar@microsoft.com)

## 0.0.0-canary.135

Wed, 05 Aug 2020 05:05:11 GMT

### Changes

- Use string object for tooltips instead of textblocks since that breaks wordwrap etc (asklar@microsoft.com)
- Handle MessageWebSocket constructor exceptions (julio.rocha@microsoft.com)
- Fix Crash Using accessibilityValue Without Web Debugging (ngerlem@microsoft.com)
- Add option to devmenu to configure bundler (email not defined)

## 0.0.0-canary.134

Fri, 31 Jul 2020 05:05:10 GMT

### Changes

- WinUI 3 islands support (asklar@microsoft.com)

## 0.0.0-canary.133

Thu, 30 Jul 2020 05:04:47 GMT

### Changes

- Share packager connection between instances to same packager (email not defined)

## 0.0.0-canary.132

Wed, 29 Jul 2020 05:04:59 GMT

### Changes

- Update codegen targets for source mode to use property instead of items (dannyvv@microsoft.com)

## 0.0.0-canary.131

Tue, 28 Jul 2020 05:06:13 GMT

### Changes

- Separate local-cli into its own package (ngerlem@microsoft.com)
- Clang tidy, wave 2 (asklar@microsoft.com)
- Deploy apps in CI because we want to ensure we are catching cli deployment issues (asklar@microsoft.com)

## 0.0.0-canary.130

Sun, 26 Jul 2020 05:06:58 GMT

### Changes

- Integrate Up To The Base of React Native 0.63 (ngerlem@microsoft.com)

## 0.0.0-canary.129

Sat, 25 Jul 2020 05:04:34 GMT

### Changes

- clang tidy, wave 1 (asklar@microsoft.com)

## 0.0.0-canary.128

Fri, 24 Jul 2020 05:05:51 GMT

### Changes

- Stop placing Microsoft.ReactNative.Cxx in the Microsoft.ReactNative nuget package (dannyvv@microsoft.com)
- Update V8 package version (tudorm@microsoft.com)

## 0.0.0-canary.127

Thu, 23 Jul 2020 05:04:42 GMT

### Changes

- move legacy tests to end of list (kmelmon@microsoft.com)

## 0.0.0-canary.126

Wed, 22 Jul 2020 05:04:42 GMT

### Changes

- move e2etest pages into rntester (kmelmon@microsoft.com)
- Implement Image tintColor style prop (email not defined)
- move to winui3 preview 2 (asklar@microsoft.com)
- Use compositor in TLS (asklar@microsoft.com)

## 0.0.0-canary.125

Tue, 21 Jul 2020 05:05:04 GMT

### Changes

- Remove ReactWindows-Universal.sln and update publish.yml to use Microsoft.React-native.sln (dannyvv@microsoft.com)
- Display expected answers and default one (sam@elborai.me)
- Update e2e-testing.md (dannyvv@microsoft.com)
- Add windows adk to machine setup script (for build lab) (dannyvv@microsoft.com)

## 0.0.0-canary.124

Sat, 18 Jul 2020 05:04:20 GMT

### Changes

- Fix layout deploy (asklar@microsoft.com)
- Enabeling SourceLink support. (dannyvv@microsoft.com)
- Add fix for msbuild long paths in fixup script (dannyvv@microsoft.com)

## 0.0.0-canary.123

Fri, 17 Jul 2020 05:06:31 GMT

### Changes

- Codesign Microsoft.ReactNative binaries and nuget package (disabled) (jthysell@microsoft.com)

## 0.0.0-canary.122

Thu, 16 Jul 2020 05:04:45 GMT

### Changes

- Implement Image blurRadius (email not defined)
- Add support for `Task` and `Task<T>` async methods for C# native modules. (dannyvv@microsoft.com)
- Add debugging output for image load failures (asklar@microsoft.com)
- Have Desktop int. tests consume DLL. (julio.rocha@microsoft.com)

## 0.0.0-canary.121

Wed, 15 Jul 2020 05:04:55 GMT

### Changes

- Move some templates to shared location. (dannyvv@microsoft.com)

## 0.0.0-canary.120

Tue, 14 Jul 2020 05:05:25 GMT

### Changes

- don't error out on errors from installing dependent framework packages (asklar@microsoft.com)
- Start using xamlroot's compositor when available for islands to work. This enables most of playground win32 for rnw-on-winui3 for a pre-preview 2 build (asklar@microsoft.com)

## 0.0.0-canary.119

Sun, 12 Jul 2020 05:05:04 GMT

### Changes

- Move index file to flow (acoates@microsoft.com)

## 0.0.0-canary.118

Sat, 11 Jul 2020 05:09:38 GMT

### Changes

- Move contents of ReactDirectories.props to Directory.build.props (ngerlem@microsoft.com)
- Define Set/Get FeatureGate (julio.rocha@microsoft.com)
- Create nuget package for Microsoft.ReacNative.Cxx (dannyvv@microsoft.com)
- [Image] Remove resizemode assert (email not defined)
- Automatically Pull In New Files from RNTester and IntegrationTests (ngerlem@microsoft.com)

## 0.0.0-canary.117

Fri, 10 Jul 2020 05:06:06 GMT

### Changes

- Make IWebSocketResource methods noexcept. (julio.rocha@microsoft.com)
- run-windows cannot find built app with default init app (acoates@microsoft.com)
- Create nuget package for Managed CodeGen (dannyvv@microsoft.com)

## 0.0.0-canary.116

Wed, 08 Jul 2020 05:04:59 GMT

### Changes

- Dont try to parseJson unless redbox error starts with { (acoates@microsoft.com)
- Deploy dependencies when registering from layout (asklar@microsoft.com)

## 0.0.0-canary.115

Wed, 08 Jul 2020 01:27:37 GMT

### Changes

- Add a bunch of deps we are using but not declaring (acoates@microsoft.com)
- Fix #4809: Remove the /flow folder from the shipping npm package (dannyvv@microsoft.com)
- Rever to new experimentalNuGetDependency Naming (ngerlem@microsoft.com)

## 0.0.0-canary.114

Tue, 07 Jul 2020 05:05:23 GMT

### Changes

- Enable experimental usage of Microsoft.ReactNative.Managed as a nuget package (dannyvv@microsoft.com)

## 0.0.0-canary.113

Mon, 06 Jul 2020 23:11:46 GMT

### Changes

- Add Support for Binary Overrides to react-native-platform-override (ngerlem@microsoft.com)
- Create a Programmatic API for react-native-platform-override (ngerlem@microsoft.com)

## 0.0.0-canary.112

Fri, 03 Jul 2020 05:05:03 GMT

### Changes

- Enforce null/undefined safety for the CLI (ngerlem@microsoft.com)
- Fix #4810: Remove the /vnext/etc/react-native-windows.api.md file from the shipping npm package (dannyvv@microsoft.com)
- Remove github actions workflow for CI and publish (dannyvv@microsoft.com)
- Include "nuget.exe" as Package Dependency of react-native-windows (ngerlem@microsoft.com)
- Replace Custom Generator Prompt With Inquirer (ngerlem@microsoft.com)
- Cleanup VS Installation Related CLI code (ngerlem@microsoft.com)
- add guard to ensure only one version of XAML is loaded (asklar@microsoft.com)

## 0.0.0-canary.111

Thu, 02 Jul 2020 05:05:06 GMT

### Changes

- Migrate 'AppState' to turbo module (zihanc@microsoft.com)
- Migrate 'DeviceInfo' to turbo module (zihanc@microsoft.com)
- Migrate 'I18nManager' to turbo module (zihanc@microsoft.com)

## 0.0.0-canary.110

Wed, 01 Jul 2020 05:06:19 GMT

### Changes

- Fix compile warnings (dannyvv@microsoft.com)
- Remove delayload dependencies on some winrt apisets, to reduce warning count (dannyvv@microsoft.com)
- Migrate 'DevSettings' to turbo module (zihanc@microsoft.com)
- Migrate 'LogBox' to turbo module (zihanc@microsoft.com)
- Implement REACT_CONSTANT as getConstants sync method for turbo module (zihanc@microsoft.com)
- Fix a few more casing typos (dannyvv@microsoft.com)
- Share eslint and Typescript configs across packages (ngerlem@microsoft.com)
- Migrate the local-cli to TypeScript (ngerlem@microsoft.com)
- Migrate "Alert" to turbo module (zihanc@microsoft.com)

## 0.0.0-canary.109

Tue, 30 Jun 2020 05:06:03 GMT

### Changes

- Call AppPolicyGetWindowingModel dynamically since it's WACK-banned (asklar@microsoft.com)

## 0.0.0-canary.108

Mon, 29 Jun 2020 05:04:57 GMT

### Changes

- Removed references to ReactWindowsCore (julio.rocha@microsoft.com)

## 0.0.0-canary.107

Sun, 28 Jun 2020 05:04:51 GMT

### Changes

- Fix casing typos in NuGet and MSBuild (dannyvv@microsoft.com)
- Remove Include/ReactUWP files (vmorozov@microsoft.com)
- Fix build errors for codegen unittest project when building in VS2019 (dannyvv@microsoft.com)

## 0.0.0-canary.106

Sat, 27 Jun 2020 03:52:38 GMT

### Changes

- Add Enterprise vs2019 support to rnw-dependencies (dannyvv@microsoft.com)

## 0.0.0-canary.104

Fri, 26 Jun 2020 23:59:27 GMT

### Changes

- Upgrade React Native to 3/22 Nightly Build (ngerlem@microsoft.com)

## 0.0.0-canary.103

Fri, 26 Jun 2020 20:17:03 GMT

### Changes

- Add a null check on reload callback (acoates@microsoft.com)
- Unify storage for sourceBundleHost/Port and debugHost (acoates@microsoft.com)
- Remove ReactUWP project folder (vmorozov@microsoft.com)
- Fix empty host getting passed to HMR client when SourceBundleHost not specified (acoates@microsoft.com)

## 0.0.0-canary.102

Thu, 25 Jun 2020 14:32:50 GMT

### Changes

- WinUI3 Preview 1 (asklar@microsoft.com)

## 0.0.0-canary.101

Thu, 25 Jun 2020 00:07:54 GMT

### Changes

- Use Deterministic NuGet Binary (ngerlem@microsoft.com)

## 0.0.0-canary.100

Wed, 24 Jun 2020 00:07:20 GMT

### Changes

- Fix warnings of signed/unsigned comparrison mismatches on x86 builds (dannyvv@microsoft.com)
- update csproj to use versions from winui.props (asklar@microsoft.com)
- Fix JSValueReaderGenerator runtime error for read-only properties (vmorozov@microsoft.com)

## 0.0.0-canary.99

Tue, 23 Jun 2020 00:05:47 GMT

### Changes

- start moving away from config and use a sentinel file instead (asklar@microsoft.com)

## 0.0.0-canary.98

Sat, 20 Jun 2020 00:05:18 GMT

### Changes

- Auto-linking for native modules (source-based) (jthysell@microsoft.com)
- Implement AccessibilityValue from RN 62 (igklemen@microsoft.com)
- Complete BundleUrl should be provided to JSExecutor when using fastrefresh without remote debugging (acoates@microsoft.com)
- turbo modules should be replacable (zihanc@microsoft.com)

## 0.0.0-canary.97

Fri, 19 Jun 2020 00:07:58 GMT

### Changes

- Move the addition of folly.natvis to ReactWin32.nuspec (warleu@microsoft.com)
- Fix building vcxproj files outside of solution (dannyvv@microsoft.com)

## 0.0.0-canary.96

Thu, 18 Jun 2020 00:07:44 GMT

### Changes

- Create Scripts for the Release Process (ngerlem@microsoft.com)
- Run NativeModules 2.0 in JS thread (vmorozov@microsoft.com)
- Update C++/WinRT to version 2.0.200615.7 (vmorozov@microsoft.com)
- version agnostic msbuild log viewer (asklar@microsoft.com)

## 0.0.0-canary.95

Wed, 17 Jun 2020 00:05:39 GMT

### Changes

- Unify DevSupportManager and WebSocketJSExecutor (acoates@microsoft.com)
- Rename "master" packages to "canary" (ngerlem@microsoft.com)
- Don't lint target folder and don't flow check node_modules packages (dannyvv@microsoft.com)

## 0.0.0-master.94

Tue, 16 Jun 2020 00:05:48 GMT

### Changes

-  Upgrade to boost 1.72.0. (julio.rocha@microsoft.com)
- Prevent Switches from Accidentally Becoming Disabled (ngerlem@microsoft.com)
- use default brush for border (kmelmon@microsoft.com)

## 0.0.0-master.93

Sat, 13 Jun 2020 00:09:11 GMT

### Changes

- Implement logbox (acoates@microsoft.com)
- Add Microsoft.ReactNative.Cxx to the nugets. (acoates@microsoft.com)
- Fix file lock issue wiht msbuild /bl flag and the metro bundler (dannyvv@microsoft.com)

## 0.0.0-master.92

Fri, 12 Jun 2020 00:33:44 GMT

### Changes

- Fix support for `--namespace` on react-native-windows init for cpp (dannyvv@microsoft.com)

## 0.0.0-master.91

Thu, 11 Jun 2020 00:05:14 GMT

### Changes

- Add support for redbox's on xamlislands (acoates@microsoft.com)
- Manually set Lib and Include paths for Hermes (julio.rocha@microsoft.com)
- More rnw-dependencies improvements (asklar@microsoft.com)

## 0.0.0-master.90

Wed, 10 Jun 2020 00:05:10 GMT

### Changes

- implement --proxy in CLI (igklemen@microsoft.com)
- Don't crash when using animations in RS3-RS4, just no-op (asklar@microsoft.com)
- Add support for Alert module with Xaml Islands (acoates@microsoft.com)
- fix onPress for nested Text (igklemen@microsoft.com)
- translate PS exception into error code (asklar@microsoft.com)

## 0.0.0-master.89

Tue, 09 Jun 2020 00:06:03 GMT

### Changes

- Use yoga config to align with ios/android (acoates@microsoft.com)

## 0.0.0-master.88

Mon, 08 Jun 2020 00:05:53 GMT

### Changes

- Add Integration and Component tests to CI (vmorozov@microsoft.com)

## 0.0.0-master.87

Sun, 07 Jun 2020 00:05:23 GMT

### Changes

- Remove SharedProject (dannyvv@microsoft.com)
- Update template gen to use mustache (dannyvv@microsoft.com)
- Install MSBuild log viewer (asklar@microsoft.com)
- DevMenu should only show up when DeveloperSupport is on (acoates@microsoft.com)
- Remove legacy ReactInstanceSettings struct (vmorozov@microsoft.com)

## 0.0.0-master.86

Sat, 06 Jun 2020 00:05:20 GMT

### Changes

- Expose turbo module registry API (zihanc@microsoft.com)
- Ensure Image.windows.js is an exact copy of Image.ios.js (ngerlem@microsoft.com)

## 0.0.0-master.85

Fri, 05 Jun 2020 00:06:16 GMT

### Changes

- Adding compile time Code generation for Managed React Native modules for Windows (dannyvv@microsoft.com)
- Move devmenu out of RootControl, implement more of DevSettings (acoates@microsoft.com)
- Fix SampleApp run-windows with deploy (dannyvv@microsoft.com)

## 0.0.0-master.84

Thu, 04 Jun 2020 00:05:05 GMT

### Changes

- Fix ReloadInstance crash caused by unloaded instance (vmorozov@microsoft.com)
- Re-enable RNTester integration tests. (julio.rocha@microsoft.com)

## 0.0.0-master.83

Wed, 03 Jun 2020 00:05:25 GMT

### Changes

- Deprecate acceptsKeyboardFocus (kaigu@microsoft.com)
- Remove react-native-community/cli dependency from template (acoates@microsoft.com)
- Code Cleanup: Some low haning fruit lint fixes (dannyvv@microsoft.com)
- Allow paremeterization of buildLogDirectory for msbuild task when using run-windows (dannyvv@microsoft.com)
- Minor autolinking template update prep (jthysell@microsoft.com)

## 0.0.0-master.82

Tue, 02 Jun 2020 00:07:29 GMT

### Changes

- Enable ReactNativeHost integration test (vmorozov@microsoft.com)

## 0.0.0-master.81

Mon, 01 Jun 2020 00:05:31 GMT

### Changes

- Update projects to use shared props/targets (dannyvv@microsoft.com)

## 0.0.0-master.80

Sun, 31 May 2020 00:05:21 GMT

### Changes

- blacklist all ProjectImports.zip (asklar@microsoft.com)
- docs CI (asklar@microsoft.com)

## 0.0.0-master.79

Sat, 30 May 2020 00:04:58 GMT

### Changes

- Remove references to ReactWindowsCore projects (julio.rocha@microsoft.com)
- Add XamlUIService with ElementFromReactTag (acoates@microsoft.com)
- Fixes CLI C# app not deploying (jthysell@microsoft.com)

## 0.0.0-master.78

Fri, 29 May 2020 00:09:08 GMT

### Changes

- Remove unused ABI definitions (vmorozov@microsoft.com)
- Update shared props to have csc emit absolute paths on errors and warnings (dannyvv@microsoft.com)
- add support for long paths to rnw-dependencies.ps1 (asklar@microsoft.com)

## 0.0.0-master.77

Thu, 28 May 2020 00:05:30 GMT

### Changes

- set up react application to use an app's reactrootview (asklar@microsoft.com)

## 0.0.0-master.76

Wed, 27 May 2020 00:05:42 GMT

### Changes

- Allow developers to independently set hostname and port. (12337821+nasadigital@users.noreply.github.com)
- Move I18nManager to spec based native module (acoates@microsoft.com)
- Add UIDispatcher property to ReactInstanceSettings and IReactContext (vmorozov@microsoft.com)

## 0.0.0-master.74

Sat, 23 May 2020 00:04:45 GMT

### Changes

- Implemented support for native module std::weak_ptr (vmorozov@microsoft.com)
- Fix deploy issue causing 2nd+ builds to break due to bug in appxrecipe which gets imported (asklar@microsoft.com)
- Drop LazySupportManager (julio.rocha@microsoft.com)
- Fix ReactInstance error state to avoid crashes (vmorozov@microsoft.com)
- Move DeviceInfo and AppState to new native module impl (acoates@microsoft.com)

## 0.0.0-master.73

Fri, 22 May 2020 00:05:31 GMT

### Changes

- Work around regression in app packaging in VS 16.5-16.6 (asklar@microsoft.com)
- Do not emit click events for buttons other than left. (igklemen@microsoft.com)
- Fixed Microsoft.ReactNative build in VS 16.6.0 (vmorozov@microsoft.com)
- fix for animation bug (kmelmon@microsoft.com)
- dont overwrite msbuild log when deploying (asklar@microsoft.com)
- Fix spacing and extra dot in copyright of the property sheets (dannyvv@microsoft.com)
- Standardize version of Microsoft.NETCore.UniversalWindowsPlatform to 6.2.9 (dannyvv@microsoft.com)
- Remove unneeded reference form Microsoft.ReactNative.Managed (dannyvv@microsoft.com)

## 0.0.0-master.72

Thu, 21 May 2020 00:04:53 GMT

### Changes

- deploy from layout (asklar@microsoft.com)
- ReactNotificationService to allow communications between native modules (vmorozov@microsoft.com)

## 0.0.0-master.71

Wed, 20 May 2020 00:04:49 GMT

### Changes

- Implement Keybpoarding/Gamepad Activation for Pressability (ngerlem@microsoft.com)
- Reduce need for custom metro configs (acoates@microsoft.com)
- Update the react-native-windows-init template to use the managed library the default (instead of the shared csproj file). (dannyvv@microsoft.com)

## 0.0.0-master.70

Tue, 19 May 2020 00:05:07 GMT

### Changes

- Fixes for bundle loading and back handler crash using xaml islands (acoates@microsoft.com)
- Remove legacy ReactUWP (acoates@microsoft.com)
- Support 0..n args in native module callbacks (vmorozov@microsoft.com)
- Adapt ViewWin32, etc to "accessibilityState" (ngerlem@microsoft.com)

## 0.0.0-master.69

Sun, 17 May 2020 00:04:22 GMT

### Changes

- ReactNonAbiValue for IReactPropertyBag non-ABI values (vmorozov@microsoft.com)
- Prevent hard crash on invalid jsx (button inside text e.g.) (asklar@microsoft.com)
- Fixed ReactContext CallJSFunction and EmitJSEvent (vmorozov@microsoft.com)

## 0.0.0-master.68

Sat, 16 May 2020 00:04:28 GMT

### Changes

- some projects have target instead of bin folders (asklar@microsoft.com)
- .\JustMyXaml.ps1 (asklar@microsoft.com)

## 0.0.0-master.67

Fri, 15 May 2020 00:04:44 GMT

### Changes

- winui3 option for app project template (asklar@microsoft.com)
- Reduce PCH Sizes (ngerlem@microsoft.com)
- Remove unused lifecycle events in ReactNativeHost (kaigu@microsoft.com)
- Move pch files into a Pch subfolder. (vmorozov@microsoft.com)
- exact match of WinAppDriver required (asklar@microsoft.com)

## 0.0.0-master.66

Thu, 14 May 2020 00:04:37 GMT

### Changes

- add rnw-dependencies.ps1 (asklar@microsoft.com)
- Use DispatcherQueue instead of CoreDispatcher for Mso::DispatchQueue (vmorozov@microsoft.com)
- Allow storing non-WinRT types in ReactPropertyBag (vmorozov@microsoft.com)
- revert dfc57fcf2504f57baab20f550b36a618eaa99e56 (kmelmon@microsoft.com)
- don't exit the powershell session and pause for keyboard input if interactive (asklar@microsoft.com)

## 0.0.0-master.65

Wed, 13 May 2020 00:04:21 GMT

### Changes

- add ReactNativeHost to Win32 C++/WinRT ABI (aeulitz@microsoft.com)
- Fixed ReactContext copy/move semantic (vmorozov@microsoft.com)
- Use spec file for DevSettings NativeModule (acoates@microsoft.com)

## 0.0.0-master.64

Tue, 12 May 2020 00:04:39 GMT

### Changes

- Fire onLoad event when a bitmap image is opeed (lamdoan@microsoft.com)
- Expose ability for apps to provide their own RedBox implementation (acoates@microsoft.com)

## 0.0.0-master.63

Sun, 10 May 2020 00:05:02 GMT

### Changes

- Reenable V8 for desktop projects (tudorm@microsoft.com)

## 0.0.0-master.62

Sat, 09 May 2020 00:04:42 GMT

### Changes

- Autolinking (jthysell@microsoft.com)
- Implemented PropertyBag for ReactContext and ReactInstanceSettings (vmorozov@microsoft.com)
- Align local metro config with future CLI changes (acoates@microsoft.com)
- separate desktop from uwp ReactWindowsCore since desktop uses V8 (asklar@microsoft.com)
- Set C# langversion of templates back to 7.3 from 8.0 (dannyvv@microsoft.com)
- Correct binary message tagging (julio.rocha@microsoft.com)
- Added Microsoft.ReactNative.IntegrationTests project to Microsoft.ReactNative.sln (vmorozov@microsoft.com)

## 0.0.0-master.61

Fri, 08 May 2020 00:05:16 GMT

### Changes

- Promote awaitable DispatchQueue callback to member field (julio.rocha@microsoft.com)
- Upgrade the shared managed project to a proper csharp library. (dannyvv@microsoft.com)
- Ensure InitializeCore is run before app code (acoates@microsoft.com)

## 0.0.0-master.60

Thu, 07 May 2020 00:05:07 GMT

### Changes

- Don't delete nuget.exe if using locally-installed (e.g. chocolatey) (asklar@microsoft.com)
- Only bundle when not running Intellisense builds (asklar@microsoft.com)
- Disable CxxFrameHandler4 (ngerlem@microsoft.com)
- Switch package layout to use `files` in `package.json` rather than `.npmignore` (dannyvv@microsoft.com)

## 0.0.0-master.59

Wed, 06 May 2020 00:05:03 GMT

### Changes

- WinUI 3 Alpha support in RNW (asklar@microsoft.com)
- Treat yes the same as y in template overwrite prompt (acoates@microsoft.com)

## 0.0.0-master.58

Tue, 05 May 2020 00:04:31 GMT

### Changes

- Make generated msbuild project compile in parralel (/m) (dannyvv@microsoft.com)
- fix appPackage path (kaigu@microsoft.com)

## 0.0.0-master.57

Sun, 03 May 2020 00:04:42 GMT

### Changes

- Implement WinRTWebSocketResource unit tests (julio.rocha@microsoft.com)

## 0.0.0-master.56

Sat, 02 May 2020 00:05:12 GMT

### Changes

- - Change MouseEnter/MouseLeave behavior to match web behavior - Improve performance of frequently-called mouse move handler (aschultz@microsoft.com)
- Delay load ChakraCore.dll (tudorm@microsoft.com)

## 0.0.0-master.55

Fri, 01 May 2020 00:04:15 GMT

### Changes

- Removed the ref count work around from classes derived from ReactApplications (vmorozov@microsoft.com)
- Minor cleanup: Remove unused namespace from C# files (dannyvv@microsoft.com)

## 0.0.0-master.54

Thu, 30 Apr 2020 18:40:40 GMT

### Changes

- Expose YellowBox functionality to native code (asklar@microsoft.com)
- .net projects need more work to build a package layout (asklar@microsoft.com)
- Package Fixes (ngerlem@microsoft.com)

## 0.0.0-master.53

Thu, 30 Apr 2020 00:04:27 GMT

### Changes

- auto-detect RTL and push into root view (kmelmon@microsoft.com)
- Refactor TestHook out of ViewManagerBase and special case layout properties (asklar@microsoft.com)
- Expose Debug macro for hresult_error. Start catching these... (asklar@microsoft.com)
- Try uninstalling without elevation in case since it is not needed for layout installs (debug) (asklar@microsoft.com)

## 0.0.0-master.52

Wed, 29 Apr 2020 00:04:54 GMT

### Changes

- Start forking namespace and includes for WUX->MUX move (asklar@microsoft.com)
- Update react version (kaigu@microsoft.com)
- Exclude unittests and proposals from being published in the npm package (dannyvv@microsoft.com)

## 0.0.0-master.51

Tue, 28 Apr 2020 00:04:13 GMT

### Changes

- Added C# ReactContext for Module Initialize method (vmorozov@microsoft.com)
- Use readme file from root of repo for react-native-windows (acoates@microsoft.com)

## 0.0.0-master.50

Mon, 27 Apr 2020 00:04:59 GMT

### Changes

- Added C++ ReactContext for Module Initialize method (vmorozov@microsoft.com)

## 0.0.0-master.49

Sun, 26 Apr 2020 00:04:59 GMT

### Changes

- Use separate brush for text control border (kaigu@microsoft.com)
- Fix typo causing activity indicators to be sad and colorless (asklar@microsoft.com)
- Disable copying V8 dll to target output; we need a clearer way to distinguish uwp vs. win32 in the build system but this will do for now to unblock #4475 (tudorm@microsoft.com)

## 0.0.0-master.48

Sat, 25 Apr 2020 00:05:26 GMT

### Changes

- Better error reporting and elevating if necessary in CLI (asklar@microsoft.com)
- Detect when we have more than one SLN as it may not be obvious to the user. Add --sln option to CLI. (asklar@microsoft.com)
- Install from layout in Debug builds (asklar@microsoft.com)

## 0.0.0-master.47

Fri, 24 Apr 2020 00:04:41 GMT

### Changes

- Pass XamlView's through ReactUwp View Managers by Const Ref (ngerlem@microsoft.com)
- fix RTL (kmelmon@microsoft.com)
- Use spec file for clipboard module (acoates@microsoft.com)
- Fix Missing Include in DevSupportManagerUwp (ngerlem@microsoft.com)
- Fixed .Net Native crash for custom struct deserialization (vmorozov@microsoft.com)

## 0.0.0-master.46

Thu, 23 Apr 2020 00:04:37 GMT

### Changes

- Hook up to packager websocket to allow 'r' to reload instance (acoates@microsoft.com)
- Handle syntax errors in RedBox (asklar@microsoft.com)
- Publish ship and debug bits in the nuget (acoates@microsoft.com)
- Implement String ViewManager Command IDs (ngerlem@microsoft.com)

## 0.0.0-master.45

Wed, 22 Apr 2020 00:04:29 GMT

### Changes

- Make default template build non-dev bundle when not using debug configuration (acoates@microsoft.com)
- Implemented C++ TurboModule compile time spec validation (vmorozov@microsoft.com)
- Fix devtools connection to match RN0.62 (acoates@microsoft.com)

## 0.0.0-master.44

Sun, 19 Apr 2020 00:04:29 GMT

### Changes

- fix include paths (kmelmon@microsoft.com)

## 0.0.0-master.43

Sat, 18 Apr 2020 00:04:34 GMT

### Changes

- Allow enabling of direct debugging through the CLI. (12337821+nasadigital@users.noreply.github.com)
- Implelent Shared StatusBarManagerModule and Do Module Cleanup (ngerlem@microsoft.com)
- Implement AppearanceModule (ngerlem@microsoft.com)
- Add support for React Native BackHandler API (jahiggin@microsoft.com)
- remove AccessibilityStates (kmelmon@microsoft.com)
- Fix issue with yoga layout in x64 release (acoates@microsoft.com)

## 0.0.0-master.42

Fri, 17 Apr 2020 00:04:27 GMT

### Changes

- run_wdio.js will run the tests but also set the exit code to zero/non-zero on success/failure respectively. This is important to break the CI/PR build on test failures, which we weren't doing until now. (asklar@winse.microsoft.com)
- Add msbuild.ProjectImports.zip to default blacklist to avoid metro error on run-windows (acoates@microsoft.com)
- implement accessibilityState (kmelmon@microsoft.com)

## 0.0.0-master.41

Wed, 15 Apr 2020 23:02:16 GMT

### Changes

- Fix PickerWindows (and DatePickerExample Page) (ngerlem@microsoft.com)

## 0.0.0-master.40

Wed, 15 Apr 2020 22:26:36 GMT

### Changes

- Use a custom resolver wrapper to allow metro to run for multiple platforms at once (acoates@microsoft.com)

## 0.0.0-master.39

Wed, 15 Apr 2020 19:18:34 GMT

### Changes

- Rename DeveloperSettings.SourceBundlePath -> SourceBundleName (rezha@microsoft.com)

## 0.0.0-master.38

Wed, 15 Apr 2020 07:53:39 GMT

### Changes

- fix bug where not specifying height on TextInput causes 0 height (kmelmon@microsoft.com)

## 0.0.0-master.37

Tue, 14 Apr 2020 19:34:42 GMT

### Changes

- [generator] Use `npx` for consistency (eloy.de.enige@gmail.com)

## 0.0.0-master.36

Tue, 14 Apr 2020 18:14:12 GMT

### Changes

- Elevate to install app certificate if necessary (asklar@microsoft.com)

## 0.0.0-master.35

Tue, 14 Apr 2020 14:59:17 GMT

### Changes

- Upgrade to React Native 0.62 (ngerlem@microsoft.com)

## 0.0.0-master.34

Tue, 14 Apr 2020 02:46:30 GMT

### Changes

- add missing headers to proj file (kmelmon@microsoft.com)

## 0.0.0-master.33

Mon, 13 Apr 2020 18:45:40 GMT

### Changes

- Remove Folly ContinueOnError (ngerlem@microsoft.com)

## 0.0.0-master.32

Mon, 13 Apr 2020 07:51:05 GMT

### Changes

- Refactor View managers to detect unimplemented props (asklar@microsoft.com)

## 0.0.0-master.31

Sat, 11 Apr 2020 02:36:32 GMT

### Changes

- Improve RedBox ux, and don't require elevation unless it's the first time running the build (and need to enable dev mode) (asklar@microsoft.com)

## 0.0.0-master.30

Fri, 10 Apr 2020 22:03:49 GMT

### Changes

- Simplified C++ macros and improved their comments (vmorozov@microsoft.com)

## 0.0.0-master.29

Fri, 10 Apr 2020 14:44:13 GMT

### Changes

- Don't set readyState on connection error (julio@rochsquadron.net)

## 0.0.0-master.28

Fri, 10 Apr 2020 06:15:31 GMT

### Changes

- Improve inner loop and error reporting (local CLI) (asklar@microsoft.com)

## 0.0.0-master.27

Thu, 09 Apr 2020 21:36:07 GMT

### Changes

- Enable managed unit tests (jthysell@microsoft.com)

## 0.0.0-master.26

Thu, 09 Apr 2020 17:16:44 GMT

### Changes

- Creating new Microsoft.ReactNative.sln without legacy ReactUWP (jthysell@microsoft.com)

## 0.0.0-master.25

Wed, 08 Apr 2020 19:59:18 GMT

### Changes

- Remove using namespace in ReactPackageProvider.h (kaigu@microsoft.com)

## 0.0.0-master.24

Wed, 08 Apr 2020 17:48:10 GMT

### Changes

- Rework more publish logic - produce nugets in PRs (acoates@microsoft.com)

## 0.0.0-master.23

Wed, 08 Apr 2020 13:58:29 GMT

### Changes

- Apply version numbers to Microsoft.ReactNative (acoates@microsoft.com)

## 0.0.0-master.22

Wed, 08 Apr 2020 00:50:14 GMT

### Changes

- Optimizing VS component installer to speed up CI builds (jthysell@microsoft.com)

## 0.0.0-master.21

Tue, 07 Apr 2020 21:11:39 GMT

### Changes

- placeholderTextColor for secureTextEntry on TextInput (dzndlhov@microsoft.com)

## 0.0.0-master.20

Tue, 07 Apr 2020 20:21:01 GMT

### Changes

- Fix issue adding react-native-windows to a package in a monorepo (acoates@microsoft.com)

## 0.0.0-master.19

Tue, 07 Apr 2020 18:46:38 GMT

### Patches

- Start of implementation for nuget template (acoates@microsoft.com)

## 0.0.0-master.18

Tue, 07 Apr 2020 07:15:14 GMT

### Changes

- Enabling arm64 where possible (jthysell@microsoft.com)

## 0.0.0-master.17

Tue, 07 Apr 2020 06:29:14 GMT

### Changes

- Clean up normalization (ngerlem@microsoft.com)

## 0.0.0-master.16

Tue, 07 Apr 2020 00:48:37 GMT

### Changes

- undefined (julio@rochsquadron.net)

## 0.0.0-master.15

Mon, 06 Apr 2020 21:15:49 GMT

### Changes

- Update V8 package version (tudorm@microsoft.com)

## 0.0.0-master.14

Sat, 04 Apr 2020 05:07:07 GMT

### Changes

- Clean Up Directory Logic to Fix Publish (ngerlem@microsoft.com)

## 0.0.0-master.13
Thu, 02 Apr 2020 23:27:21 GMT

### Changes

- Fix VS 2019 16.5 build errors (jthysell@microsoft.com)
## 0.0.0-master.12
Thu, 02 Apr 2020 22:39:22 GMT

### Changes

- Improve popups (aschultz@microsoft.com)
## 0.0.0-master.11
Thu, 02 Apr 2020 18:40:22 GMT

### Changes

- Switching to VS 2019 and v142 tools (jthysell@microsoft.com)
## 0.0.0-master.10
Wed, 01 Apr 2020 21:47:08 GMT

### Changes

- Fix crash when calling measure on non-framework element (acoates@microsoft.com)
## 0.0.0-master.9
Wed, 01 Apr 2020 14:24:03 GMT

### Changes

- Fixed Mso test crash (vmorozov@microsoft.com)
## 0.0.0-master.8
Wed, 01 Apr 2020 05:12:32 GMT

### Changes

- Set $(ReactNativeWindowsDir) before it is used (vmorozov@microsoft.com)
## 0.0.0-master.7
Tue, 31 Mar 2020 21:49:07 GMT

### Changes

- Update MSBuild Tasks to use MSBuild x64 (jthysell@microsoft.com)
## 0.0.0-master.6
Tue, 31 Mar 2020 20:21:56 GMT

### Changes

- 1. Make changes in response to Vladimir's feedback in the PR for bb8c00133... 2. Fix a dumb bug in multiRemove; the parameterized query variable was not being bound so nothing ever got deleted (hpratt@microsoft.com)
## 0.0.0-master.5
Tue, 31 Mar 2020 02:03:05 GMT

### Changes

- Allow user to specify direct debugging port (daseramo@microsoft.com)
## 0.0.0-master.4
Mon, 30 Mar 2020 22:21:02 GMT

### Changes

- Updated C++/WinRT to 2.0.200316.3 and Min Windows to RS3+ (10.0.16299.0) (vmorozov@microsoft.com)
## 0.0.0-master.3
Fri, 27 Mar 2020 20:29:40 GMT

### Changes

- Fix direct debugging (acoates@microsoft.com)
## 0.0.0-master.2
Thu, 26 Mar 2020 23:11:17 GMT

### Changes

- Tests for NativeModule registration without attributes (vmorozov@microsoft.com)
## 0.0.0-master.1
Thu, 26 Mar 2020 22:33:20 GMT

### Changes

- 0.0.0 Master Versioning Scheme (ngerlem@microsoft.com)
## 0.61.0-beta.75
Thu, 26 Mar 2020 19:54:26 GMT

### Changes

- Add a timeout to catch hangs during cert generation (acoates@microsoft.com)
## 0.61.0-beta.74
Thu, 26 Mar 2020 16:02:57 GMT

### Changes

- Allow NativeModules events to have 0..n arguments (vmorozov@microsoft.com)
## 0.61.0-beta.73
Thu, 26 Mar 2020 14:54:10 GMT

### Changes

- Enable basic V8 direct debugging (tudorm@microsoft.com)
## 0.61.0-beta.72
Thu, 26 Mar 2020 05:34:25 GMT

### Changes

- Fix hang on react-native-init when run from a non-elevated prompt (acoates@microsoft.com)
## 0.61.0-beta.71
Tue, 24 Mar 2020 07:05:57 GMT

### Changes

- Do not enter loaded state when an error occurs during web bundle loadJSBundles (acoates@microsoft.com)
## 0.61.0-beta.70
Mon, 23 Mar 2020 21:05:53 GMT

### Changes

- Fixed crash when trying to load invalid image uri (jthysell@microsoft.com)
## 0.61.0-beta.69
Mon, 23 Mar 2020 14:55:40 GMT

### Changes

- Implemented Linking module protocol activation (vmorozov@microsoft.com)
## 0.61.0-beta.68
Sat, 21 Mar 2020 00:52:27 GMT

### Changes

- Update V8 package (tudorm@microsoft.com)
## 0.61.0-beta.67
Thu, 19 Mar 2020 04:03:12 GMT

### Changes

- Add stub devStettingModule to react.uwp (acoates@microsoft.com)
## 0.61.0-beta.66
Wed, 18 Mar 2020 23:06:37 GMT

### Changes

- remove unused stubs (kmelmon@microsoft.com)
## 0.61.0-beta.65
Wed, 18 Mar 2020 22:00:44 GMT

### Changes

- Changed C# JSValue API to match C++ version (vmorozov@microsoft.com)
## 0.61.0-beta.64
Wed, 18 Mar 2020 20:19:18 GMT

### Changes

- Fix ReactInstanceWin::GetBundleRootPath (jthysell@microsoft.com)
## 0.61.0-beta.63
Wed, 18 Mar 2020 04:50:36 GMT

### Changes

- remove react-native-windows-extended (kmelmon@microsoft.com)
## 0.61.0-beta.62
Tue, 17 Mar 2020 23:28:32 GMT

### Changes

- Set TabIndex to 0 for Pickers (jagorrin@microsoft.com)
## 0.61.0-beta.61
Tue, 17 Mar 2020 22:54:20 GMT

### Changes

- Enable fast refresh (acoates@microsoft.com)
## 0.61.0-beta.60
Tue, 17 Mar 2020 01:55:59 GMT

### Changes

- Set TabIndex to 0 for ScrollViewers (jagorrin@microsoft.com)
## 0.61.0-beta.59
Mon, 16 Mar 2020 19:54:21 GMT

### Changes

- Remove cli.js (kaigu@microsoft.com)
## 0.61.0-beta.58
Sat, 14 Mar 2020 03:04:21 GMT

### Changes

- Add cli.js (kaigu@microsoft.com)
## 0.61.0-beta.57
Fri, 13 Mar 2020 17:37:53 GMT

### Changes

- Build unit tests in ReactWindows-Universal solution (jthysell@microsoft.com)
- Address PR comments (dida@ntdev.microsoft.com)
- Code-Gen nativeModules.g.h for auto-linking (dida@ntdev.microsoft.com)
## 0.61.0-beta.56
Thu, 12 Mar 2020 19:18:01 GMT

### Changes

- Use redbox for native errors (acoates@microsoft.com)
## 0.61.0-beta.55
Thu, 12 Mar 2020 00:01:14 GMT

### Changes

- Fix JSValueObject operator[] (vmorozov@microsoft.com)
## 0.61.0-beta.54
Wed, 11 Mar 2020 19:32:20 GMT

### Changes

- Fully implement ExceptionsManagerModule (acoates@microsoft.com)
## 0.61.0-beta.53
Wed, 11 Mar 2020 18:14:12 GMT

### Changes

- Code review feedback (acoates@microsoft.com)
## 0.61.0-beta.52
Wed, 11 Mar 2020 16:18:00 GMT

### Changes

- Fix crash in timers during instance shutdown, and in systrace (acoates@microsoft.com)
## 0.61.0-beta.51
Tue, 10 Mar 2020 21:51:40 GMT

### Changes

- Bundle command clean-up (jthysell@microsoft.com)
## 0.61.0-beta.50
Tue, 10 Mar 2020 00:33:15 GMT

### Changes

- Strip out color markers from redbox error messages (acoates@microsoft.com)
## 0.61.0-beta.49
Mon, 09 Mar 2020 20:08:13 GMT

### Changes

- Fix Support For SysTraceSection (ngerlem@microsoft.com)
## 0.61.0-beta.48
Mon, 09 Mar 2020 18:53:03 GMT

### Changes

- Add initial redbox implementation (acoates@microsoft.com)
## 0.61.0-beta.47
Sat, 07 Mar 2020 19:35:40 GMT

### Changes

- Add XamlHelpers (kaigu@microsoft.com)
## 0.61.0-beta.46
Sat, 07 Mar 2020 02:35:07 GMT

### Changes

- Add Override Manifest for Native Code (ngerlem@microsoft.com)
## 0.61.0-beta.45
Sat, 07 Mar 2020 01:43:31 GMT

### Changes

- Remove Most cxxreact Patches and Executor Sandboxing Code (ngerlem@microsoft.com)
- Deprecated JSValue typed value access functions (vmorozov@microsoft.com)
## 0.61.0-beta.44
Sat, 07 Mar 2020 01:08:09 GMT

### Changes

- Dont show loading/redbox in ship mode (acoates@microsoft.com)
## 0.61.0-beta.43
Sat, 07 Mar 2020 00:30:45 GMT

### Changes

- Bundle commands should work if you dont have react-native installed globally (acoates@microsoft.com)
## 0.61.0-beta.42
Fri, 06 Mar 2020 23:55:14 GMT

### Changes

- Add JS engine selection to Playground (tudorm@microsoft.com)
## 0.61.0-beta.41
Fri, 06 Mar 2020 22:46:28 GMT

### Changes

- Support dependency config for RNW (dida@ntdev.microsoft.com)
## 0.61.0-beta.40
Fri, 06 Mar 2020 19:29:08 GMT

### Changes

- Fix include filters (jthysell@microsoft.com)
## 0.61.0-beta.39
Thu, 05 Mar 2020 23:13:07 GMT

### Changes

- undefined (ngerlem@microsoft.com)
- Configure vnext validate-overrides to check both override manifests (ngerlem@microsoft.com)
## 0.61.0-beta.38
Thu, 05 Mar 2020 18:30:29 GMT

### Changes

- Added TryAddAttributedModule in addition to AddAttributedModules (vmorozov@microsoft.com)
## 0.61.0-beta.37
Thu, 05 Mar 2020 05:33:57 GMT

### Changes

- Move ViewPanel import from PropertyUtils to FrameworkElementViewManager (jagorrin@microsoft.com)
## 0.61.0-beta.36
Thu, 05 Mar 2020 03:41:34 GMT

### Changes

- Enable V8 support for win32 (no defaults changed) (tudorm@microsoft.com)
## 0.61.0-beta.35
Thu, 05 Mar 2020 00:11:30 GMT

### Changes

- Support RNW project config (dida@ntdev.microsoft.com)
## 0.61.0-beta.34
Wed, 04 Mar 2020 08:47:08 GMT

### Changes

- CI Enforce that Overrides are Recorded Up to Date (ngerlem@microsoft.com)
## 0.61.0-beta.33
Wed, 04 Mar 2020 08:11:46 GMT

### Changes

- Moved header files from ReactUWP\Utils to include\ReactUWP\Utils. Updated project references. (lamdoan@microsoft.com)
## 0.61.0-beta.32
Tue, 03 Mar 2020 19:02:31 GMT

### Changes

- Remove CMakeLists (ngerlem@microsoft.com)
## 0.61.0-beta.31
Mon, 02 Mar 2020 22:47:03 GMT

### Changes

- More Override Manifest Functionality (Unit Tests Coming Next Iteration) (ngerlem@microsoft.com)
## 0.61.0-beta.30
Mon, 02 Mar 2020 21:53:02 GMT

### Changes

- Upgrade just-scripts and add root test script (ngerlem@microsoft.com)
- Add descriptions to dev menu items (acoates@microsoft.com)
## 0.61.0-beta.29
Mon, 02 Mar 2020 20:59:31 GMT

### Changes

- Fix a couple of places in run-windows that has incorrect assumptions on install layout (acoates@microsoft.com)
## 0.61.0-beta.28
Thu, 27 Feb 2020 23:11:13 GMT

### Changes

- Set IsTabStop to false when tabIndex is negative (lamdoan@microsoft.com)
- TextBox should have a default tabIndex of 0 (lamdoan@microsoft.com)
## 0.61.0-beta.27
Wed, 26 Feb 2020 18:33:35 GMT

### Changes

- Fixed call of methods with two callbacks (vmorozov@microsoft.com)
## 0.61.0-beta.26
Wed, 26 Feb 2020 17:21:19 GMT

### Changes

- Standup first TurboModule (acoates@microsoft.com)
## 0.61.0-beta.25
Wed, 26 Feb 2020 10:17:18 GMT

### Changes

- Fix concurrency issue in NetworkingModule (lamdoan@microsoft.com)
## 0.61.0-beta.24
Tue, 25 Feb 2020 21:50:32 GMT

### Changes

- Plumb directdebugging options through debug menu and instance loading (acoates@microsoft.com)
## 0.61.0-beta.23
Tue, 25 Feb 2020 03:42:21 GMT

### Changes

- Declare MessageQueueThread in header that uses it (acoates@microsoft.com)
## 0.61.0-beta.22
Sat, 22 Feb 2020 00:21:17 GMT

### Changes

- Force coroutine method parameters pass by value (vmorozov@microsoft.com)
## 0.61.0-beta.21
Fri, 21 Feb 2020 23:40:21 GMT

### Changes

- Add a Win32 playground app to RNW (hpratt@microsoft.com)
## 0.61.0-beta.20
Fri, 21 Feb 2020 16:46:52 GMT

### Changes

- Fix ReactNativeHost without package providers (vmorozov@microsoft.com)
## 0.61.0-beta.19
Fri, 21 Feb 2020 01:27:13 GMT

### Changes

- Fix some reliability isssues for current AsyncStorage implementation (dida@ntdev.microsoft.com)
- Upgrade ChakraCore Debugger to 0.0.0.43 (julio.rocha@microsoft.com)
- Update metro configs to support node 12 on windows (acoates)
## 0.61.0-beta.17
Thu, 20 Feb 2020 19:20:15 GMT

### Changes

- Bump min. Windows Desktop supported version to 8. (julio.rocha@microsoft.com)
## 0.61.0-beta.16
Thu, 20 Feb 2020 16:09:49 GMT

### Changes

- Avoid having native modules without methods (vmorozov@microsoft.com)
## 0.61.0-beta.15
Thu, 20 Feb 2020 01:52:30 GMT

### Changes

- Use verdaccio to validate react native init (acoates@microsoft.com)
## 0.61.0-beta.14
Wed, 19 Feb 2020 23:34:40 GMT

### Changes

- Implemented NativeModule initialization with IReactContext (vmorozov@microsoft.com)
## 0.61.0-beta.13
Wed, 19 Feb 2020 22:45:15 GMT

### Changes

- Revert "Drop StdCall in favor of Cdecl for x86. (#4115)" (ngerlem@microsoft.com)
## 0.61.0-beta.12
Wed, 19 Feb 2020 07:10:21 GMT

### Changes

- Use Cdecl for x86. (julio.rocha@microsoft.com)
## 0.61.0-beta.11
Wed, 19 Feb 2020 02:24:03 GMT

### Changes

- Remove useIncrementalUpdates assert from Networking module (mcota@microsoft.com)
## 0.61.0-beta.10
Tue, 18 Feb 2020 21:13:20 GMT

### Changes

- Fix issues with relative IntDir breaking builds (aschultz@microsoft.com)
## 0.61.0-beta.9
Sat, 15 Feb 2020 01:46:09 GMT

### Changes

- update devDependencies to use latest TS types (kmelmon@microsoft.com)
## 0.61.0-beta.8
Fri, 14 Feb 2020 21:40:52 GMT

### Changes

- Downngraded OpenSSL to 1.0.2-p.2. (julio.rocha@microsoft.com)
## 0.61.0-beta.7
Thu, 13 Feb 2020 21:30:14 GMT

### Changes

- Force Interpretation of Code as UTF-8 When no BOM Present (ngerlem@microsoft.com)
## 0.61.0-beta.6
Thu, 13 Feb 2020 05:48:00 GMT

### Changes

- Fixed Microsoft.ReactNative compiler warnings and start treating warnings as errors (vmorozov@microsoft.com)
## 0.61.0-beta.5
Thu, 13 Feb 2020 01:37:12 GMT

### Changes

- Set doc generation to output to new path. Update docs to include recent changes. (aschultz@microsoft.com)
## 0.61.0-beta.4
Wed, 12 Feb 2020 21:40:06 GMT

### Changes

- Fixed Developer menu and Live Reaload (vmorozov@microsoft.com)
## 0.61.0-beta.3
Wed, 12 Feb 2020 11:16:52 GMT

### Changes

- Provide a Minimal PlatformConstants Implementation (ngerlem@microsoft.com)
## 0.61.0-beta.2
Wed, 12 Feb 2020 00:46:46 GMT

### Changes

- Fix scroll indicators being shown when not required (jagorrin@microsoft.com)
- Added async code to native module test methods (vmorozov@microsoft.com)
## 0.61.0-beta.1
Tue, 11 Feb 2020 01:21:44 GMT

### Changes

- Fix Dimension returning DIP instead of physical pixels (dida@ntdev.microsoft.com)
- Added Microsoft.ReactNative dependency on Mso (vmorozov@microsoft.com)
- Merge IntegrationTests and RNTester with Stock React Native 60.0.6 (nick@nickgerleman.com)
- Add Flow Check Suppressions (nick@nickgerleman.com)
- Export loadScriptFromString from react-native-win32 (acoates@microsoft.com)
- Export Instance->getModuleRegistry from react-native-win32.dll (acoates@microsoft.com)
- More Patches, IntegrationTests and RNTester Cleanup (nick@nickgerleman.com)
- Use unforked react-native 0.60.6 (nick@nickgerleman.com)
- Implemented ReactHost for Microsoft.ReactNative (vmorozov@microsoft.com)
- Implemented ReactPromise<void> (vmorozov@microsoft.com)
- Enable screen reader announcements for accessibilityLiveRegion (jagorrin@microsoft.com)
- Enabled C++/WinRT heap enforcement (vmorozov@microsoft.com)
- upgrade to 61 (kmelmon@microsoft.com)
## 0.60.0-vnext.133
Thu, 30 Jan 2020 23:29:21 GMT

### Changes

- Allow Any Prerelease Version 0f 0.60 as Peer Dependency (nick@nickgerleman.com)
## 0.60.0-vnext.132
Thu, 30 Jan 2020 22:08:51 GMT

### Changes

- Simple fixes to IReactContext.cpp and add a writer helper function for direct event type constant (dida@ntdev.microsoft.com)
## 0.60.0-vnext.131
Thu, 30 Jan 2020 21:10:31 GMT

### Changes

- Add callstack to the error message when a JS error occurs. (kinhln@microsoft.com)
## 0.60.0-vnext.130
Thu, 30 Jan 2020 09:17:27 GMT

### Changes

- Add as Windows Overrides (nick@nickgerleman.com)
## 0.60.0-vnext.129
Thu, 30 Jan 2020 01:22:53 GMT

### Changes

- Share RCTNetworking Library Between windows and windesktop (nick@nickgerleman.com)
## 0.60.0-vnext.128
Wed, 29 Jan 2020 01:55:04 GMT

### Changes

- Fix Crash When Running CLI Projects (nick@nickgerleman.com)
## 0.60.0-vnext.127
Tue, 28 Jan 2020 18:08:35 GMT

### Changes

- Make the native build oblivous to the differences between microsoft/react-native and facebook/react-native with patches (nick@nickgerleman.com)
## 0.60.0-vnext.126
Tue, 28 Jan 2020 16:46:27 GMT

### Changes

- Add Yoga Patch from microsoft/react-native (nick@nickgerleman.com)
## 0.60.0-vnext.125
Tue, 28 Jan 2020 06:50:47 GMT

### Changes

- copy IntegrationTests from fork (kmelmon@microsoft.com)
## 0.60.0-vnext.124
Sat, 25 Jan 2020 21:45:06 GMT

### Changes

- Removed pch.h from shared items projects (vmorozov@microsoft.com)
## 0.60.0-vnext.123
Fri, 24 Jan 2020 22:33:56 GMT

### Changes

- Fork RNTester script and update 'React' to 'react' (email not defined)
## 0.60.0-vnext.122
Fri, 24 Jan 2020 21:21:47 GMT

### Changes

- Update to react-native@0.60.0-microsoft.40 (acoates@microsoft.com)
## 0.60.0-vnext.121
Thu, 23 Jan 2020 18:42:26 GMT

### Changes

- Logging and Tracing OSS_RN Removal (nick@nickgerleman.com)
## 0.60.0-vnext.120
Thu, 23 Jan 2020 06:39:59 GMT

### Changes

- Update Folly Build Logic (nick@nickgerleman.com)
## 0.60.0-vnext.119
Thu, 23 Jan 2020 00:16:13 GMT

### Changes

- Apply React Native Patches for All Consumers of ReackPackageDirectories (nick@nickgerleman.com)
## 0.60.0-vnext.118
Wed, 22 Jan 2020 07:24:31 GMT

### Changes

- Remove OSS_RN Specialization in unistd.h Stub (nick@nickgerleman.com)
## 0.60.0-vnext.117
Wed, 22 Jan 2020 03:15:49 GMT

### Changes

- Added ARM64 variant to Universal projects. (julio.rocha@microsoft.com)
- Added Mso library required for ReactHost (vmorozov@microsoft.com)
## 0.60.0-vnext.116
Wed, 22 Jan 2020 01:31:33 GMT

### Changes

- VM: Adding support for JSValue for ViewManager APIs (jthysell@microsoft.com)
- Upgrade OpenSSL NuGet to 1.1.1-d.2. (julio.rocha@microsoft.com)
## 0.60.0-vnext.115
Thu, 16 Jan 2020 05:34:40 GMT

### Changes

- Add initial set of React Native Patches (nick@nickgerleman.com)
## 0.60.0-vnext.114
Thu, 16 Jan 2020 03:02:14 GMT

### Changes

- convert haste imports to use relative requires (kmelmon@microsoft.com)
## 0.60.0-vnext.113
Wed, 15 Jan 2020 21:22:45 GMT

### Changes

- Upgrade to OpenSSL 1.1.1d/0. (julio.rocha@microsoft.com)
## 0.60.0-vnext.112
Thu, 09 Jan 2020 23:20:10 GMT

### Changes

- Header Refactoring Around jsireact to Unblock ms/rn (nick@nickgerleman.com)
## 0.60.0-vnext.111
Thu, 09 Jan 2020 22:06:05 GMT

### Changes

- Add GetFacebookReactInstance :: uwpInstance -> react::Instance (zihanc@microsoft.com)
- Added unit tests for Events with JSValue args (jthysell@microsoft.com)
## 0.60.0-vnext.110
Wed, 08 Jan 2020 23:03:25 GMT

### Changes

- Image - basic svg support (mcota@microsoft.com)
## 0.60.0-vnext.109
Wed, 08 Jan 2020 01:43:57 GMT

### Changes

- Fixing bugs when using .NetNative (jthysell@microsoft.com)
## 0.60.0-vnext.108
Tue, 07 Jan 2020 19:44:17 GMT

### Changes

- Fix RNTester bundle (#2728) (acoates@microsoft.com)
## 0.60.0-vnext.107
Mon, 06 Jan 2020 21:29:21 GMT

### Changes

- Call StartAnimatiom on m_scaleCombined for ScaleX / ScaleY animations (thshea@microsoft.com)
## 0.60.0-vnext.106
Mon, 30 Dec 2019 22:14:37 GMT

### Changes

- Avoid app crash because content is not bundled for the first time (licanhua@live.com)
## 0.60.0-vnext.105
Fri, 27 Dec 2019 17:01:20 GMT

### Changes

- Changed Microsoft.ReactNative to be independent from ReactUWP project (vmorozov@microsoft.com)
## 0.60.0-vnext.104
Sat, 21 Dec 2019 03:41:33 GMT

### Changes

- Export ability to query native module names (ngerlem@microsoft.com)
## 0.60.0-vnext.103
Sat, 21 Dec 2019 00:18:22 GMT

### Changes

- Fix RNTester bundle (#2728) (acoates@microsoft.com)
## 0.60.0-vnext.102
Fri, 20 Dec 2019 19:01:50 GMT

### Changes

- Provide source uri in SourceCode module when using livereload (acoates@microsoft.com)
## 0.60.0-vnext.101
Fri, 20 Dec 2019 18:19:02 GMT

### Changes

- Get flow clean, and turn on flow check during build (acoates@microsoft.com)
## 0.60.0-vnext.100
Fri, 20 Dec 2019 05:18:08 GMT

### Changes

- Fix cornerRadius issue for TextInput and some updates to TreeDump (dida@ntdev.microsoft.com)
## 0.60.0-vnext.99
Thu, 19 Dec 2019 18:44:51 GMT

### Changes

- Removed Bridge sub-namespace in favor of Microsoft.ReactNative (vmorozov@microsoft.com)
## 0.60.0-vnext.98
Wed, 18 Dec 2019 21:47:15 GMT

### Changes

- port windowsbrush changes from Microsoft fork of react-native (kmelmon@microsoft.com)
## 0.60.0-vnext.97
Wed, 18 Dec 2019 01:40:40 GMT

### Changes

- Conditionally use BitmapImage (email not defined)
## 0.60.0-vnext.96
Mon, 16 Dec 2019 21:22:31 GMT

### Changes

- TreeDump for E2E test and fix for image border issue (dida@ntdev.microsoft.com)
## 0.60.0-vnext.95
Fri, 13 Dec 2019 17:32:38 GMT

### Changes

- Fix toggle debugger setting issue with ReactApplication (dida@ntdev.microsoft.com)
## 0.60.0-vnext.94
Thu, 12 Dec 2019 22:18:13 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.31 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.93
Thu, 12 Dec 2019 00:17:06 GMT

### Changes

- Strongly typed value serialization and deserialization using IJSValueReader, JSValue, and IJSValueWriter (vmorozov@microsoft.com)
## 0.60.0-vnext.92
Wed, 11 Dec 2019 23:31:47 GMT

### Changes

- Submitting the ETW schema resouce dll and the register script (anandrag@microsoft.com)
## 0.60.0-vnext.91
Mon, 09 Dec 2019 18:40:45 GMT

### Changes

- Aligning Microsoft.Windows.CppWinRT Versions (jthysell@microsoft.com)
## 0.60.0-vnext.90
Fri, 06 Dec 2019 21:56:22 GMT

### Changes

- Update E2ETest to use ReactApplication (dida@ntdev.microsoft.com)
## 0.60.0-vnext.89
Fri, 06 Dec 2019 19:14:55 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.28 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.88
Fri, 06 Dec 2019 17:14:06 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.26 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.87
Thu, 05 Dec 2019 19:54:18 GMT

### Changes

- Add support for custom ViewManagers with children (jthysell@microsoft.com)
## 0.60.0-vnext.86
Tue, 03 Dec 2019 23:21:34 GMT

### Changes

- Add WinUI (licanhua@live.com)
## 0.60.0-vnext.85
Mon, 02 Dec 2019 17:34:30 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.24 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.84
Tue, 26 Nov 2019 00:13:20 GMT

### Changes

- View Manager Event Support (jthysell@microsoft.com)
## 0.60.0-vnext.83
Mon, 25 Nov 2019 21:54:38 GMT

### Changes

- Properly support ScrollView KeyboardDismissMode (dida@ntdev.microsoft.com)
## 0.60.0-vnext.82
Mon, 25 Nov 2019 21:23:52 GMT

### Changes

- Revert "Add Winui 2.2 framework package (#3622)" (licanhua@live.com)
## 0.60.0-vnext.81
Sat, 23 Nov 2019 08:02:20 GMT

### Changes

- Add WinUI (licanhua@live.com)
- Support ScrollView pagingEnabled (dida@ntdev.microsoft.com)
- Add react-native run-windows --info (licanhua@live.com)
- manually bump to vnext.80 (kmelmon@microsoft.com)
## 0.60.0-vnext.79
Fri, 22 Nov 2019 20:45:17 GMT

### Changes

- Commit ecd4456c... was wrong. `ABI::Windows::ApplicationModel::IPackage::get_Current` will, when called without package identity, return `HRESULT_FROM_WIN32(APPMODEL_ERROR_NO_PACKAGE)`, not just `APPMODEL_ERROR_NO_PACKAGE` (which is a win32 error code, not an hresult). 🤦‍♂️ (hpratt@microsoft.com)
## 0.60.0-vnext.78
Fri, 22 Nov 2019 04:47:41 GMT

### Changes

- remove ReleaseBundle and DebugBundle (licanhua@live.com)
## 0.60.0-vnext.77
Fri, 22 Nov 2019 00:48:20 GMT

### Changes

- Support keyboardDismissMode on-drag for ScrollView (dida@ntdev.microsoft.com)
## 0.60.0-vnext.76
Thu, 21 Nov 2019 21:53:08 GMT

### Changes

- Merge remote-tracking branch 'upstream/master' (hpratt@microsoft.com)
## 0.60.0-vnext.75
Thu, 21 Nov 2019 01:46:31 GMT

### Changes

- Fix layout children position issue when outer border exists (dida@ntdev.microsoft.com)
- Updating react-native to version: 0.60.0-microsoft.23 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.74
Wed, 20 Nov 2019 18:56:27 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.22 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.73
Tue, 19 Nov 2019 22:36:29 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.21 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.72
Mon, 18 Nov 2019 22:41:41 GMT

### Changes

- Stop requiring Platform.cpp from MS/RN (acoates@microsoft.com)
## 0.60.0-vnext.71
Fri, 15 Nov 2019 18:17:47 GMT

### Changes

- Add TurboModule and JsCallInvoker files to ReactCommon only when targeting to microsoft/react-native (zihanc@microsoft.com)
## 0.60.0-vnext.70
Tue, 12 Nov 2019 04:50:06 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.20 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.69
Mon, 11 Nov 2019 22:01:18 GMT

### Changes

- Workaround for flyout centering problem (kenander@microsoft.com)
## 0.60.0-vnext.68
Sat, 09 Nov 2019 01:27:15 GMT

### Changes

- Native Module Setup Guide * Added NativeModuleSetup.md, #3623 * Added link to new guide in NativeModules.md, ViewManagers.md * Updated ProjectStructure.md with Microsoft.ReactNative projects * Removed PropertySheets in SampleApps and CLI solutions (jthysell@microsoft.com)
## 0.60.0-vnext.67
Fri, 08 Nov 2019 00:30:50 GMT

### Changes

- ViewManager Command Updates * Added command to CustomUserControlViewManagerCPP, #3600 * Added support for simpler C# view manager commands signatures * Added support for object properties for view managers, #3613 * Added documentation for C++ view manager * Added documentation for ViewManager commands, #3599 (jthysell@microsoft.com)
## 0.60.0-vnext.66
Thu, 07 Nov 2019 17:37:11 GMT

### Changes

- Support refreshControl (dida@ntdev.microsoft.com)
## 0.60.0-vnext.65
Wed, 06 Nov 2019 13:58:14 GMT

### Changes

- Merging upstream changes (anandrag@microsoft.com)
## 0.60.0-vnext.64
Wed, 06 Nov 2019 06:59:22 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.16 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.63
Wed, 06 Nov 2019 01:56:47 GMT

### Changes

- Native Modules: Updated samples and docs to show how to use events (jthysell@microsoft.com)
## 0.60.0-vnext.62
Tue, 05 Nov 2019 23:11:52 GMT

### Changes

- VM: Split up IViewManager into optional interfaces #3565 (jthysell@microsoft.com)
## 0.60.0-vnext.61
Fri, 01 Nov 2019 23:40:12 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.15 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.60
Fri, 01 Nov 2019 21:25:27 GMT

### Changes

- Support snapToStart and snapToEnd for ScrollView (dida@ntdev.microsoft.com)
## 0.60.0-vnext.59
Fri, 01 Nov 2019 18:09:07 GMT

### Changes

- Call native animation callbacks only once (asklar@microsoft.com)
## 0.60.0-vnext.58
Thu, 31 Oct 2019 18:11:20 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.14 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.57
Thu, 31 Oct 2019 16:59:45 GMT

### Changes

- Add todo stub for FacadeType::Progress in AnimatedNode (adamgor@microsoft.com)
## 0.60.0-vnext.56
Wed, 30 Oct 2019 22:56:11 GMT

### Changes

- Set a default 3D perspective (asklar@microsoft.com)
## 0.60.0-vnext.55
Wed, 30 Oct 2019 17:12:12 GMT

### Changes

- fix build error (enm10k@gmail.com)
## 0.60.0-vnext.54
Tue, 29 Oct 2019 23:29:07 GMT

### Changes

- Fix crash in RS5 due to missing facade property (asklar@winse.microsoft.com)
## 0.60.0-vnext.53
Tue, 29 Oct 2019 19:36:34 GMT

### Changes

- Support Keyboard events (dida@ntdev.microsoft.com)
## 0.60.0-vnext.52
Tue, 29 Oct 2019 01:31:27 GMT

### Changes

- Update the CLI templates to use Microsoft.ReactNative (jthysell@microsoft.com)
## 0.60.0-vnext.51
Mon, 28 Oct 2019 23:51:07 GMT

### Changes

- Move closer to allowing JSI with OSS RN (acoates@microsoft.com)
## 0.60.0-vnext.50
Sat, 26 Oct 2019 21:50:14 GMT

### Changes

- Fix remote debugger for Playground app. Now you can pass  the hostname:port of a running metro bundler instance and it will connect correctly (asklar@winse.microsoft.com)
## 0.60.0-vnext.49
Sat, 26 Oct 2019 02:55:31 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.13 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.48
Fri, 25 Oct 2019 23:07:25 GMT

### Changes

- Custom View Managers E2E (jthysell@microsoft.com)
## 0.60.0-vnext.47
Fri, 25 Oct 2019 21:43:48 GMT

### Changes

- go back to native implementation for getPropertyNames (stecrain@microsoft.com)
## 0.60.0-vnext.46
Fri, 25 Oct 2019 20:56:10 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.12 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.45
Fri, 25 Oct 2019 01:44:45 GMT

### Changes

- make run-windows work on x86 machines (asklar@winse.microsoft.com)
## 0.60.0-vnext.44
Thu, 24 Oct 2019 18:04:40 GMT

### Changes

- fix assert/missing else when setting alignSelf to null (adamgor@microsoft.com)
## 0.60.0-vnext.43
Thu, 24 Oct 2019 09:06:03 GMT

### Changes

- Register 2x NYI stubs for NativeAnimatedModule (adamgor@microsoft.com)
## 0.60.0-vnext.42
Wed, 23 Oct 2019 20:38:22 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.11 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.41
Tue, 22 Oct 2019 03:28:58 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.9 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.40
Tue, 22 Oct 2019 01:01:42 GMT

### Changes

- Fix build issue with react-native run-windows (kmelmon@microsoft.com)
## 0.60.0-vnext.39
Fri, 18 Oct 2019 21:22:01 GMT

### Changes

- Fix run-windows app installation hang (dida@ntdev.microsoft.com)
## 0.60.0-vnext.38
Fri, 18 Oct 2019 20:36:35 GMT

### Changes

- add developer menu property (kmelmon@microsoft.com)
## 0.60.0-vnext.37
Thu, 17 Oct 2019 23:10:13 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.8 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.36
Thu, 17 Oct 2019 20:58:22 GMT

### Changes

- Fix two issues: 1) you cannot animated 2 subchannels of the same property with different animations. to fix this we animated yet another property set for translation and scale owned by the props nodes and use one animation to animate all of the subchannels for the uiElement. 2) Reference parameter names which started with a multi digit number are unsupported so i added an n to the start of each name, which was previously just the node's tag. (stpete@microsoft.com)
## 0.60.0-vnext.35
Thu, 17 Oct 2019 16:18:52 GMT

### Changes

- Support autoCapitalize Characters (dida@ntdev.microsoft.com)
## 0.60.0-vnext.34
Thu, 17 Oct 2019 02:48:28 GMT

### Changes

- Support onSubmitEditing event (dida@ntdev.microsoft.com)
## 0.60.0-vnext.33
Thu, 17 Oct 2019 00:14:32 GMT

### Changes

- exclude Generated Files (kmelmon@microsoft.com)
## 0.60.0-vnext.32
Wed, 16 Oct 2019 02:19:39 GMT

### Changes

- Extend flyout shadow fix to Popups (kenander@microsoft.com)
## 0.60.0-vnext.31
Tue, 15 Oct 2019 23:45:52 GMT

### Changes

- Reduced and simplified sample native modules (jthysell@microsoft.com)
## 0.60.0-vnext.30
Tue, 15 Oct 2019 22:35:30 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.7 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.29
Tue, 15 Oct 2019 13:17:22 GMT

### Changes

- dont new up exceptions (adamgor@microsoft.com)
## 0.60.0-vnext.28
Mon, 14 Oct 2019 23:02:23 GMT

### Changes

- Microsoft.ReactNative attribute-based NativeModules (jthysell@microsoft.com)
## 0.60.0-vnext.27
Sat, 12 Oct 2019 16:49:47 GMT

### Changes

- add tests (kmelmon@microsoft.com)
## 0.60.0-vnext.26
Fri, 11 Oct 2019 22:15:37 GMT

### Changes

- add Common.vcxproj to CLI solutions (kmelmon@microsoft.com)
## 0.60.0-vnext.25
Fri, 11 Oct 2019 20:47:08 GMT

### Changes

- Support caretHidden (dida@ntdev.microsoft.com)
## 0.60.0-vnext.24
Fri, 11 Oct 2019 10:21:12 GMT

### Changes

- add support for offline bundles (kmelmon@microsoft.com)
## 0.60.0-vnext.23
Wed, 09 Oct 2019 18:17:42 GMT

### Changes

- Add support for forever animations. (stpete@microsoft.com)
## 0.60.0-vnext.22
Wed, 09 Oct 2019 15:13:21 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.5 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.21
Tue, 08 Oct 2019 18:33:16 GMT

### Changes

- re-enable Chakra Core on UWP (stecrain@microsoft.com)
## 0.60.0-vnext.20
Mon, 07 Oct 2019 21:46:39 GMT

### Changes

- Bump V8 and Hermes JSI package versions (tudorm@microsoft.com)
## 0.60.0-vnext.19
Fri, 04 Oct 2019 19:04:20 GMT

### Changes

- Update ChakraCore to v1.11.13 (acoates@microsoft.com)
## 0.60.0-vnext.18
Fri, 04 Oct 2019 16:32:45 GMT

### Changes

- Update to react-native@0.60.0-microsoft.3 (acoates@microsoft.com)
## 0.60.0-vnext.17
Thu, 03 Oct 2019 22:17:13 GMT

### Changes

- change file (randyfl@microsoft.com)
- Support contextMenuHidden (dida@ntdev.microsoft.com)
- Remove react-native as a haste root, as all the files should be in react-native-windows (acoates@microsoft.com)
- Connect DeviceInfo dimensions with root element (dida@ntdev.microsoft.com)
## 0.60.0-vnext.16
Tue, 01 Oct 2019 18:17:42 GMT

### Changes

- Turn ControlFlowGuard on by default (acoates@microsoft.com)
- The customer provided examples revealed a few of issues.  One, we were dubble counding the starting value of animatiosn in some cases. Two we were incorrectly relying on JS to tell us to flatten our animated values. Three we were detaching the expression animations that tied the property to the UIElement and in certain cases the JS thread does not inform us that we need to rebuild this cuppling.  There is an open issue with this final part #3280 (stpete@microsoft.com)
## 0.60.0-vnext.11
Mon, 30 Sep 2019 18:28:23 GMT

### Changes

- Update to folly 2019.09.30.00 (acoates@microsoft.com)
## 0.60.0-vnext.10
Mon, 30 Sep 2019 16:21:08 GMT

### Changes

- Bump up to latest published version. (#3275) (julio.rocha@microsoft.com)
## 0.60.0-vnext.9
Sat, 28 Sep 2019 01:21:33 GMT

### Changes

- add support for cpp template app (email not defined)
## 0.60.0-vnext.8
Sat, 28 Sep 2019 00:21:32 GMT

### Changes

- Fix build break when using latest Visual Studio compiler versions (acoates@microsoft.com)
## 0.60.0-vnext.7
Fri, 27 Sep 2019 17:40:33 GMT

### Changes

- Allow focus transfer to Flyout on Open (kenander@microsoft.com)
## 0.60.0-vnext.6
Fri, 27 Sep 2019 17:34:59 GMT

### Changes

- change file (randyfl@microsoft.com)
## 0.60.0-vnext.5
Fri, 27 Sep 2019 00:08:28 GMT

### Changes

- Allow focus transfer to Flyout on Open (kenander@microsoft.com)
## 0.60.0-vnext.4
Wed, 25 Sep 2019 18:32:05 GMT

### Changes

- Enabling Systrace/ETW plumbing by default. The tracing will be controlled by whether the provider is enabled or not (anandrag@microsoft.com)
## 0.60.0-vnext.3
Tue, 24 Sep 2019 23:29:18 GMT

### Changes

- Create stub StatusBarManager to avoid crash when using <StatusBar> (acoates@microsoft.com)
## 0.60.0-vnext.2
Tue, 24 Sep 2019 19:11:20 GMT

### Changes

- Update to react-native 0.60 (acoates@microsoft.com)
## 0.59.0-vnext.190
Thu, 19 Sep 2019 17:02:46 GMT

### Changes

- Support keyboardType for TextInput (dida@ntdev.microsoft.com)
## 0.59.0-vnext.189
Wed, 18 Sep 2019 18:18:37 GMT

### Changes

- Add the SpringAnimationDriver (stpete@microsoft.com)
## 0.59.0-vnext.188
Tue, 17 Sep 2019 17:42:42 GMT

### Changes

- Refined logic for handling Flyout shadows (kenander@microsoft.com)
## 0.59.0-vnext.187
Tue, 17 Sep 2019 11:32:39 GMT

### Changes

- Simplify NuGet packing YAML. (julio@rochsquadron.net)
## 0.59.0-vnext.186
Tue, 17 Sep 2019 05:17:28 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.82 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.185
Sat, 14 Sep 2019 14:09:03 GMT

### Changes

- Add PackageCertificatePassword attribute to MyApp.csproj to solve the build issue in VS2019 due to temp pfx certificate issue (dida@ntdev.microsoft.com)
## 0.59.0-vnext.184
Fri, 13 Sep 2019 21:56:22 GMT

### Changes

- Support live reload for developer menu (dida@ntdev.microsoft.com)
## 0.59.0-vnext.183
Fri, 13 Sep 2019 15:59:25 GMT

### Changes

- MultiLine TextInput should accepts Enter key (dida@ntdev.microsoft.com)
## 0.59.0-vnext.182
Thu, 12 Sep 2019 23:51:35 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.79 (53619745+rnbot@users.noreply.github.com)
- Updating react-native to version: 0.59.0-microsoft.80 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.179
Wed, 11 Sep 2019 18:44:17 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.77 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.178
Tue, 10 Sep 2019 23:25:37 GMT

### Changes

- Fix onPress not firing for Touchable* keyboard events (thshea@microsoft.com)
## 0.59.0-vnext.177
Tue, 10 Sep 2019 19:16:08 GMT

### Changes

- Fix Login E2E test failure (dida@ntdev.microsoft.com)
## 0.59.0-vnext.176
Tue, 10 Sep 2019 00:44:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.74 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.175
Mon, 09 Sep 2019 21:39:46 GMT

### Changes

- Alphabetize def files. (yicyao@microsoft.com)
## 0.59.0-vnext.174
Mon, 09 Sep 2019 19:22:09 GMT

### Changes

- Put desktop unit tests into namespaces. (yicyao@microsoft.com)
## 0.59.0-vnext.173
Sat, 07 Sep 2019 20:17:19 GMT

### Changes

- Support dynamic swapping TextBox/PasswordBox view for SecureTextEntry property (dida@ntdev.microsoft.com)
## 0.59.0-vnext.172
Fri, 06 Sep 2019 18:03:18 GMT

### Changes

- Fix issue with using getInt instead of asDouble for JSI (jthysell@microsoft.com)
## 0.59.0-vnext.171
Thu, 05 Sep 2019 17:37:33 GMT

### Changes

- ReactUWP def cleanup and unit test fixes (jthysell@microsoft.com)
## 0.59.0-vnext.170
Tue, 03 Sep 2019 21:49:49 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.73 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.169
Tue, 03 Sep 2019 20:24:37 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.72 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.168
Fri, 30 Aug 2019 23:08:02 GMT

### Changes

- set IsTabStop = false for SnapPointManagingContentControl (stecrain@microsoft.com)
## 0.59.0-vnext.167
Fri, 30 Aug 2019 20:30:25 GMT

### Changes

- fix double-apply TransformMatrix bug (email not defined)
## 0.59.0-vnext.166
Fri, 30 Aug 2019 20:17:20 GMT

### Changes

- changed alert export (email not defined)
## 0.59.0-vnext.165
Fri, 30 Aug 2019 19:46:18 GMT

### Changes

- Fixed sampleapps build and updated CI (jthysell@microsoft.com)
## 0.59.0-vnext.164
Thu, 29 Aug 2019 22:14:06 GMT

### Changes

- changed alert export (email not defined)
## 0.59.0-vnext.163
Thu, 29 Aug 2019 20:21:10 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.71 (53619745+rnbot@users.noreply.github.com)
- change file (randyfl@microsoft.com)
## 0.59.0-vnext.162
Thu, 29 Aug 2019 19:38:17 GMT

### Changes

- Allow windowsbrush: color syntax to use non-SolidColor brushes (thshea@microsoft.com)
## 0.59.0-vnext.161
Thu, 29 Aug 2019 01:41:01 GMT

### Changes

- Use PreparedScriptStore for V8 in win32 (tudorm@microsoft.com)
## 0.59.0-vnext.160
Thu, 29 Aug 2019 01:33:54 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.70 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.159
Wed, 28 Aug 2019 19:55:48 GMT

### Changes

- Added React.Windows.Desktop.Test project. (julio@rochsquadron.net)
- Add isOverlayEnabled (cknestri@microsoft.com)
## 0.59.0-vnext.158
Wed, 28 Aug 2019 18:34:51 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.69 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.157
Wed, 28 Aug 2019 02:44:14 GMT

### Changes

- Let View process color objects (email not defined)
## 0.59.0-vnext.156
Tue, 27 Aug 2019 04:41:01 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.68 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.155
Mon, 26 Aug 2019 21:10:00 GMT

### Changes

- Enable experimental V8 JSI Executor support (off by default) (tudorm@microsoft.com)
## 0.59.0-vnext.154
Sat, 24 Aug 2019 15:32:08 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.67 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.153
Sat, 24 Aug 2019 04:24:35 GMT

### Changes

- Fix change log formatting (acoates@microsoft.com)
## 0.59.0-vnext.152
Sat, 24 Aug 2019 04:09:20 GMT

### Changes

- move cppwinrt less exception helper, use in DevSupportManager (andrewh@microsoft.com)
## 0.59.0-vnext.151
Fri, 23 Aug 2019 01:27:19 GMT

### Changes

- turn off RTTI (email not defined)

## 0.59.0-vnext.150
Thu, 22 Aug 2019 21:17:45 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.49 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.149
Thu, 22 Aug 2019 15:43:48 GMT

### Changes

- Improve Networking error reporting (andrewh@microsoft.com)

## 0.59.0-vnext.148
Wed, 21 Aug 2019 20:44:12 GMT

### Changes

- Only send focus/blur from OriginalSource, remove Click event for enter/space (andrewh@microsoft.com)

## 0.59.0-vnext.147
Wed, 21 Aug 2019 06:12:54 GMT

### Changes

- add default message queue that uses CoreDispatcher (email not defined)

## 0.59.0-vnext.146
Wed, 21 Aug 2019 00:42:37 GMT

### Changes

- revert fix, not quite working (email not defined)

## 0.59.0-vnext.145
Tue, 20 Aug 2019 23:40:14 GMT

### Changes

- Light up Inspector debugging tool. (kinhln@microsoft.com)
- Remove old playground app (acoates@microsoft.com)

## 0.59.0-vnext.144
Tue, 20 Aug 2019 21:54:07 GMT

### Changes

- Minor fixes made in RN60 update that can be brought right to master (acoates@microsoft.com)

## 0.59.0-vnext.143
Tue, 20 Aug 2019 07:26:49 GMT

### Changes

- Don't use DefaultNativeMessageQueueThread in live reload (andrewh@microsoft.com)

## 0.59.0-vnext.142
Tue, 20 Aug 2019 03:02:53 GMT

### Changes

- add additional new warning disable to MsoFolly.h (andrewh@microsoft.com)

## 0.59.0-vnext.141
Mon, 19 Aug 2019 20:59:57 GMT

### Changes

- fix flyout full placement mode (kmelmon@microsoft.com)

## 0.59.0-vnext.140
Mon, 19 Aug 2019 16:43:43 GMT

### Changes

- Update placement mapping (cknestri@microsoft.com)
- Start API documentation (acoates@microsoft.com)
- Always use unforked version of folly + Update folly to v2019.08.12.00 (acoates@microsoft.com)
- resolved build error (email not defined)
- Improved fix for overlapping XAML theme shadows on Flyouts (kenander@microsoft.com)
- add batching queue (kmelmon@microsoft.com)
- restore accidental deletion (kmelmon@microsoft.com)
- Microsoft.ReactNative SampleApps (jthysell@microsoft.com)

## 0.59.0-vnext.139
Tue, 13 Aug 2019 19:56:52 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.43 (53619745+rnbot@users.noreply.github.com)
- Change windows template to not use cli wrapper (acoates@microsoft.com)

## 0.59.0-vnext.138
Mon, 12 Aug 2019 23:13:59 GMT

### Changes

- Rename platform uwp->windows (acoates@microsoft.com)

## 0.59.0-vnext.137
Mon, 12 Aug 2019 21:06:24 GMT

### Changes

- Improve output of run-windows command (acoates@microsoft.com)

## 0.59.0-vnext.136
Mon, 12 Aug 2019 20:02:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.42 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.135
Sun, 11 Aug 2019 04:57:41 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.41 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.134
Sat, 10 Aug 2019 19:21:55 GMT

### Changes

- Fix switch shimmy issue (ruaraki@microsoft.com)

## 0.59.0-vnext.133
Fri, 09 Aug 2019 22:37:48 GMT

### Changes

- use more specific typing for currentTheme, improve examples (andrewh@microsoft.com)

## 0.59.0-vnext.132
Fri, 09 Aug 2019 19:20:03 GMT

### Changes

- Update UwpPreparedScriptStore to use shared_ptrs (jthysell@microsoft.com)

## 0.59.0-vnext.131
Fri, 09 Aug 2019 06:07:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.40 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.130
Fri, 09 Aug 2019 04:18:45 GMT

### Changes

- Added list, listitem roles to ViewAccessibility.uwp.js (jthysell@microsoft.com)

## 0.59.0-vnext.129
Fri, 09 Aug 2019 03:27:03 GMT

### Changes

- Fix Sample/index.tsx layout, set Tag as Name in debug (andrewh@microsoft.com)

## 0.59.0-vnext.128
Thu, 08 Aug 2019 23:35:13 GMT

### Changes

- fix hit-testing bug (kmelmon@microsoft.com)

## 0.59.0-vnext.127
Thu, 08 Aug 2019 22:19:50 GMT

### Changes

- Add textDecoration support to Nested Text (dida@ntdev.microsoft.com)

## 0.59.0-vnext.126
Thu, 08 Aug 2019 20:57:44 GMT

### Changes

- Moving picker fix (decrowle@microsoft.com)

## 0.59.0-vnext.125
Wed, 07 Aug 2019 17:07:35 GMT

### Changes

- Adding accessibilityPosInSet and accessibilitySizeOfSet props to ViewWindows, #2894 (jthysell@microsoft.com)

## 0.59.0-vnext.124
Wed, 07 Aug 2019 16:00:25 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.38 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.123
Tue, 06 Aug 2019 21:09:10 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.37 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.122
Tue, 06 Aug 2019 18:30:23 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.36 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.121
Tue, 06 Aug 2019 15:46:10 GMT

### Changes

- Modify how we setup build directories to better support monorepos (acoates@microsoft.com)

## 0.59.0-vnext.120
Tue, 06 Aug 2019 00:59:54 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.35 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.119
Mon, 05 Aug 2019 17:37:47 GMT

### Changes

- Put back a couple of win32 dll exports (acoates@microsoft.com)
- Updating react-native to version: 0.59.0-microsoft.34 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.118
Mon, 05 Aug 2019 06:09:37 GMT

### Changes

- Include additional jsi headers in nuget (acoates@microsoft.com)

## 0.59.0-vnext.117
Mon, 05 Aug 2019 00:09:47 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.33 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.116
Sat, 03 Aug 2019 17:26:50 GMT

### Changes

- change dev menu shortcut to ctrl+shift+D (kmelmon@microsoft.com)

## 0.59.0-vnext.115
Sat, 03 Aug 2019 04:30:38 GMT

### Changes

- change process.env.TEMP to os.tmpdir() (email not defined)
- fix onFocus/onBlur events for Touchables (andrewh@microsoft.com)
- New switch props (thumbColor and trackColor), and fix few bugs found in TextInputViewManager (dida@ntdev.microsoft.com)
- addedundefined developer menu (kmelmon@microsoft.com)
- fix image not firing on load. (stecrain@microsoft.com)
- Delay-load dependencies absent on Win7 (acoates@microsoft.com)
- Add the SpringAnimationDriver (stpete@microsoft.com)

## 0.59.0-vnext.110
Sun, 28 Jul 2019 12:44:00 GMT

### Changes

- Allow run-windows to be more flexible about project layout to support default c++ layouts (acoates@microsoft.com)

## 0.59.0-vnext.109
Fri, 26 Jul 2019 08:32:04 GMT

### Changes

- add null checks to fix crashes with reload / shutdown with pending text updates (andrewh@microsoft.com)

## 0.59.0-vnext.108
Fri, 26 Jul 2019 07:23:20 GMT

### Changes

- Fix for flyouts unexpectedly dismissing when opening picker (ruaraki@microsoft.com)

## 0.59.0-vnext.107
Fri, 26 Jul 2019 05:43:01 GMT

### Changes

- use win32 file api to avoid runtime broker slowing us down (stecrain@microsoft.com)

## 0.59.0-vnext.106
Thu, 25 Jul 2019 00:29:50 GMT

### Changes

- Fixes to ReactImage lifetime with downloading Images (acoates@microsoft.com)

## 0.59.0-vnext.105
Wed, 24 Jul 2019 21:03:23 GMT

### Changes

- Hookup beachball for publishing (acoates@microsoft.com)
