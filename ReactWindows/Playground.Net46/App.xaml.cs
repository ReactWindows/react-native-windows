﻿using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace Playground.Net46
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        private readonly AppReactPage _reactPage = new AppReactPage();

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            OnCreate(e.Args);
        }

        protected override void OnActivated(EventArgs e)
        {
            base.OnActivated(e);
        }

        /// <summary>
        /// Called whenever the app is opened to initia
        /// </summary>
        /// <param name="arguments"></param>
        private void OnCreate(string[] arguments)
        {
            var shellWindow = Application.Current.MainWindow;

            if (shellWindow == null)
            {
                shellWindow = new Window();

                Application.Current.MainWindow = shellWindow;
            }

            var rootFrame = shellWindow.Content as Frame;

            // Do not repeat app initialization when the Window already has content,
            // just ensure that the window is active
            if (rootFrame == null)
            {
                _reactPage.OnCreate(arguments);

                // Create a Frame to act as the navigation context and navigate to the first page
                rootFrame = new Frame();

                rootFrame.NavigationFailed += OnNavigationFailed;

                // Place the frame in the current Window
                shellWindow.Content = rootFrame;
            }

            if (rootFrame.Content == null)
            {
                // When the navigation stack isn't restored navigate to the first page,
                // configuring the new page by passing required information as a navigation
                // parameter
                rootFrame.Content = _reactPage;
            }

            shellWindow.ShowActivated = true;
            shellWindow.ShowInTaskbar = true;
            shellWindow.Title = "Playground.Net46";
            shellWindow.Height = 768;
            shellWindow.Width = 1024;
            shellWindow.WindowStartupLocation = WindowStartupLocation.CenterScreen;

            // Ensure the current window is active
            shellWindow.Show();
        }

        /// <summary>
        /// Invoked when Navigation to a certain page fails
        /// </summary>
        /// <param name="sender">The Frame which failed navigation</param>
        /// <param name="e">Details about the navigation failure</param>
        private void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            throw new Exception("Failed to load Page...");
        }
    }
}
