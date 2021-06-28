// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests
{
    public abstract class AnalysisTestBase
    {
        public TestContext TestContext { get; set; }

        protected CancellationTokenSource TokenSource;

        public AnalysisTestBase()
        {
            TokenSource = new CancellationTokenSource();
        }

        protected (CodeAnalyzer, INamedTypeSymbol) AnalyzeModuleFile(string csSnippet)
        {
            var csCode = @"
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.ReactNative.Managed;

[ReactModule]
public class TestClass
{
" + csSnippet + @"
}
";

            return AnalyzeFileAsync(csCode).GetAwaiter().GetResult();
        }

        protected (CodeAnalyzer, INamedTypeSymbol) AnalyzeFile(string csCode)
        {
            return AnalyzeFileAsync(csCode).GetAwaiter().GetResult();
        }

        protected async Task<(CodeAnalyzer, INamedTypeSymbol)> AnalyzeFileAsync(string csCode)
        {
            var codeAnalyzer = new CodeAnalyzer(TokenSource.Token);

            codeAnalyzer.AddSyntaxTree(
              CSharpSyntaxTree.ParseText(csCode, path: "test.cs", options: new CSharpParseOptions()));

            var targetFolderWithConfiguration = Path.GetDirectoryName(TestContext.TestDeploymentDir);
            var configuration = Path.GetFileName(targetFolderWithConfiguration);
            var targetRoot = Path.GetDirectoryName(Path.GetDirectoryName(targetFolderWithConfiguration));

            var references = new List<string>();

            var uapVersion = "uap10.0.15138";
            var uwpPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86),
                "Microsoft SDKs", "UWPNuGetPackages", "microsoft.netcore.universalwindowsplatform");

            var versions = Directory.GetDirectories(uwpPath).Select(x => new Version(Path.GetFileName(x)));
            Assert.IsTrue(versions.Count() > 0);
            var microsoftNetCoreUwpPkgVersion = versions.Max();
            Assert.IsTrue(microsoftNetCoreUwpPkgVersion >= new Version("6.2.10"));
            var microsoftNetCoreUwpPkgFolder = Path.Combine(uwpPath, microsoftNetCoreUwpPkgVersion.ToString(), "ref", uapVersion);
            if (!Directory.Exists(microsoftNetCoreUwpPkgFolder))
            {
                Assert.Fail($"Could not find path {microsoftNetCoreUwpPkgFolder}. This should have been installed as part of the UWP workload for Microsoft Visual Studio 2019 version 16.6");
            }
            references.AddRange(Directory.EnumerateFiles(microsoftNetCoreUwpPkgFolder, "*.dll"));


            // A custom target in the test project ensures the x64 versions of these assemblies are built
            // before the unittest is run.
            // I could not find a way for the dotnet core project to NOT be "Any CPU" and then produce x64 binaries
            // in that configuration. I tried all the usual tricks for the netcore project to match the universal
            // multi platform support.
            references.Add($@"{targetRoot}\X64\{configuration}\Microsoft.ReactNative.Managed\Microsoft.ReactNative.Managed\Microsoft.ReactNative.Managed.dll");
            references.Add($@"{targetRoot}\X64\{configuration}\Microsoft.ReactNative\Microsoft.ReactNative.winmd");

            // Add the windows 10 sdk assemblies for UWP. We have a compile time safeguard in place to
            // ensure that it matches the version that is defined in our MSBuild projects matches the one
            // used in our unittests. Unfortunately C# doesn't allow constants to flow into code, nor do any
            // of the unitest runners are allowed to pick up a variable from the build file. And given the many
            // ways one can run inttests, this seemed to be the most reasonable out of a lot of poor options.
            var win10SdkFolder = @"C:\Program Files (x86)\Windows Kits\10";
#if win10SdkVersion10_0_19041_0
      var win10SdkVersion = "10.0.19041.0";
#else
#error The Win10 Sdk Version must be updated in code when updated in MSBuild.
#endif

            references.AddRange(new[] {
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.AI.MachineLearning.MachineLearningContract\3.0.0.0\Windows.AI.MachineLearning.MachineLearningContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.AI.MachineLearning.Preview.MachineLearningPreviewContract\2.0.0.0\Windows.AI.MachineLearning.Preview.MachineLearningPreviewContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.ApplicationModel.Calls.Background.CallsBackgroundContract\2.0.0.0\Windows.ApplicationModel.Calls.Background.CallsBackgroundContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.ApplicationModel.Calls.CallsPhoneContract\5.0.0.0\Windows.ApplicationModel.Calls.CallsPhoneContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.ApplicationModel.Calls.CallsVoipContract\4.0.0.0\Windows.ApplicationModel.Calls.CallsVoipContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.ApplicationModel.CommunicationBlocking.CommunicationBlockingContract\2.0.0.0\Windows.ApplicationModel.CommunicationBlocking.CommunicationBlockingContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.ApplicationModel.SocialInfo.SocialInfoContract\2.0.0.0\Windows.ApplicationModel.SocialInfo.SocialInfoContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.ApplicationModel.StartupTaskContract\3.0.0.0\Windows.ApplicationModel.StartupTaskContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Devices.Custom.CustomDeviceContract\1.0.0.0\Windows.Devices.Custom.CustomDeviceContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Devices.DevicesLowLevelContract\3.0.0.0\Windows.Devices.DevicesLowLevelContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Devices.Printers.PrintersContract\1.0.0.0\Windows.Devices.Printers.PrintersContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Devices.SmartCards.SmartCardBackgroundTriggerContract\3.0.0.0\Windows.Devices.SmartCards.SmartCardBackgroundTriggerContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Devices.SmartCards.SmartCardEmulatorContract\6.0.0.0\Windows.Devices.SmartCards.SmartCardEmulatorContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Foundation.FoundationContract\4.0.0.0\Windows.Foundation.FoundationContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Foundation.UniversalApiContract\10.0.0.0\Windows.Foundation.UniversalApiContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Gaming.XboxLive.StorageApiContract\1.0.0.0\Windows.Gaming.XboxLive.StorageApiContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Graphics.Printing3D.Printing3DContract\4.0.0.0\Windows.Graphics.Printing3D.Printing3DContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Networking.Connectivity.WwanContract\2.0.0.0\Windows.Networking.Connectivity.WwanContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Networking.Sockets.ControlChannelTriggerContract\3.0.0.0\Windows.Networking.Sockets.ControlChannelTriggerContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Services.Maps.GuidanceContract\3.0.0.0\Windows.Services.Maps.GuidanceContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Services.Maps.LocalSearchContract\4.0.0.0\Windows.Services.Maps.LocalSearchContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Services.Store.StoreContract\4.0.0.0\Windows.Services.Store.StoreContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.Services.TargetedContent.TargetedContentContract\1.0.0.0\Windows.Services.TargetedContent.TargetedContentContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.System.Profile.ProfileHardwareTokenContract\1.0.0.0\Windows.System.Profile.ProfileHardwareTokenContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.System.Profile.ProfileSharedModeContract\2.0.0.0\Windows.System.Profile.ProfileSharedModeContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.System.Profile.SystemManufacturers.SystemManufacturersContract\3.0.0.0\Windows.System.Profile.SystemManufacturers.SystemManufacturersContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.System.SystemManagementContract\7.0.0.0\Windows.System.SystemManagementContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.UI.ViewManagement.ViewManagementViewScalingContract\1.0.0.0\Windows.UI.ViewManagement.ViewManagementViewScalingContract.winmd",
        $@"{win10SdkFolder}\References\{win10SdkVersion}\Windows.UI.Xaml.Core.Direct.XamlDirectContract\2.0.0.0\Windows.UI.Xaml.Core.Direct.XamlDirectContract.winmd",
        $@"{win10SdkFolder}\UnionMetadata\{win10SdkVersion}\Facade\Windows.winmd"
      });

            await codeAnalyzer.LoadMetadataReferencesAsync(references);

            var success = codeAnalyzer.TryCompileAndCheckForErrors();
            if (!success)
            {
                TestContext.WriteLine(string.Join(Environment.NewLine, codeAnalyzer.Diagnostics));
                Assert.Fail("Diagnostics encountered");
            }

            var errorCount = codeAnalyzer.Diagnostics.Count(diag => diag.Severity == DiagnosticSeverity.Error);
            Assert.AreEqual(0, errorCount);

            var type = codeAnalyzer.GetAllTypes().FirstOrDefault(tp => tp.Name == "TestClass");

            return (codeAnalyzer, type);
        }

        protected void ExpectSingleError(CodeAnalyzer analyzer, DiagnosticDescriptor descriptor)
        {
            Assert.AreEqual(1, analyzer.Diagnostics.Count());
            var error = analyzer.Diagnostics.First();
            Assert.AreEqual(descriptor, error.Descriptor);
            Assert.AreNotEqual(Location.None, error.Location);
        }
    }
}
