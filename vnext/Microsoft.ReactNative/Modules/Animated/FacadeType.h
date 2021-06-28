// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <cassert>
#include <string>

enum class FacadeType {
  Opacity,
  Rotation,
  RotationX,
  RotationY,
  Scale,
  ScaleX,
  ScaleY,
  TranslateX,
  TranslateY,
  Perspective,
  Progress,

  // Indicates the unrecognized/unsupported facade
  None
};

inline FacadeType StringToFacadeType(const std::string &value) noexcept {
  if (value == "opacity")
    return FacadeType::Opacity;
  if (value == "rotate")
    return FacadeType::Rotation;
  if (value == "rotateX")
    return FacadeType::RotationX;
  if (value == "rotateY")
    return FacadeType::RotationY;
  if (value == "scale")
    return FacadeType::Scale;
  if (value == "scaleX")
    return FacadeType::ScaleX;
  if (value == "scaleY")
    return FacadeType::ScaleY;
  if (value == "translateX")
    return FacadeType::TranslateX;
  if (value == "translateY")
    return FacadeType::TranslateY;
  if (value == "perspective")
    return FacadeType::Perspective;
  if (value == "progress")
    return FacadeType::Progress;

  // None of the above facade has been recognized
  return FacadeType::None;
}
