using Microsoft.ReactNative;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace {{ namespace }}
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
            reactRootView.Loaded += ReactRootView_Loaded;
        }

        private async void ReactRootView_Loaded(object sender, RoutedEventArgs e)
        {
            var app = Application.Current as App;
            XamlUIService.SetXamlRoot(app.Host.InstanceSettings.Properties, this.Content.XamlRoot);
            reactRootView.ReactNativeHost = app.Host;
            await app.Host.LoadInstance();
        }
    }
}
