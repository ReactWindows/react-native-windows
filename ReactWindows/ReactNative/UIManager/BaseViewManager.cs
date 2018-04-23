// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using ReactNative.Accessibility;
using ReactNative.Reflection;
using ReactNative.Storage;
using ReactNative.Touch;
using ReactNative.UIManager.Annotations;
using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.Storage;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Media3D;

namespace ReactNative.UIManager
{
    /// <summary>
    /// Base class that should be suitable for the majority of subclasses of <see cref="IViewManager"/>.
    /// It provides support for base view props such as opacity, etc.
    /// </summary>
    /// <typeparam name="TFrameworkElement">Type of framework element.</typeparam>
    /// <typeparam name="TLayoutShadowNode">Type of shadow node.</typeparam>
    public abstract class BaseViewManager<TFrameworkElement, TLayoutShadowNode> :
            ViewManager<TFrameworkElement, TLayoutShadowNode>
        where TFrameworkElement : FrameworkElement
        where TLayoutShadowNode : LayoutShadowNode
    {
        private readonly ConcurrentDictionary<TFrameworkElement, DimensionBoundProperties> _dimensionBoundProperties =
            new ConcurrentDictionary<TFrameworkElement, DimensionBoundProperties>();

        /// <summary>
        /// Sets the 3D tranform on the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="transforms">
        /// The transform matrix or the list of transforms.
        /// </param>
        [ReactProp("transform")]
        public void SetTransform(TFrameworkElement view, JArray transforms)
        {
            if (transforms == null)
            {
                var dimensionBoundProperties = GetDimensionBoundProperties(view);
                if (dimensionBoundProperties?.MatrixTransform != null)
                {
                    dimensionBoundProperties.MatrixTransform = null;
                    ResetProjectionMatrix(view);
                    ResetRenderTransform(view);
                }
            }
            else
            {
                var dimensionBoundProperties = GetOrCreateDimensionBoundProperties(view);
                dimensionBoundProperties.MatrixTransform = transforms;
                var dimensions = GetDimensions(view);
                SetProjectionMatrix(view, dimensions, transforms);
            }
        }

        /// <summary>
        /// Sets the opacity of the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="opacity">The opacity value.</param>
        [ReactProp("opacity", DefaultDouble = 1.0)]
        public void SetOpacity(TFrameworkElement view, double opacity)
        {
            view.Opacity = opacity;
        }

        /// <summary>
        /// Sets the overflow prop for the <typeparamref name="TFrameworkElement"/>.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="overflow">The overflow value.</param>
        [ReactProp("overflow")]
        public void SetOverflow(TFrameworkElement view, string overflow)
        {
            if (overflow == "hidden")
            {
                var dimensionBoundProperties = GetOrCreateDimensionBoundProperties(view);
                dimensionBoundProperties.OverflowHidden = true;
                var dimensions = GetDimensions(view);
                SetOverflowHidden(view, dimensions);
                view.SizeChanged += OnSizeChanged;
            }
            else
            {
                view.SizeChanged -= OnSizeChanged;
                var dimensionBoundProperties = GetDimensionBoundProperties(view);
                if (dimensionBoundProperties != null && dimensionBoundProperties.OverflowHidden)
                {
                    dimensionBoundProperties.OverflowHidden = false;
                    SetOverflowVisible(view);
                }
            }
        }

        /// <summary>
        /// Sets the z-index of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="zIndex">The z-index.</param>
        [ReactProp("zIndex")]
        public void SetZIndex(TFrameworkElement view, int zIndex)
        {
            Canvas.SetZIndex(view, zIndex);
        }

        /// <summary>
        /// Sets the display mode of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="display">The display mode.</param>
        [ReactProp(ViewProps.Display)]
        public void SetDisplay(TFrameworkElement view, string display)
        {
            view.Visibility = display == "none" ? Visibility.Collapsed : Visibility.Visible;
        }

        /// <summary>
        /// Sets the manipulation mode for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="manipulationModes">The manipulation modes.</param>
        [ReactProp("manipulationModes")]
        public void SetManipulationModes(TFrameworkElement view, JArray manipulationModes)
        {
            if (manipulationModes == null)
            {
                view.ManipulationMode = ManipulationModes.System;
                return;
            }

            var manipulationMode = ManipulationModes.System;
            foreach (var modeString in manipulationModes)
            {
                Debug.Assert(modeString.Type == JTokenType.String);
                var mode = EnumHelpers.Parse<ManipulationModes>(modeString.Value<string>());
                manipulationMode |= mode;
            }

            view.ManipulationMode = manipulationMode;
        }

        /// <summary>
        /// Sets the accessibility label of the element.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="label">The label.</param>
        [ReactProp("accessibilityLabel")]
        public void SetAccessibilityLabel(TFrameworkElement view, string label)
        {
            AccessibilityHelper.SetAccessibilityLabel(view, label ?? "");
        }

        /// <summary>
        /// Sets the accessibility live region.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="liveRegion">The live region.</param>
        [ReactProp("accessibilityLiveRegion")]
        public void SetAccessibilityLiveRegion(TFrameworkElement view, string liveRegion)
        {
            var liveSetting = AutomationLiveSetting.Off;
            switch (liveRegion)
            {
                case "polite":
                    liveSetting = AutomationLiveSetting.Polite;
                    break;
                case "assertive":
                    liveSetting = AutomationLiveSetting.Assertive;
                    break;
            }

            AutomationProperties.SetLiveSetting(view, liveSetting);
        }

        /// <summary>
        /// Sets the test ID, i.e., the automation ID.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="testId">The test ID.</param>
        [ReactProp("testID")]
        public void SetTestId(TFrameworkElement view, string testId)
        {
            AutomationProperties.SetAutomationId(view, testId ?? "");
        }

        /// <summary>
        /// Enables the Canvas as a drop target.
        /// </summary>
        [ReactProp("allowDrop")]
        public void SetAllowDrop(TFrameworkElement view, bool allowDrop)
        {
            view.AllowDrop = allowDrop;

            if (allowDrop)
            {
                view.DragEnter += OnDragEnter;
                view.DragOver += OnDragOver;
                view.Drop += OnDrop;
                view.DragLeave += OnDragLeave;
            }
            else
            {
                view.DragEnter -= OnDragEnter;
                view.DragOver -= OnDragOver;
                view.Drop -= OnDrop;
                view.DragLeave -= OnDragLeave;
            }
        }

        /// <summary>
        /// Sets a tooltip for the view.
        /// </summary>
        /// <param name="view">The view instance.</param>
        /// <param name="tooltip">String to display in the tooltip.</param>
        [ReactProp("tooltip")]
        public void SetTooltip(TFrameworkElement view, string tooltip)
        {
            ToolTipService.SetToolTip(view, tooltip);
        }

        /// <summary>
        /// Called when view is detached from view hierarchy and allows for 
        /// additional cleanup by the <see cref="IViewManager"/> subclass.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view.</param>
        /// <remarks>
        /// Be sure to call this base class method to register for pointer 
        /// entered and pointer exited events.
        /// </remarks>
        public override void OnDropViewInstance(ThemedReactContext reactContext, TFrameworkElement view)
        {
            view.DragEnter -= OnDragEnter;
            view.DragOver -= OnDragOver;
            view.Drop -= OnDrop;
            view.DragLeave -= OnDragLeave;
            view.PointerEntered -= OnPointerEntered;
            view.PointerExited -= OnPointerExited;
            _dimensionBoundProperties.TryRemove(view, out _);
        }

        /// <summary>
        /// Sets the dimensions of the view.
        /// </summary>
        /// <param name="view">The view.</param>
        /// <param name="dimensions">The dimensions.</param>
        public override void SetDimensions(TFrameworkElement view, Dimensions dimensions)
        {
            var dimensionBoundProperties = GetDimensionBoundProperties(view);
            var matrixTransform = dimensionBoundProperties?.MatrixTransform;
            var overflowHidden = dimensionBoundProperties?.OverflowHidden ?? false;
            if (matrixTransform != null)
            {
                SetProjectionMatrix(view, dimensions, matrixTransform);
            }

            if (overflowHidden)
            {
                SetOverflowHidden(view, dimensions);
                view.SizeChanged -= OnSizeChanged;
            }

            base.SetDimensions(view, dimensions);

            if (overflowHidden)
            {
                view.SizeChanged += OnSizeChanged;
            }
        }

        /// <summary>
        /// Subclasses can override this method to install custom event 
        /// emitters on the given view.
        /// </summary>
        /// <param name="reactContext">The React context.</param>
        /// <param name="view">The view instance.</param>
        /// <remarks>
        /// Consider overriding this method if your view needs to emit events
        /// besides basic touch events to JavaScript (e.g., scroll events).
        /// 
        /// Make sure you call the base implementation to ensure base pointer
        /// event handlers are subscribed.
        /// </remarks>
        protected override void AddEventEmitters(ThemedReactContext reactContext, TFrameworkElement view)
        {
            view.PointerEntered += OnPointerEntered;
            view.PointerExited += OnPointerExited;
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            var view = (TFrameworkElement)sender;
            view.Clip = new RectangleGeometry
            {
                Rect = new Rect(0, 0, e.NewSize.Width, e.NewSize.Height),
            };
        }

        private void OnPointerEntered(object sender, PointerRoutedEventArgs e)
        {
            var view = (TFrameworkElement)sender;
            TouchHandler.OnPointerEntered(view, e);
        }

        private void OnPointerExited(object sender, PointerRoutedEventArgs e)
        {
            var view = (TFrameworkElement)sender;
            TouchHandler.OnPointerExited(view, e);
        }

        // This event is intentionally allowed to bubble up, so all the corresponding
        // RN views get the proper enter-leave event pairs.
        private static async void OnDragEnter(object sender, DragEventArgs args)
        {
            var view = (TFrameworkElement)sender;
            var data = await GetDataTransferInfo(args.DataView);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDragEnter", data));
        }

        private static async void OnDragOver(object sender, DragEventArgs args)
        {
            args.Handled = true;

            var view = (TFrameworkElement)sender;

            // In web when JS gets a "drag over" event, it modifies the event
            // object to tell if the DOM element supports dropping items:
            //
            //      e.dataTransfer.effectAllowed = 'copy';
            //      e.dataTransfer.dropEffect = 'copy';
            //
            // However in RN this approach doesn't work, so we use a simpler
            // solution: JS sets allowDrop=true which implies that the RN element
            // always allows dropping items.
            args.AcceptedOperation = DataPackageOperation.Copy;

            var data = await GetDataTransferInfo(args.DataView);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDragOver", data));
        }

        private static async void OnDrop(object sender, DragEventArgs args)
        {
            args.Handled = true;

            var view = (TFrameworkElement)sender;
            var data = await GetDataTransferInfo(args.DataView, true);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDrop", data));
        }

        // This event is intentionally allowed to bubble up, so all the corresponding
        // RN views get the proper enter-leave event pairs.
        private static async void OnDragLeave(object sender, DragEventArgs args)
        {
            var view = (TFrameworkElement)sender;
            var data = await GetDataTransferInfo(args.DataView);

            view.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(new DragDropEvent(view.GetTag(), "topDragLeave", data));
        }

        private DimensionBoundProperties GetDimensionBoundProperties(TFrameworkElement view)
        {
            if (!_dimensionBoundProperties.TryGetValue(view, out var properties))
            {
                properties = null;
            }

            return properties;
        }

        private DimensionBoundProperties GetOrCreateDimensionBoundProperties(TFrameworkElement view)
        {
            if (!_dimensionBoundProperties.TryGetValue(view, out var properties))
            {
                properties = new DimensionBoundProperties();
                _dimensionBoundProperties.AddOrUpdate(view, properties, (k, v) => properties);
            }

            return properties;
        }

        private static void SetProjectionMatrix(TFrameworkElement view, Dimensions dimensions, JArray transforms)
        {
            var transformMatrix = TransformHelper.ProcessTransform(transforms);

            var translateMatrix = Matrix3D.Identity;
            var translateBackMatrix = Matrix3D.Identity;
            if (!double.IsNaN(dimensions.Width))
            {
                translateMatrix.OffsetX = -dimensions.Width / 2;
                translateBackMatrix.OffsetX = dimensions.Width / 2;
            }

            if (!double.IsNaN(dimensions.Height))
            {
                translateMatrix.OffsetY = -dimensions.Height / 2;
                translateBackMatrix.OffsetY = dimensions.Height / 2;
            }

            var projectionMatrix = translateMatrix * transformMatrix * translateBackMatrix;
            ApplyProjection(view, projectionMatrix);
        }

        private static void ApplyProjection(TFrameworkElement view, Matrix3D projectionMatrix)
        {
            if (IsSimpleTranslationOnly(projectionMatrix))
            {
                ResetProjectionMatrix(view);
                // We need to use a new instance of MatrixTransform because matrix
                // updates to an existing MatrixTransform don't seem to take effect.
                var transform = new MatrixTransform();
                var matrix = transform.Matrix;
                matrix.OffsetX = projectionMatrix.OffsetX;
                matrix.OffsetY = projectionMatrix.OffsetY;
                transform.Matrix = matrix;
                view.RenderTransform = transform;
            }
            else
            {
                ResetRenderTransform(view);
                var projection = EnsureProjection(view);
                projection.ProjectionMatrix = projectionMatrix;
            }
        }

        private static bool IsSimpleTranslationOnly(Matrix3D matrix)
        {
            // Matrix3D is a struct and passed-by-value. As such, we can modify
            // the values in the matrix without affecting the caller.
            matrix.OffsetX = matrix.OffsetY = 0;
            return matrix.IsIdentity;
        }

        private static void ResetProjectionMatrix(TFrameworkElement view)
        {
            var projection = view.Projection;
            var matrixProjection = projection as Matrix3DProjection;
            if (projection != null && matrixProjection == null)
            {
                throw new InvalidOperationException("Unknown projection set on framework element.");
            }

            view.Projection = null;
        }

        private static void ResetRenderTransform(TFrameworkElement view)
        {
            var transform = view.RenderTransform;
            var matrixTransform = transform as MatrixTransform;
            if (transform != null && matrixTransform == null)
            {
                throw new InvalidOperationException("Unknown transform set on framework element.");
            }

            view.RenderTransform = null;
        }

        private static Matrix3DProjection EnsureProjection(FrameworkElement view)
        {
            var projection = view.Projection;
            var matrixProjection = projection as Matrix3DProjection;
            if (projection != null && matrixProjection == null)
            {
                throw new InvalidOperationException("Unknown projection set on framework element.");
            }

            if (matrixProjection == null)
            {
                matrixProjection = new Matrix3DProjection();
                view.Projection = matrixProjection;
            }

            return matrixProjection;
        }

        private static void SetOverflowHidden(TFrameworkElement element, Dimensions dimensions)
        {
            if (double.IsNaN(dimensions.Width) || double.IsNaN(dimensions.Height))
            {
                element.Clip = null;
            }
            else
            {
                element.Clip = new RectangleGeometry
                {
                    Rect = new Rect(0, 0, dimensions.Width, dimensions.Height),
                };
            }
        }

        private static void SetOverflowVisible(TFrameworkElement element)
        {
            element.Clip = null;
        }

        private static async Task<JObject> GetDataTransferInfo(DataPackageView data, bool drop = false)
        {
            var files = new JArray();
            var items = new JArray();
            var types = new JArray();

            if (data.Contains(StandardDataFormats.StorageItems))
            {
                foreach (var item in await data.GetStorageItemsAsync())
                {
                    var file = item as StorageFile;

                    if (file != null)
                    {
                        var props = await file.GetBasicPropertiesAsync();
                        var type = file.ContentType;
                        var path = drop ? FutureAccessList.Add(file) : "";

                        files.Add(new JObject
                            {
                                { "name", file.Name },
                                { "size", props.Size },
                                { "type", type },
                                { "uri", path }
                            });

                        items.Add(new JObject
                            {
                                { "kind", "file" },
                                { "type", type }
                            });

                        types.Add(type);
                    }
                }
            }

            return new JObject
            {
                { "files", files },
                { "items", items },
                { "types", types }
            };
        }

        class DimensionBoundProperties
        {
            public bool OverflowHidden { get; set; }

            public JArray MatrixTransform { get; set; }
        }
    }
}
