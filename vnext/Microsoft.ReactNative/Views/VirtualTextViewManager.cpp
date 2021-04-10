// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RawTextViewManager.h"
#include "TextViewManager.h"
#include "VirtualTextViewManager.h"

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <Utils/PropertyUtils.h>
#include <Utils/TextHitTestUtils.h>
#include <Utils/TransformableText.h>
#include <Utils/ValueUtils.h>

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Documents;
} // namespace winrt

namespace Microsoft::ReactNative {

void VirtualTextShadowNode::AddView(ShadowNode &child, int64_t index) {
  auto &childNode = static_cast<ShadowNodeBase &>(child);
  ApplyTextTransform(childNode, textTransform, /* forceUpdate = */ false, /* isRoot = */ false);
  if (auto span = childNode.GetView().try_as<xaml::Documents::Span>()) {
    auto &childVTSN = static_cast<VirtualTextShadowNode &>(child);
    m_highlightData.data.emplace_back(childVTSN.m_highlightData);
    AddToPressableCount(childVTSN.m_pressableCount);
  }
  Super::AddView(child, index);
}

void VirtualTextShadowNode::onDropViewInstance() {
  AddToPressableCount(-m_pressableCount);
  Super::onDropViewInstance();
}

void VirtualTextShadowNode::AddToPressableCount(int count) {
  m_pressableCount += count;
  if (const auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
    if (m_parent != -1) {
      const auto parentNode = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(m_parent));
      const auto viewManager = parentNode->GetViewManager();
      if (!std::wcscmp(viewManager->GetName(), L"RCTText")) {
        static_cast<TextViewManager *>(viewManager)->AddToPressableCount(parentNode, count);
      } else if (!std::wcscmp(parentNode->GetViewManager()->GetName(), L"RCTVirtualText")) {
        static_cast<VirtualTextShadowNode *>(parentNode)->AddToPressableCount(count);
      }
    }
  }
}

void VirtualTextShadowNode::SetPressable(bool isPressable) {
  const auto wasPressable = m_isPressable;
  m_isPressable = isPressable;
  if (!wasPressable && isPressable) {
    AddToPressableCount(1);
  } else if (wasPressable && !isPressable) {
    AddToPressableCount(-1);
  }
}

void VirtualTextShadowNode::ApplyTextTransform(
    ShadowNodeBase &node,
    TextTransform transform,
    bool forceUpdate,
    bool isRoot) {
  // The `forceUpdate` option is used to force the tree to update, even if the
  // transform value is undefined or set to 'none'. This is used when a leaf
  // raw text value has changed, or a textTransform prop has changed.
  if (forceUpdate || (transform != TextTransform::Undefined && transform != TextTransform::None)) {
    // Use the view manager name to determine the node type
    const auto viewManager = node.GetViewManager();
    const auto nodeType = viewManager->GetName();

    // Base case: apply the inherited textTransform to the raw text node
    if (!std::wcscmp(nodeType, L"RCTRawText")) {
      auto &rawTextNode = static_cast<RawTextShadowNode &>(node);
      auto originalText = rawTextNode.originalText;
      auto run = node.GetView().try_as<winrt::Run>();
      // Set originalText on the raw text node if it hasn't been set yet
      if (originalText.size() == 0) {
        // Lazily setting original text to avoid keeping two copies of all raw text strings
        originalText = run.Text();
        rawTextNode.originalText = originalText;
      }

      run.Text(TransformableText::TransformText(originalText, transform));

      if (!std::wcscmp(originalText.c_str(), run.Text().c_str())) {
        // If the transformed text is the same as the original, we no longer need a second copy
        rawTextNode.originalText = winrt::hstring{};
      }
    } else {
      // Recursively apply the textTransform to the children of the composite text node
      if (!std::wcscmp(nodeType, L"RCTVirtualText")) {
        auto &virtualTextNode = static_cast<VirtualTextShadowNode &>(node);
        // If this is not the root call, we can skip sub-trees with explicit textTransform settings.
        if (!isRoot && virtualTextNode.textTransform != TextTransform::Undefined) {
          return;
        }
      }

      if (auto uiManager = GetNativeUIManager(viewManager->GetReactContext()).lock()) {
        for (auto childTag : node.m_children) {
          const auto childNode = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(childTag));
          ApplyTextTransform(*childNode, transform, forceUpdate, /* isRoot = */ false);
        }
      }
    }
  }
}

