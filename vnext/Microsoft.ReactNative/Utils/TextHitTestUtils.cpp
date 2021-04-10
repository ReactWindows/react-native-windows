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
    bool isRtl) {
  const auto bottom = rect.Y + rect.Height;

  auto right = rect.X + rect.Width;
  if (!isRtl) {
    // The character Rect always has Width = 0, so we use the X-dimension of
    // the next character on the same line. If the next character is not on the
    // same line, use the right dimension of the rect. This can lead to hit box
    // test failures in the first character after a line break.
    const auto nextPointer = textPointer.GetPositionAtOffset(1, xaml::Documents::LogicalDirection::Forward);
    if (nextPointer != nullptr) {
      const auto nextRect = nextPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
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

static bool IsPointBeforeCharacter(
    const winrt::Point &point,
    const xaml::Documents::TextPointer &textPointer,
    winrt::Rect rect,
    bool isRtl) {
  const auto bottom = rect.Y + rect.Height;

  auto right = rect.X + rect.Width;
  if (isRtl) {
    // The character Rect always has Width = 0, so we use the X-dimension of
    // the previous character on the same line. If the previous character is
    // not on the same line, use the right dimension of the rect. This can lead
    // to hit box test failures in the first character after a line break.
    const auto prevPointer = textPointer.GetPositionAtOffset(-1, xaml::Documents::LogicalDirection::Forward);
    if (prevPointer != nullptr) {
      const auto prevRect = prevPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
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
  auto textPointer = start;
  auto L = start.Offset();
  auto R = end.Offset();
  while (L <= R) {
    const auto m = /* floor */ (L + R) / 2;
    const auto relativeOffset = m - textPointer.Offset();
    textPointer = textPointer.GetPositionAtOffset(relativeOffset, xaml::Documents::LogicalDirection::Forward);
    const auto rect = textPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    if (IsPointAfterCharacter(targetPoint, textPointer, rect, isRtl) /* A[m] < T */) {
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
