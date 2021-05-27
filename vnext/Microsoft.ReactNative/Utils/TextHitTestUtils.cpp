// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TextHitTestUtils.h"

#include <UI.Xaml.Documents.h>

namespace Microsoft::ReactNative {

struct BidirectionalTextBoundary {
  xaml::Documents::TextPointer ltrPointer{nullptr};
  xaml::Documents::TextPointer rtlPointer{nullptr};
  winrt::Rect ltrRect;
  winrt::Rect rtlRect;
};

/**
 * Finds the character closest to X = 0 for the first line. I.e., the
 * leftmost character when flow direction is left-to-right.
 * ┌─────> X ↓
 * │ (x,y)  ┌─<─┐<┌─>─┐
 * │ ┌──────┘   └─┘   │
 * │ │                │
 * ↓ │     ┌──────────┘
 * Y └─────┘
 */
static BidirectionalTextBoundary FindStartRTLBoundary(
    const xaml::Documents::TextPointer &start,
    const xaml::Documents::TextPointer &end,
    const winrt::Rect &target) {
  auto textPointer = start;
  auto L = start.Offset();
  auto R = end.Offset();
  while (L <= R) {
    const auto m = /* floor */ (L + R) / 2;
    const auto relativeOffset = m - textPointer.Offset();
    textPointer = textPointer.GetPositionAtOffset(relativeOffset, xaml::Documents::LogicalDirection::Forward);
    const auto rect = textPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    // If median character is on different line, boundary is before median
    if (rect.Y != target.Y) {
      R = m - 1;
    } else {
      const auto prevPointer = textPointer.GetPositionAtOffset(-1, xaml::Documents::LogicalDirection::Forward);
      const auto prevRect = prevPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
      if (prevRect.X > target.X) {
        // If previous character is right of start, boundary is before median
        R = m - 1;
      } else if (rect.X <= target.X) {
        // If median character is left of start, boundary is after median
        L = m + 1;
      } else {
        // Otherwise, we've found the boundary where the previous character is
        // the last RTL character and the median is the first LTR character
        BidirectionalTextBoundary result;
        result.rtlPointer = prevPointer;
        result.rtlRect = prevRect;
        result.ltrPointer = textPointer;
        result.ltrRect = rect;
        return result;
      }
    }
  }

  // No LTR characters on first line, R points to last character on first line
  auto lastRTL = start;
  auto lastRTLRect = target;
  if (R > start.Offset()) {
    const auto relativeOffset = R - start.Offset();
    lastRTL = start.GetPositionAtOffset(relativeOffset, xaml::Documents::LogicalDirection::Forward);
    lastRTLRect = lastRTL.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
  }

  BidirectionalTextBoundary result;
  result.rtlPointer = lastRTL;
  result.rtlRect = lastRTLRect;
  return result;
}

/**
 * Finds the character farthest from X = 0 for the last line. I.e., the
 * rightmost character when flow direction is left-to-right.
 * ┌─────> X
 * │ (x,y) ┌──────┐
 * │ ┌─────┘      │
 * │ │            │
 * ↓ │   ┌─┐   ┌──┘
 * Y └─>─┘<└─<─┘
 *            ↑
 */
static BidirectionalTextBoundary FindEndRTLBoundary(
    const xaml::Documents::TextPointer &start,
    const xaml::Documents::TextPointer &end,
    const winrt::Rect &target) {
  auto textPointer = start;
  auto L = start.Offset();
  auto R = end.Offset();
  while (L <= R) {
    const auto m = /* floor */ (L + R) / 2;
    const auto relativeOffset = m - textPointer.Offset();
    textPointer = textPointer.GetPositionAtOffset(relativeOffset, xaml::Documents::LogicalDirection::Forward);
    const auto rect = textPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    // If median character is on different line, boundary is after median
    if (rect.Y != target.Y) {
      L = m + 1;
    } else if (m < R) {
      const auto nextPointer = textPointer.GetPositionAtOffset(1, xaml::Documents::LogicalDirection::Forward);
      const auto nextRect = nextPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
      if (nextRect.X < target.X) {
        // If next character is left of end, boundary is after median
        L = m + 1;
      } else if (rect.X >= target.X) {
        // If median character is right of end, boundary is before median
        R = m - 1;
      } else {
        // Otherwise, we've found the boundary where the next character is the
        // first RTL character and the median is the last LTR character
        BidirectionalTextBoundary result;
        result.rtlPointer = nextPointer;
        result.rtlRect = nextRect;
        result.ltrPointer = textPointer;
        result.ltrRect = rect;
        return result;
      }
    } else {
      break;
    }
  }

  // No LTR characters on last line, L points to first character on last line
  auto lastRTL = end;
  auto lastRTLRect = target;
  if (L < end.Offset()) {
    const auto relativeOffset = L - end.Offset();
    lastRTL = end.GetPositionAtOffset(relativeOffset, xaml::Documents::LogicalDirection::Forward);
    lastRTLRect = lastRTL.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
  }

  BidirectionalTextBoundary result;
  result.rtlPointer = lastRTL;
  result.rtlRect = lastRTLRect;
  return result;
}

/**
 * Determine if the next character in the given direction on the same line is
 * RTL. If the next character is not on the same line, assume LTR. This may be
 * The case if the run has only one character on the target line.
 */
static bool IsRTL(const xaml::Documents::TextPointer &textPointer, const winrt::Rect &rect, int direction) {
  auto currentPointer = textPointer.GetPositionAtOffset(direction, xaml::Documents::LogicalDirection::Forward);
  while (currentPointer != nullptr) {
    const auto currentRect = currentPointer.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    if (currentRect.Y != rect.Y) {
      return false;
    } else if (currentRect.X != rect.X) {
      return direction > 0 ? currentRect.X < rect.X : currentRect.X > rect.X;
    }

    currentPointer = currentPointer.GetPositionAtOffset(direction, xaml::Documents::LogicalDirection::Forward);
  }

  return false;
}

/**
 * Determine if the character is either the first or last character of a line in the given direction.
 */
static std::optional<winrt::Rect>
GetBoundaryCharacter(const xaml::Documents::TextPointer &textPointer, const winrt::Rect &rect, int direction) {
  const auto nextPosition = textPointer.GetPositionAtOffset(direction, xaml::Documents::LogicalDirection::Forward);
  if (nextPosition) {
    const auto nextRect = nextPosition.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
    if (nextRect.Y == rect.Y) {
      return nextRect;
    }
  }

  return std::nullopt;
}

bool IsInBounds(const winrt::Point &point, float left, float right, float top, float bottom) {
  return left <= point.X && right >= point.X && top <= point.Y && bottom >= point.Y;
}

bool TextHitTestUtils::HitTest(const xaml::Documents::Run &run, const winrt::Point &targetPoint) {
  const auto start = run.ContentStart();
  const auto end = run.ContentEnd();
  const auto startRect = start.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
  const auto endRect = end.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);