xaml::Documents::TextPointer
VirtualTextShadowNode::HitTest(const ShadowNodeBase &node, const winrt::Point &point, bool hasPressableParent) {
  const auto viewManager = node.GetViewManager();
  const auto nodeType = viewManager->GetName();
  if (!std::wcscmp(nodeType, L"RCTRawText")) {
    // Check if the point is within the bounds of the Run
    const auto run = node.GetView().as<winrt::Run>();
    return TextHitTestUtils::GetPositionFromPoint(run, point);
  } else {
    const auto isVirtualText = !std::wcscmp(nodeType, L"RCTVirtualText");
    auto isPressable = hasPressableParent;
    if (isVirtualText) {
      const auto &virtualTextNode = static_cast<const VirtualTextShadowNode &>(node);
      if (virtualTextNode.m_isPressable) {
        isPressable = true;
      }

      // If the node is a nested Text component, skip if it has no pressable
      // descendants and it is not contained inside pressable text.
      if (!isPressable && virtualTextNode.m_pressableCount == 0) {
        return nullptr;
      }
    }

    if (auto uiManager = GetNativeUIManager(viewManager->GetReactContext()).lock()) {
      for (const auto childTag : node.m_children) {
        const auto childNode = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(childTag));
        const auto textPointer = HitTest(*childNode, point, isPressable);
        if (textPointer != nullptr) {
          return textPointer;
        }
      }
    }
  }

  return nullptr;
}

VirtualTextViewManager::VirtualTextViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *VirtualTextViewManager::GetName() const {
  return L"RCTVirtualText";
}

XamlView VirtualTextViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  return winrt::Span();
}

bool VirtualTextViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto span = nodeToUpdate->GetView().as<winrt::Span>();
  if (span == nullptr)
    return true;

  // FUTURE: In the future cppwinrt will generate code where static methods on
  // base types can be called.  For now we specify the base type explicitly
  if (TryUpdateForeground<winrt::TextElement>(span, propertyName, propertyValue)) {
    static_cast<VirtualTextShadowNode *>(nodeToUpdate)->m_highlightData.foregroundColor = ColorFrom(propertyValue);
  } else if (TryUpdateFontProperties<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateCharacterSpacing<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (TryUpdateTextDecorationLine<winrt::TextElement>(span, propertyName, propertyValue)) {
  } else if (propertyName == "textTransform") {
    auto node = static_cast<VirtualTextShadowNode *>(nodeToUpdate);
    node->textTransform = TransformableText::GetTextTransform(propertyValue);
    VirtualTextShadowNode::ApplyTextTransform(
        *node, node->textTransform, /* forceUpdate = */ true, /* isRoot = */ true);
  } else if (propertyName == "backgroundColor") {
    if (IsValidColorValue(propertyValue)) {
      static_cast<VirtualTextShadowNode *>(nodeToUpdate)->m_highlightData.backgroundColor = ColorFrom(propertyValue);
    }
  } else if (propertyName == "isPressable") {
    static_cast<VirtualTextShadowNode *>(nodeToUpdate)->SetPressable(propertyValue.AsBoolean());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }

  return true;
}

void VirtualTextViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  auto span(parent.as<winrt::Span>());
  auto childInline(child.as<winrt::Inline>());
  span.Inlines().InsertAt(static_cast<uint32_t>(index), childInline);
}

void VirtualTextViewManager::RemoveAllChildren(const XamlView &parent) {
  auto span(parent.as<winrt::Span>());
  span.Inlines().Clear();
}

void VirtualTextViewManager::RemoveChildAt(const XamlView &parent, int64_t index) {
  auto span(parent.as<winrt::Span>());
  return span.Inlines().RemoveAt(static_cast<uint32_t>(index));
}

bool VirtualTextViewManager::RequiresYogaNode() const {
  return false;
}

} // namespace Microsoft::ReactNative
