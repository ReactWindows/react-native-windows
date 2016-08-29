using Newtonsoft.Json.Linq;
using ReactNative.UIManager;
using ReactNative.UIManager.Annotations;
using ReactNative.UIManager.Events;
using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace ReactNative.Views.Slider
{
    /// <summary>
    /// A view manager responsible for rendering picker.
    /// </summary>
    public class ReactSliderManager : BaseViewManager<Windows.UI.Xaml.Controls.Slider, ReactSliderShadowNode>
    { 
        /// <summary>
        /// The name of the view manager.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RCTSlider";
            }
        }

        /// <summary>
        /// Sets whether a picker is disabled.
        /// </summary>
        /// <param name="view">a slider view.</param>
        /// <param name="disabled">
        /// Set to <code>true</code> if the picker should be disabled,
        /// otherwise, set to <code>false</code>.
        /// </param>
        [ReactProp("disabled")]
        public void SetDisabled(Windows.UI.Xaml.Controls.Slider view, bool disabled)
        {
            view.IsEnabled = !disabled;
        }

        /// <summary>
        /// Sets to change slider minimum value.
        /// </summary>
        /// <param name="view">a slider view.</param>
        /// <param name="minimum">
        ///
        /// </param>
        [ReactProp("minimumValue")]
        public void SetMinimumValue(Windows.UI.Xaml.Controls.Slider view, double minimum)
        {
            view.Minimum = minimum;
        }

        /// <summary>
        /// Sets to change slider maximum value.
        /// </summary>
        /// <param name="view">a slider view.</param>
        /// <param name="minimum">
        ///
        /// </param>
        [ReactProp("maximumValue")]
        public void SetMaximumValue(Windows.UI.Xaml.Controls.Slider view, double maximum)
        {
            view.Maximum = maximum;
        }

        /// <summary>
        /// Sets slider value.
        /// </summary>
        /// <param name="view">The slider view.</param>
        /// <param name="value">
        ///
        /// </param>
        [ReactProp(ViewProps.Value)]
        public void SetValue(Windows.UI.Xaml.Controls.Slider view, double value)
        {
            view.Value = value;
        }

        /// <summary>
        /// Sets slider step.
        /// </summary>
        /// <param name="view">The slider view.</param>
        /// <param name="step">
        ///
        /// </param>
        [ReactProp("step")]
        public void SetStep(Windows.UI.Xaml.Controls.Slider view, double step)
        {
            view.StepFrequency = step;
        }

        /// <summary>
        /// This method should return the <see cref="ReactPickerShadowNode"/>
        /// which will be then used for measuring the position and size of the
        /// view. 
        /// </summary>
        /// <returns>The shadow node instance.</returns>
        public override ReactSliderShadowNode CreateShadowNodeInstance()
        {
            return new ReactSliderShadowNode();
        }

        public override void UpdateExtraData(Windows.UI.Xaml.Controls.Slider root, object extraData)
        {
            throw new NotImplementedException();
        }

        protected override Windows.UI.Xaml.Controls.Slider CreateViewInstance(ThemedReactContext reactContext)
        {
            return new Windows.UI.Xaml.Controls.Slider();
        }

        /// <summary>
        /// Subclasses can override this method to install custom event 
        /// emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        protected override void AddEventEmitters(ThemedReactContext reactContext, Windows.UI.Xaml.Controls.Slider view)
        {
            view.ValueChanged += OnValueChange;
            view.PointerReleased += OnPointerReleased;
            view.PointerCaptureLost += OnPointerReleased;
        }

        private void OnValueChange(object sender, RoutedEventArgs e)
        {
            var slider = (Windows.UI.Xaml.Controls.Slider)sender;
            var reactContext = slider.GetReactContext();
            reactContext.GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactSliderChangeEvent(
                        slider.GetTag(),
                        slider.Value));
        }

        private void OnPointerReleased(object sender, RoutedEventArgs e)
        {
            var slider = (Windows.UI.Xaml.Controls.Slider)sender;
            var reactContext = slider.GetReactContext();
            reactContext.GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactSliderCompleteEvent(
                        slider.GetTag(),
                        slider.Value));
        }


    }
}