  // Get first and last line vertical bounds
  const auto startTop = startRect.Y;
  const auto startBottom = startRect.Y + startRect.Height;
  const auto endTop = endRect.Y;
  const auto endBottom = endRect.Y + endRect.Height;

  // Check if we have a single line of unidirectional text
  const auto isSingleLine = startRect.Y == endRect.Y;
  const auto isStartRTL = IsRTL(start, startRect, 1);
  const auto isEndRTL = IsRTL(end, endRect, -1);
  const auto isUnidirectional = (!isStartRTL && !isEndRTL) || (isSingleLine && endRect.X < startRect.X);

  // For unidirectional runs on the same line, we can use a simple bounding box test
  if (isSingleLine && isUnidirectional) {
    return endRect.X < startRect.X
        ? IsInBounds(targetPoint, endRect.X + endRect.Width, startRect.X, startTop, startBottom)
        : IsInBounds(targetPoint, startRect.X, endRect.X + endRect.Width, startTop, startBottom);
  }

  // If we don't have a single line of unidirectional text, we need to search
  // up to 5 bounding boxes for the target point:
  // 1. The lines between the first and last lines
  // 2. The LTR characters on the first line after initial RTL
  // 3. The LTR characters on the last line before final RTL
  // 4. The initial RTL characters on the first line
  // 5. The final RTL characters on the last line
  //
  // We can optimize the following situations:
  // a. Skip step 2 if no LTR characters on the first line
  // b. Skip step 3 if no LTR characters on the last line
  // c. Skip step 4 if the first character is LTR
  // d. Skip step 5 if the last character is LTR

