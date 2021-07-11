# Scroll Anchoring in React Native

Scroll anchoring allows app developers to prevent content from jumping in a scrollable context. It generally works by selecting an anchor in the view port and adjusting the scroll offsets synchronously when the anchor has changed position as a result of layout. This functionality is enabled by default in many browsers.

## Why do we need scroll anchoring?

The primary motivation for adding scroll anchoring to React Native is to support reversed lists. In React Native, the VirtualizedList component has an inverted prop that allows you to reverse order of items rendered in the scroll content container. As of writing, this is implemented with scale transforms, flipping the entire content container on the scroll axis, and then flipping each item again on the scroll axis. While this works for many scenarios on mobile devices, the approach breaks down on React Native desktop platforms when it comes to scrolling. Here are a handful of issues related to the inverted prop on desktop platforms:

- https://github.com/facebook/react-native/issues/25239
- https://github.com/facebook/react-native/issues/28556
- https://github.com/facebook/react-native/issues/30383
- https://github.com/necolas/react-native-web/issues/995
- https://github.com/necolas/react-native-web/issues/1254
- https://github.com/necolas/react-native-web/issues/1579
- https://github.com/necolas/react-native-web/issues/1807
- https://github.com/microsoft/react-native-windows/issues/4098
- https://github.com/facebook/yoga/issues/866

If we cannot create an inverted VirtualizedList using the inverted prop as implemented today, perhaps we can approximate the most important functional requirements.

### Functional Requirements of inverted VirtualizedList

#### Render views from bottom to top

When using inverted VirtualizedList, views are rendered from bottom to top of the scroll view. This can be achieved already with existing React Native and Yoga functionality, setting `flexDirection: 'column-reverse'` and `minHeight: '100%'` on the `contentContainerStyle` prop for vertical VirtualizedLists, or `flexDirection: 'row-reverse'` and `minWidth: '100%'` for horizontal.

The reversed flex direction ensures that the view rendered at the bottom of the ScrollViewer on XAML will be first in child order, and thus be rendered first. While it's also possible to reverse the order of the items in the list view, this would result in items being rendered to screen out of order, and depending on how many items are to be rendered, it may appear as if the ScrollView scrolls down to the bottom edge as the initial rendering of items completes.

The minimum dimension setting is required to ensure that the ScrollContentView parent is at least as tall as the view port so the list items always layout at the bottom edge of the view port, even when there is not enough content to fill the view port.

#### Bottom edge anchoring

When using inverted VirtualizedList, as new items are added to the "top" of the list (i.e., inserted at position 0), when at the bottom edge, the view port appears to anchor such that the newly inserted item appears to scroll into view and layout synchronously. In reality, since the ScrollView itself is flipped, the view port is just staying at offset 0.

In XAML, there are two properties, [`HorizontalAnchorRatio`](https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.scrollviewer.horizontalanchorratio?view=winrt-19041) and [`VerticalAnchorRatio`](https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.scrollviewer.verticalanchorratio?view=winrt-19041), that implement bottom edge anchoring when given a value of 1.0 (or top edge anchoring when given a value of 0.0). Any other value controls how `IScrollAnchorProvider` chooses to anchor a specific view in the view port.

One caveat to the anchor ratio properties in XAML is that you need to call `ScrollViewer.InvalidateArrange` whenever the layout of its descendants change (e.g., when a new view is inserted) in order for the edge anchoring behavior to function properly.

Since currently we only want to have start or end edge anchoring, or none at all, we could add a prop called `anchorEdge` with value options `start`, `end`, or `none`.

#### View anchoring not anchored at edge

Another feature of inverted VirtualizedList is that view port will not jump when more content is loaded "above" it. Again, this is a side-effect of the ScrollView itself being inverted, so the view port offset actually remains the same, even though the perceived offset will have shifted downward.

In XAML, we can achieve this using the [`CanBeScrollAnchor`](https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.uielement.canbescrollanchor?view=winrt-19041) property on the list items. Under the hood, the XAML ScrollViewer implements `IScrollAnchorProvider`, and if a layout operation will result in a selected scroll anchor changing position, the view port will be synchronously scrolled to keep the scroll anchor in place. The `CanBeScrollAnchor` property simply tells the ScrollViewer which of its descendants are eligible to be scroll anchors.

