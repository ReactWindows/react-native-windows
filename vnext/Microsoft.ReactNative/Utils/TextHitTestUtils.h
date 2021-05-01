// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>

namespace Microsoft::ReactNative {
struct TextHitTestUtils final {
  static bool HitTest(const xaml::Documents::Run &run, const winrt::Point &targetPoint);
};
} // namespace Microsoft::ReactNative