  // Get the parent bounds
  const auto maxLeft = 0;
  const auto maxRight = static_cast<float>(start.VisualParent().Width());

  // Check if run is either the start or end of a line
  const auto characterBeforeStart = GetBoundaryCharacter(start, startRect, -1);
  const auto characterAfterEnd = GetBoundaryCharacter(end, endRect, 1);

  // Find leftmost character of the first line and first LTR character
  BidirectionalTextBoundary startBoundary;
  startBoundary.ltrPointer = start;
  startBoundary.ltrRect = startRect;
  if (isStartRTL) {
    startBoundary = FindStartRTLBoundary(start, end, startRect);
  }

  // Find rightmost character of the last line and last LTR character
  BidirectionalTextBoundary endBoundary;
  endBoundary.ltrPointer = end;
  endBoundary.ltrRect = endRect;
  if (isEndRTL) {
    endBoundary = FindEndRTLBoundary(start, end, endRect);
  }

  // Step 1: The lines between the first and last lines
  const auto middleTop = startBottom;
  const auto middleBottom = endRect.Y;
  if (middleTop <= targetPoint.Y && middleBottom >= targetPoint.Y) {
    return true;
  }

  // Step 2: The LTR characters on the first line before initial RTL
  // Optimization b: Skip if no LTR characters in first line
  if (startBoundary.ltrPointer) {
    auto left = startBoundary.ltrRect.X;
    if (!characterBeforeStart.has_value()) {
      left = maxLeft;
    }

    auto right = maxRight;
    if (isSingleLine && characterAfterEnd.has_value()) {
      // TODO(#7792): last LTR character is ignored because width is always 0
      right = endBoundary.ltrRect.X + endBoundary.ltrRect.Width;
    }

    if (IsInBounds(targetPoint, left, right, startTop, startBottom)) {
      return true;
    }
  }

  // Step 3: The LTR characters on the last line after final RTL
  // Optimization b: Skip if no LTR characters in last line
  if (!isSingleLine && endBoundary.ltrPointer) {
    const auto left = 0;
    // TODO(#7792): last LTR character is ignored because width is always 0
    auto right = endBoundary.ltrRect.X + endBoundary.ltrRect.Width;
    if (!characterAfterEnd.has_value()) {
      right = maxRight;
    }

    if (IsInBounds(targetPoint, left, right, endTop, endBottom)) {
      return true;
    }
  }

  // Step 4: The initial RTL characters on the first line
  // Optimization c: Skip if first line starts with LTR
  if (isStartRTL) {
    // TODO(#7792): last RTL character is ignored because width is always 0
    auto left = startBoundary.rtlRect.X + startBoundary.rtlRect.Width;
    if (!characterBeforeStart.has_value()) {
      left = maxLeft;
    }

    auto right = maxRight;
    if (isSingleLine) {
      right = startRect.X;
    }

    if (IsInBounds(targetPoint, left, right, startTop, startBottom)) {
      return true;
    }
  }

  // Step 5: The final RTL characters on the last line
  // Optimization d: Skip if last line ends with LTR
  if (isEndRTL) {
    // TODO(#7792): last RTL character is ignored because width is always 0
    const auto left = endRect.X + endRect.Width;
    auto right = endBoundary.rtlRect.X;
    if (!characterAfterEnd.has_value()) {
      right = maxRight;
    }

    if (IsInBounds(targetPoint, left, right, endTop, endBottom)) {
      return true;
    }
  }

  return false;
}

} // namespace Microsoft::ReactNative
