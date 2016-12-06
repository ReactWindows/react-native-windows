﻿using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Collections;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;
using Windows.UI.Popups;

namespace ReactNative.Modules.Dialog
{
    class DialogModule : ReactContextNativeModuleBase, ILifecycleEventListener
    {
        private MessageDialog _pendingDialog;
        private bool _isInForeground;

        public DialogModule(ReactContext reactContext)
            : base(reactContext)
        {
        }

        public override string Name
        {
            get
            {
                return "DialogManagerWindows";
            }
        }

        public override IReadOnlyDictionary<string, object> Constants
        {
            get
            {
                return new Dictionary<string, object>
                {
                    { DialogModuleHelper.ActionButtonClicked, DialogModuleHelper.ActionButtonClicked },
                    { DialogModuleHelper.ActionDismissed, DialogModuleHelper.ActionDismissed },
                    { DialogModuleHelper.KeyButtonPositive, DialogModuleHelper.KeyButtonPositiveValue },
                    { DialogModuleHelper.KeyButtonNegative, DialogModuleHelper.KeyButtonNegativeValue },
                };
            }
        }

        public override void Initialize()
        {
            Context.AddLifecycleEventListener(this);
        }

        public void OnSuspend()
        {
            _isInForeground = false;   
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "Reviewed.")]
#pragma warning disable AvoidAsyncVoid
        public async void OnResume()
#pragma warning restore AvoidAsyncVoid
        {
            _isInForeground = true;

            var pendingDialog = _pendingDialog;
            _pendingDialog = null;
            if (pendingDialog != null)
            {
                await pendingDialog.ShowAsync().AsTask().ConfigureAwait(false);
            }
        }

        public void OnDestroy()
        {
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("AsyncUsage.CSharp.Reliability", "AvoidAsyncVoid", Justification = "Reviewed.")]
        [ReactMethod]
#pragma warning disable AvoidAsyncVoid
        public async void showAlert(
#pragma warning restore AvoidAsyncVoid
            JObject config,
            ICallback errorCallback,
            ICallback actionCallback)
        {
            var message = config.Value<string>("message") ?? "";
            var messageDialog = new MessageDialog(message)
            {
                Title = config.Value<string>("title"),
            };

            if (config.ContainsKey(DialogModuleHelper.KeyButtonPositive))
            {
                messageDialog.Commands.Add(new UICommand
                {
                    Label = config.Value<string>(DialogModuleHelper.KeyButtonPositive),
                    Id = DialogModuleHelper.KeyButtonPositiveValue,
                    Invoked = target => OnInvoked(target, actionCallback),
                });
            }

            if (config.ContainsKey(DialogModuleHelper.KeyButtonNegative))
            {
                messageDialog.Commands.Add(new UICommand
                {
                    Label = config.Value<string>(DialogModuleHelper.KeyButtonNegative),
                    Id = DialogModuleHelper.KeyButtonNegativeValue,
                    Invoked = target => OnInvoked(target, actionCallback),
                });
            }

            await RunOnDispatcherAsync(async () =>
            {
                if (_isInForeground)
                {
                    await messageDialog.ShowAsync().AsTask().ConfigureAwait(false);
                }
                else
                {
                    _pendingDialog = messageDialog;
                }
            });
        }

        private void OnInvoked(IUICommand target, ICallback callback)
        {
            callback.Invoke(DialogModuleHelper.ActionButtonClicked, target.Id);
        }

        private static async Task RunOnDispatcherAsync(DispatchedHandler action)
        {
            await CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, action).AsTask().ConfigureAwait(false);
        }
    }
}