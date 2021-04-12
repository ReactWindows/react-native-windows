// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextHitTestUtils.h"

#include <UI.Xaml.Documents.h>

namespace Microsoft::ReactNative {

static bool IsPointAfterCharacter(
    const winrt::Point &point,
    const xaml::Documents::TextPointer &textPointer,
    const winrt::Rect &rect,
    bool isRtl,
    double endOfLine) {
  // The point is after the character if below the bottom of the character or
  // if on the same line and "after" the end of the character (greater than
  // the X-dimension of the next character rect in LTR, less than for RTL):
  // ┌────> X
  // │ ┌───────────┐  ┌───────────┐
  // ↓ │     ┌─────┘  └─────┐     │
  // Y └─────┘ (x,y)  (x,y) └─────┘
  //        LTR            RTL
  if (point.Y < rect.Y) {
    // Point is above the top of the character.
    return false;
  }

  const auto bottom = rect.Y + rect.Height;
  if (point.Y > bottom) {
    // Point is below the bottom of the character.
    return true;
  }

  auto end = endOfLine;
  const auto endPointer = textPointer.GetPositionAtOffset(1, xaml::Documents::LogicalDirection::Forward);
  if (endPointer != nullptr) {
    const auto endRect = endPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    if (rect.Y == endRect.Y) {
      end = endRect.X;
    }
  }

  return !isRtl ? point.X >= end : point.X <= end;
}

static bool IsPointBeforeCharacter(
    const winrt::Point &point,
    const xaml::Documents::TextPointer &textPointer,
    winrt::Rect rect,
    bool isRtl) {
  // The point is before the character if above the top of the character or if
  // on the same line and "before" the start of the character (less than the
  // X-dimension of the character rect in LTR, greater than for RTL):
  // ┌────> X
  // │ (x,y) ┌─────┐  ┌─────┐ (x,y)
  // ↓ ┌─────┘     │  │     └─────┐
  // Y └───────────┘  └───────────┘
  //        LTR            RTL
  if (point.Y < rect.Y) {
    // Point is above the top of the character.
    return true;
  }

  const auto bottom = rect.Y + rect.Height;
  if (point.Y > bottom) {
    // Point is below the bottom of the character.
    return false;
  }

  // The point is on the same line, check if the point is before the start of
  // the character.
  return !isRtl ? point.X < rect.X : point.X > rect.X;
}

static bool IsRTL(const xaml::Documents::TextPointer &textPointer) {
  auto currentPointer = textPointer.GetPositionAtOffset(1, xaml::Documents::LogicalDirection::Forward);
  auto currentRect = textPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
  auto firstCharacterRect = currentRect;
  while (currentPointer != nullptr) {
    currentRect = currentPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    // If we haven't figured out the direction by the time we reach the end of
    // a line, try again for the next line.
    if (currentRect.Y != firstCharacterRect.Y) {
      firstCharacterRect = currentRect;
    } else if (currentRect.X != firstCharacterRect.X) {
      return currentRect.X < firstCharacterRect.X;
    }

    currentPointer = currentPointer.GetPositionAtOffset(1, xaml::Documents::LogicalDirection::Forward);
  }

  // Assume LTR if there are not enough characters to determine LTR vs. RTL.
  // This may occur for TextBlocks with a single character per line.
  return false;
}

static xaml::Documents::TextPointer GetPositionFromPointCore(
    const xaml::Documents::TextPointer &start,
    const xaml::Documents::TextPointer &end,
    const winrt::Point &targetPoint) {
  // Since characters in a TextBlock are sorted from top-left to bottom-right
  // (or top-right to bottom-left for RTL), we can use binary search to find
  // the character with bounds that container the pointer point.
  //
  // This algorithm currently makes the following assumptions:
  // 1. Characters on the same line have the same Rect::Y value
  // 2. Search space is over only LTR or only RTL characters
  const auto isRtl = IsRTL(start);
  const auto endOfLine = !isRtl ? start.VisualParent().Width() : 0.0;
  auto textPointer = start;
  auto L = start.Offset();
  auto R = end.Offset();
  while (L <= R) {
    const auto m = /* floor */ (L + R) / 2;
    const auto relativeOffset = m - textPointer.Offset();
    textPointer = textPointer.GetPositionAtOffset(relativeOffset, xaml::Documents::LogicalDirection::Forward);
    const auto rect = textPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    if (IsPointAfterCharacter(targetPoint, textPointer, rect, isRtl, endOfLine) /* A[m] < T */) {
      L = m + 1;
    } else if (IsPointBeforeCharacter(targetPoint, textPointer, rect, isRtl) /* A[m] > T */) {
      R = m - 1;
    } else {
      return textPointer;
    }
  }

  return nullptr;
}

xaml::Documents::TextPointer TextHitTestUtils::GetPositionFromPoint(
    const xaml::Controls::TextBlock &textBlock,
    const winrt::Point &targetPoint) {
  return GetPositionFromPointCore(textBlock.ContentStart(), textBlock.ContentEnd(), targetPoint);
}

xaml::Documents::TextPointer TextHitTestUtils::GetPositionFromPoint(
    const xaml::Documents::Run &run,
    const winrt::Point &targetPoint) {
  const auto start = run.ContentStart();
  const auto end = run.ContentEnd();

  auto startRect = start.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
  auto endRect = end.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);

  // For runs on the same line, we can use a simple bounding box test
  if (startRect.Y == endRect.Y) {
    // Swap rectangles in RTL scenarios.
    if (startRect.X > endRect.X) {
      const auto tempRect = startRect;
      startRect = endRect;
      endRect = tempRect;
    }

    if ((startRect.X <= targetPoint.X) && (endRect.X + endRect.Width >= targetPoint.X) &&
        (startRect.Y <= targetPoint.Y) && (endRect.Y + endRect.Height >= targetPoint.Y)) {
      return start;
    }

    return nullptr;
  }

  return GetPositionFromPointCore(start, end, targetPoint);
}

} // namespace Microsoft::ReactNative