This view anchoring behavior is actually the default behavior on many Web browsers, and [some Web browsers](https://developer.mozilla.org/en-US/docs/Web/CSS/overflow-anchor#browser_compatibility) even expose a CSS property to opt-out of scroll anchoring. Since what we are trying to achieve here has effectively the same semantics as scroll anchoring on Web browsers, we could call the prop to enable scroll anchoring `overflowAnchor` and add it to `View`. Since this is not a platform default behavior, and there is likely some performance penalty to opting in to scroll anchoring, we should add an `overflowAnchorMode` prop to `ScrollView` to switch between `explicit` (opt-in), `implicit` (opt-out), and 

You might ask, why might you even need to specify which values to opt-out of scroll anchoring? Could we not just turn on anchoring for all top-level items in the ScrollContentView and call it a day? We cannot because of VirtualizedList and sticky headers. Consider what might happen if the ScrollView chooses the VirtualizedList spacer as the anchor candidate. As content is rendered in the view port and the spacer shrinks, the view port will scroll to keep the spacer anchored, which is not expected behavior. Similarly for sticky headers, as the sticky header shifts to remain at the top of the view port, if the ScrollView selected the sticky header as the anchor, it may not appear to anchor at all as the sticky header has it's own mechanism to remain anchored to the top of the view port. Note, there is also some precedent for opting out specific views in anchoring on iOS with the `maintainVisibleContentPosition` prop. If you exclude the auto-scroll to top functionality that this iOS prop provides, you could potentially emulate, e.g., `maintainVisibleContentPosition={{minIndexForVisible: 1}}` using `enableAnchoring={true}` and setting `overflowAnchor={'none'}` on the first item in the list (or the first N items where N is the value for `minIndexForVisible`).

#### Layout independent scroll-to-end

When you want to scroll to the bottom of an inverted VirtualizedList, it's pretty easy because you can just scroll to offset 0. No matter what layout changes occur between the time that the scroll operation starts and completes, the final result will be that the view is at offset 0.

For a non-inverted ScrollView, scrolling to the bottom means scrolling to the end of the content. In XAML, the `scrollToEnd` command checks once for the `ScrollableHeight` (or `ScrollableWidth`) property of the `ScrollViewer` and calls the `ScrollViewer.ChangeView` function. Unfortunately, if a new view is added to the bottom of the view while, for example, the `scrollToEnd` command is animating, the view port will come to rest just above the new view.

To get around this, we should add a new behavior option to the `ScrollView.scrollToEnd` command that would update the final offset of the scroll operation if the scrollable height (or width) changes before the animation completes. Right now, the `ScrollView.scrollToEnd` command already accepts a JSON payload to toggle the scroll animation behavior. We could add an additional property to that payload called `layoutDependent` that would signal that we want to track the scroll animation and restart it if necessary.

## React Native API Proposal

### Prop Type Changes

```ts
// Proposed prop additions

type ScrollViewAnchorProps = {
    anchorEdge?: 'start' | 'end' | 'none',
    overflowAnchorMode?: 'explicit' | 'implicit' | 'none',
}

type ViewAnchorProps = {
    overflowAnchor?: 'auto' | 'none',
}

// Proposed ScrollView scrollToEnd change
let ref: RefObject<ScrollView>;
ref.scrollToEnd({
    animated?: boolean,
    layoutDependent?: boolean,
});

// Default scrollToEnd behavior
ref.scrollToEnd({
    animated: false,
    layoutDependent: false,
});
```

### Behavior Table

| **PROP** | **VALUE** | **DESCRIPTION** |
| --- | --- | --- |
| overflowAnchorMode | 'explicit' | Only descendants of ScrollViews with `overflowAnchor === 'auto'` are anchor candidates. |
| overflowAnchorMode | 'implicit' | All descendents of the ScrollView are anchor candidates unless they have set `overflowAnchor === 'none'`. |
| overflowAnchorMode | 'none' | View anchoring is not enabled. |
| overflowAnchorMode | null or undefined | Equivalent to 'none'. |
| overflowAnchor | 'auto' | View is an eligible anchor candidate when anchoring is enabled. |
| overflowAnchor | 'none' | View is not an anchor candidate when anchoring is enabled. |
| overflowAnchor | null or undefined | View is an anchor candidate if the first ScrollView ancestor has `overflowAnchorMode === 'implicit'`, otherwise view is not an anchor candidate. |
| anchorEdge | 'start' | ScrollView anchors to the flex start edge on the primary scrolling axis (i.e., left for horizontal ScrollView in LTR, right for horizontal in RTL, and top for vertical). |
| anchorEdge | 'end' | ScrollView anchors to the flex end edge on the primary scrolling axis (i.e., right for horizontal ScrollView in LTR, left for horizontal in RTL, and bottom for vertical). |
| anchorEdge | 'none' | ScrollView does not anchor to an edge. This is likely only relevant if you want a descendent view to be an anchor point while scrolled to the default anchor edge. |
| anchorEdge | null or undefined | Equivalent to 'start'. |

## What's next?

One way to take this proposal a step further would be to add functionality to VirtualizedList to choose different `inverted` implementations based on platform capabilities. We could add a view manager constant to ScrollView that signals whether "native" inversion features are available, and if so, use those features when inverting a VirtualizedList as opposed to the scale transform approach.
