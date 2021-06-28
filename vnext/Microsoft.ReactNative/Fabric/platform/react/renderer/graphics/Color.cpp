// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Color.h"
#include <Utils/ValueUtils.h>

namespace facebook {
namespace react {

bool isColorMeaningful(SharedColor const &color) noexcept {
  if (!color) {
    return false;
  }

  return colorComponentsFromColor(color).alpha > 0;
}

xaml::Media::Brush SharedColor::AsWindowsBrush() const {
  if (!m_color)
    return nullptr;
  if (!m_color->m_platformColor.empty()) {
    return Microsoft::ReactNative::BrushFromColorObject(winrt::to_hstring(m_color->m_platformColor));
  }
  return xaml::Media::SolidColorBrush(m_color->m_color);
}

SharedColor colorFromComponents(ColorComponents components) {
  float ratio = 255;
  return SharedColor(ui::ColorHelper::FromArgb(
      (int)round(components.alpha * ratio) & 0xff,
      (int)round(components.red * ratio) & 0xff,
      (int)round(components.green * ratio) & 0xff,
      (int)round(components.blue * ratio) & 0xff));
}

ColorComponents colorComponentsFromColor(SharedColor sharedColor) {
  float ratio = 255;
  auto color = sharedColor.AsWindowsColor();
  return ColorComponents{
      (float)color.R / ratio, (float)color.G / ratio, (float)color.B / ratio, (float)color.A / ratio};
}

SharedColor clearColor() {
  static SharedColor color = colorFromComponents(ColorComponents{0, 0, 0, 0});
  return color;
}

SharedColor blackColor() {
  static SharedColor color = colorFromComponents(ColorComponents{0, 0, 0, 1});
  return color;
}

SharedColor whiteColor() {
  static SharedColor color = colorFromComponents(ColorComponents{1, 1, 1, 1});
  return color;
}

} // namespace react
} // namespace facebook
