// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>

namespace winrt {
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Documents;
} // namespace winrt

namespace Microsoft::ReactNative {
struct TextHitTestUtils final {
  static winrt::TextPointer GetPositionFromPoint(const winrt::TextBlock &textBlock, const winrt::Point &targetPoint);
  static winrt::TextPointer GetPositionFromPoint(const winrt::Run &run, const winrt::Point &targetPoint);
};
} // namespace Microsoft::ReactNative
