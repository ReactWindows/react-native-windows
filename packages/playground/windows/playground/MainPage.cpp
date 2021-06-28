#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <App.h>

using namespace winrt;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Media;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Pickers;

namespace winrt::playground::implementation {

MainPage::MainPage() {
  InitializeComponent();

  x_DebuggerPort().Text(std::to_wstring(Host().InstanceSettings().DebuggerPort()));

  // IsEditable is only supported on RS4 or higher
  if (Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(
          L"Windows.Foundation.UniversalApiContract", 6)) {
    x_rootComponentNameCombo().IsEditable(true);
    x_entryPointCombo().IsEditable(true);
  }

  // TODO: a way to determine which engines are actually available
  x_engineChakra().IsEnabled(true);
  x_engineHermes().IsEnabled(true);
  x_engineV8().IsEnabled(true);

  x_JsEngine().SelectedIndex(1);
}

void MainPage::OnUnloadClick(
    Windows::Foundation::IInspectable const & /*sender*/,
    xaml::RoutedEventArgs const & /*args*/) {
  Host().UnloadInstance();
}

void MainPage::OnLoadClick(
    Windows::Foundation::IInspectable const & /*sender*/,
    xaml::RoutedEventArgs const & /*args*/) {
  auto host = Host();

  winrt::hstring bundleFile;

  if (auto cbi = x_entryPointCombo().SelectedItem().try_as<ComboBoxItem>()) {
    bundleFile = unbox_value<hstring>(cbi.Content());
  } else {
    bundleFile = unbox_value<hstring>(x_entryPointCombo().SelectedItem());
  }
  host.InstanceSettings().JavaScriptBundleFile(bundleFile);

  auto item = x_rootComponentNameCombo().SelectedItem();
  winrt::hstring mainComponentName;
  if (auto selected = item.try_as<ComboBoxItem>()) {
    mainComponentName = unbox_value<hstring>(selected.Content());
  } else {
    mainComponentName = unbox_value<hstring>(item);
  }
  ReactRootView().ComponentName(mainComponentName);
  ReactRootView().ExperimentalUseFabric(x_UseFabric().IsChecked().GetBoolean());
  ReactRootView().ReactNativeHost(host);

  host.InstanceSettings().UseDeveloperSupport(true);
  host.InstanceSettings().UseWebDebugger(x_UseWebDebuggerCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().UseDirectDebugger(x_UseDirectDebuggerCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().DebuggerBreakOnNextLine(x_BreakOnFirstLineCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().UseFastRefresh(x_UseFastRefreshCheckBox().IsChecked().GetBoolean());
  host.InstanceSettings().DebuggerPort(static_cast<uint16_t>(std::stoi(std::wstring(x_DebuggerPort().Text()))));
  host.InstanceSettings().JSIEngineOverride(
      static_cast<Microsoft::ReactNative::JSIEngine>(x_JsEngine().SelectedIndex()));
  if (!m_bundlerHostname.empty()) {
    std::wstring dhost(m_bundlerHostname);
    auto colonPos = dhost.find(L':');
    if (colonPos != std::wstring::npos) {
      host.InstanceSettings().SourceBundleHost(hstring(dhost.substr(0, colonPos)));
      dhost.erase(0, colonPos + 1);
      host.InstanceSettings().SourceBundlePort(static_cast<uint16_t>(std::stoi(dhost)));
    }
  }

  host.InstanceSettings().InstanceCreated(
      [wkThis = get_weak()](auto sender, winrt::Microsoft::ReactNative::InstanceCreatedEventArgs args) {
        if (auto strongThis = wkThis.get()) {
          args.Context().UIDispatcher().Post([wkThis, context = args.Context()]() {
            if (auto strongThis = wkThis.get()) {
              strongThis->x_UseWebDebuggerCheckBox().IsChecked(context.SettingsSnapshot().UseWebDebugger());
              strongThis->x_UseFastRefreshCheckBox().IsChecked(context.SettingsSnapshot().UseFastRefresh());
              strongThis->x_UseDirectDebuggerCheckBox().IsChecked(context.SettingsSnapshot().UseDirectDebugger());
              strongThis->x_BreakOnFirstLineCheckBox().IsChecked(context.SettingsSnapshot().DebuggerBreakOnNextLine());
              auto debugBundlePath = context.SettingsSnapshot().DebugBundlePath();
              for (auto item : strongThis->x_entryPointCombo().Items()) {
                if (winrt::unbox_value<winrt::hstring>(item.as<ComboBoxItem>().Content()) == debugBundlePath) {
                  strongThis->x_entryPointCombo().SelectedItem(item);
                  break;
                }
              }
              strongThis->x_DebuggerPort().Text(winrt::to_hstring(context.SettingsSnapshot().DebuggerPort()));
              if (context.SettingsSnapshot().UseWebDebugger()) {
                strongThis->RequestedTheme(xaml::ElementTheme::Light);
              } else {
                strongThis->RequestedTheme(xaml::ElementTheme::Default);
              }
            }
          });
        }
      });
  // Nudge the ReactNativeHost to create the instance and wrapping context
  host.ReloadInstance();
}

void winrt::playground::implementation::MainPage::x_entryPointCombo_SelectionChanged(
    winrt::Windows::Foundation::IInspectable const & /*sender*/,
    xaml::Controls::SelectionChangedEventArgs const & /*e*/) {
  if (x_rootComponentNameCombo()) {
    auto item = x_entryPointCombo().SelectedItem();
    if (item) {
      winrt::IInspectable content;
      if (auto cbi = item.try_as<ComboBoxItem>()) {
        content = cbi.Content();
      } else {
        content = item;
      }

      auto selectedItem = winrt::to_string(winrt::unbox_value<winrt::hstring>(content));
      if (selectedItem == "Samples\\rntester") {
        x_rootComponentNameCombo().SelectedIndex(0);
      } else {
        x_rootComponentNameCombo().SelectedIndex(1);
      }
    }
  }
}

void winrt::playground::implementation::MainPage::x_UseWebDebuggerCheckBox_Checked(
    winrt::Windows::Foundation::IInspectable const & /*sender*/,
    winrt::Windows::Foundation::IInspectable const & /*e*/) {
  if (x_JsEngine()) {
    x_JsEngine().IsEnabled(false);
  }
  if (x_DebuggerPort()) {
    x_DebuggerPort().IsEnabled(false);
  }
  if (x_BreakOnFirstLineCheckBox()) {
    x_BreakOnFirstLineCheckBox().IsEnabled(false);
  }
}

void winrt::playground::implementation::MainPage::x_UseWebDebuggerCheckBox_Unchecked(
    winrt::Windows::Foundation::IInspectable const & /*sender*/,
    winrt::Windows::Foundation::IInspectable const & /*e*/) {
  if (x_JsEngine()) {
    x_JsEngine().IsEnabled(true);
  }
  if (x_DebuggerPort()) {
    x_DebuggerPort().IsEnabled(true);
  }
  if (x_BreakOnFirstLineCheckBox()) {
    x_BreakOnFirstLineCheckBox().IsEnabled(true);
  }
}

void MainPage::OnNavigatedTo(xaml::Navigation::NavigationEventArgs const &e) {
  m_bundlerHostname = unbox_value<hstring>(e.Parameter());
}

Microsoft::ReactNative::ReactNativeHost MainPage::Host() noexcept {
  return Application::Current().as<App>()->Host();
}

Microsoft::ReactNative::ReactInstanceSettings MainPage::InstanceSettings() noexcept {
  if (!m_instanceSettings) {
    m_instanceSettings = Microsoft::ReactNative::ReactInstanceSettings();
  }

  return m_instanceSettings;
}

Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider>
MainPage::PackageProviders() noexcept {
  if (!m_packageProviders) {
    m_packageProviders = single_threaded_vector<Microsoft::ReactNative::IReactPackageProvider>();
  }

  return m_packageProviders;
}

} // namespace winrt::playground::implementation
