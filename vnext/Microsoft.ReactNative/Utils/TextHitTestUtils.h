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
  static winrt::TextPointer
  GetPositionFromPoint(const winrt::TextPointer &start, const winrt::TextPointer &end, const winrt::Point &targetPoint);
};
} // namespace Microsoft::ReactNative
