#pragma once
#include "MainPage.g.h"

namespace winrt::playground::implementation {

struct MainPage : MainPageT<MainPage> {
  MainPage();

  void OnLoadClick(Windows::Foundation::IInspectable const & /*sender*/, xaml::RoutedEventArgs const & /*args*/);
  void OnUnloadClick(Windows::Foundation::IInspectable const & /*sender*/, xaml::RoutedEventArgs const & /*args*/);

 private:
  Microsoft::ReactNative::ReactNativeHost Host() noexcept;
  Microsoft::ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider> PackageProviders() noexcept;

  Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings;
  Windows::Foundation::Collections::IVector<Microsoft::ReactNative::IReactPackageProvider> m_packageProviders;

  winrt::hstring m_bundlerHostname;

 public:
  void x_entryPointCombo_SelectionChanged(
      winrt::Windows::Foundation::IInspectable const &sender,
      xaml::Controls::SelectionChangedEventArgs const &e);
  void x_UseWebDebuggerCheckBox_Checked(
      winrt::Windows::Foundation::IInspectable const &sender,
      winrt::Windows::Foundation::IInspectable const &e);
  void x_UseWebDebuggerCheckBox_Unchecked(
      winrt::Windows::Foundation::IInspectable const &sender,
      winrt::Windows::Foundation::IInspectable const &e);
  void OnNavigatedTo(xaml::Navigation::NavigationEventArgs const &e);
};

} // namespace winrt::playground::implementation
namespace winrt::playground::factory_implementation {
struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
} // namespace winrt::playground::factory_implementation
