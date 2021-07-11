# @react-native-windows/cli

CLI to build and run React Native for  Windows apps.

## Usage

Run this from an existing `react-native` project with React Native Windows to build and deploy apps.


Example usage
```
npx react-native run-windows
```

Options:
| option                | description                          | type                                             |
|-----------------------|--------------------------------------|--------------------------------------------------|
| `--release`           | Specifies a Release build | [boolean] |
| `--root`              | Override the root directory for the windows build which contains the windows folder. (default: "E:\\test63") | [string] |
| `--arch`              | The build architecture (ARM64, x86, x64). default: x86 | [string] |
| `--singleproc`        | Disables multi-proc build | [boolean] |
| `--emulator`          | Deploys the app to an emulator | [boolean] |
| `--device`            | Deploys the app to a connected device | [boolean] |
| `--target`            | Deploys the app to the specified GUID for a device | [string] |
| `--remote-debugging`  | Deploys the app in remote debugging mode. | [boolean] |
| `--logging`           | Enables logging | [boolean] | 
| `--no-packager`       | Do not launch packager while building | [boolean] |
| `--bundle`            | Enable Bundle configuration and it would be ReleaseBundle/DebugBundle other than Release/Debug | [boolean] | 
| `--no-launch`         | Do not launch the app after deployment | [boolean] | 
| `--no-autolink`       | Do not run autolinking | [boolean] | 
| `--no-build`          | Do not build the solution | [boolean] |
| `--no-deploy`         | Do not deploy the app | [boolean] |
| `--deploy-from-layout`| Force deploy from layout, even in release builds | [boolean] |
| `--sln`               | Override the app solution file determined by 'react-native config', e.g. windows\myApp.sln | [string] | 
| `--proj`              | Override the app project file determined by 'react-native config', e.g. windows\myApp\myApp.vcxproj | [string] |
| `--msbuildprops`      | Comma separated props to pass to MSBuild, eg: prop1=value1,prop2=value2 | [string] |
| `--buildLogDirectory` | Optional directory where MSBuild log files should be stored | [string] |
| `--info`              | Dump environment information | [boolean] |
| `--direct-debugging`  | Enable direct debugging on specified port | [number] |
| `--useWinUI3`         | Targets WinUI 3 (through the [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)) instead of UWP XAML. | [boolean] |
| `--useHermes`         | Use Hermes instead of Chakra as the JS engine (supported on 0.64+) | [boolean] |
| `--no-telemetry`      | Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI | [boolean] |
| `-h`, `--help`        | output usage information | [boolean] |


## Data Collection
The software may collect information about you and your use of the software and send it to Microsoft. Microsoft may use this information to provide services and improve our products and services. You may turn off the telemetry as described in the repository. There are also some features in the software that may enable you and Microsoft to collect data from users of your applications. If you use these features, you must comply with applicable law, including providing appropriate notices to users of your applications together with a copy of Microsoft's privacy statement. Our privacy statement is located at https://go.microsoft.com/fwlink/?LinkID=824704. You can learn more about data collection and use in the help documentation and our privacy statement. Your use of the software operates as your consent to these practices.

This data collection notice only applies to the process of running the react-native-windows CLI (run-windows and related tools like autolinking).