// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextHitTestUtils.h"

namespace Microsoft::ReactNative {

bool IsPointAfterCharacter(
    const winrt::Point &point,
    const winrt::TextPointer &textPointer,
    winrt::Rect rect,
    double width,
    bool isRtl) {
  const auto bottom = rect.Y + rect.Height;

  auto right = width;
  if (!isRtl) {
    // The character Rect always has Width = 0, so we use the X-dimension of
    // the next character on the same line. If the next character is not on the
    // same line, we use the rightmost boundary of the TextBlock.
    //
    // The side-effect is that LTR text may have additional hit box space at
    // the end of a line, but it's better than the alternative of the hit box
    // excluding the last character on a line.
    const auto nextPointer = textPointer.GetPositionAtOffset(1, winrt::LogicalDirection::Forward);
    if (nextPointer != nullptr) {
      const auto nextRect = nextPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
      if (rect.Y == nextRect.Y) {
        right = nextRect.X;
      }
    }
  }

  // The character is before the point if the Y-coordinate of the point is
  // below (greater than) the bottom of the character rect, or if the
  // Y-coordinate is below (greater than) the top of the character rect and
  // the X-coordinate is "after" the end of the character rect (greater than
  // the right side for LTR or less than the left for RTL):
  // ┌────> X
  // │ ┌───────────┐  ┌───────────┐
  // ↓ │     ┌─────┘  └─────┐     │
  // Y └─────┘ (x,y)  (x,y) └─────┘
  //        LTR            RTL
  const auto isAfterX = !isRtl ? point.X > right : point.X < rect.X;
  return point.Y > bottom || (point.Y > rect.Y && isAfterX);
}

bool IsPointBeforeCharacter(
    const winrt::Point &point,
    const winrt::TextPointer &textPointer,
    winrt::Rect rect,
    double width,
    bool isRtl) {
  const auto bottom = rect.Y + rect.Height;

  auto right = width;
  if (isRtl) {
    // The character Rect always has Width = 0, so we use the X-dimension of
    // the previous character on the same line. If the previous character is
    // not on the same line, we use the rightmost boundary of the TextBlock.
    //
    // The side-effect is that RTL text may have additional valid hit box
    // space at the end of a line, but it's better than the alternative of the
    // hit box excluding the last character on a line.
    const auto prevPointer = textPointer.GetPositionAtOffset(-1, winrt::LogicalDirection::Forward);
    if (prevPointer != nullptr) {
      const auto prevRect = prevPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
      if (rect.Y == prevRect.Y) {
        right = prevRect.X;
      }
    }
  }

  // The character is after the point if the Y-coordinate of the point is above
  // (less than) the top of the character rect, or if the Y-coordinate is above
  // (less than) the bottom of the character rect and the X-coordinate is
  // "before" the beginning of the character rect (less than the left side for
  // LTR or greater than the right for RTL):
  // ┌────> X
  // │ (x,y) ┌─────┐  ┌─────┐ (x,y)
  // ↓ ┌─────┘     │  │     └─────┐
  // Y └───────────┘  └───────────┘
  //        LTR            RTL
  const auto isBeforeX = !isRtl ? point.X < rect.X : point.X > right;
  return point.Y < rect.Y || (point.Y < bottom && isBeforeX);
}

bool IsRTL(const winrt::TextPointer &textPointer) {
  auto currentPointer = textPointer.GetPositionAtOffset(1, winrt::LogicalDirection::Forward);
  auto currentRect = textPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
  auto firstCharacterRect = currentRect;
  while (currentPointer != nullptr) {
    currentRect = currentPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
    // If we haven't figured out the direction by the time we reach the end of
    // a line, try again for the next line.
    if (currentRect.Y != firstCharacterRect.Y) {
      firstCharacterRect = currentRect;
    } else if (currentRect.X != firstCharacterRect.X) {
      return currentRect.X < firstCharacterRect.X;
    }

    currentPointer = currentPointer.GetPositionAtOffset(1, winrt::LogicalDirection::Forward);
  }

  // Assume LTR if there are not enough characters to determine LTR vs. RTL.
  // This may occur for TextBlocks with a single character per line.
  return false;
}

winrt::TextPointer TextHitTestUtils::GetPositionFromPoint(
    const winrt::TextBlock &textBlock,
    const winrt::Point &targetPoint) {
  // Since characters in a TextBlock are sorted from top-left to bottom-right
  // (or top-right to bottom-left for RTL), we can use binary search to find
  // the character with bounds that container the pointer point.
  //
  // This algorithm currently makes the following assumptions:
  // 1. Characters on the same line have the same Rect::Y value
  const auto width = textBlock.Width();
  auto textPointer = textBlock.ContentStart();
  const auto isRtl = IsRTL(textPointer);
  auto L = 0;
  auto R = /* n - 1 */ textBlock.ContentEnd().Offset();
  while (L <= R) {
    const auto m = /* floor */ (L + R) / 2;
    const auto relativeOffset = m - textPointer.Offset();
    textPointer = textPointer.GetPositionAtOffset(relativeOffset, winrt::LogicalDirection::Forward);
    const auto rect = textPointer.GetCharacterRect(winrt::LogicalDirection::Forward);
    if (IsPointAfterCharacter(targetPoint, textPointer, rect, width, isRtl) /* A[m] < T */) {
      L = m + 1;
    } else if (IsPointBeforeCharacter(targetPoint, textPointer, rect, width, isRtl) /* A[m] > T */) {
      R = m - 1;
    } else {
      return textPointer;
    }
  }

  return nullptr;
}

} // namespace Microsoft::ReactNative
