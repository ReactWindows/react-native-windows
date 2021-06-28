// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Utils/Helpers.h>
#include <Views/ShadowNodeBase.h>
#include <Views/XamlFeatures.h>
#include "NativeAnimatedNodeManager.h"
#include "PropsAnimatedNode.h"
#include "StyleAnimatedNode.h"

namespace Microsoft::ReactNative {
PropsAnimatedNode::PropsAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const Mso::CntPtr<Mso::React::IReactContext> &context,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, manager), m_context(context) {
  for (const auto &entry : config.find("props").dereference().second.items()) {
    m_propMapping.insert({entry.first.getString(), static_cast<int64_t>(entry.second.asDouble())});
  }
  auto compositor = Microsoft::ReactNative::GetCompositor();
  m_subchannelPropertySet = compositor.CreatePropertySet();
  m_subchannelPropertySet.InsertScalar(L"TranslationX", 0.0f);
  m_subchannelPropertySet.InsertScalar(L"TranslationY", 0.0f);
  m_subchannelPropertySet.InsertScalar(L"ScaleX", 1.0f);
  m_subchannelPropertySet.InsertScalar(L"ScaleY", 1.0f);

  m_translationCombined =
      compositor.CreateExpressionAnimation(L"Vector3(subchannels.TranslationX, subchannels.TranslationY, 0.0)");
  m_translationCombined.SetReferenceParameter(L"subchannels", m_subchannelPropertySet);
  m_translationCombined.Target(L"Translation");

  m_scaleCombined = compositor.CreateExpressionAnimation(L"Vector3(subchannels.ScaleX, subchannels.ScaleY, 1.0)");
  m_scaleCombined.SetReferenceParameter(L"subchannels", m_subchannelPropertySet);
  m_scaleCombined.Target(L"Scale");
}

void PropsAnimatedNode::ConnectToView(int64_t viewTag) {
  if (m_connectedViewTag != s_connectedViewTagUnset) {
    throw new std::invalid_argument(
        "Animated node " + std::to_string(m_tag) + " has already been attached to a view already exists.");
    return;
  }
  m_connectedViewTag = viewTag;
  UpdateView();
}

void PropsAnimatedNode::DisconnectFromView(int64_t viewTag) {
  if (m_connectedViewTag != viewTag) {
    throw new std::invalid_argument(
        "Attempting to disconnect view that has not been connected with the given animated node.");
    return;
  }

  std::vector<int64_t> keys{};
  for (const auto &anim : m_expressionAnimations) {
    keys.push_back(anim.first);
  }
  for (const auto &key : keys) {
    DisposeCompletedAnimation(key);
  }

  if (m_centerPointAnimation) {
    if (const auto target = GetUIElement()) {
      target.StopAnimation(m_centerPointAnimation);
    }
    m_centerPointAnimation = nullptr;
  }

  m_connectedViewTag = s_connectedViewTagUnset;
  m_needsCenterPointAnimation = false;
}

void PropsAnimatedNode::RestoreDefaultValues() {}

void PropsAnimatedNode::UpdateView() {
  if (m_connectedViewTag == s_connectedViewTagUnset) {
    return;
  }

  if (const auto manager = std::shared_ptr<NativeAnimatedNodeManager>(m_manager)) {
    for (const auto &entry : m_propMapping) {
      if (const auto &styleNode = manager->GetStyleAnimatedNode(entry.second)) {
        for (const auto &styleEntry : styleNode->GetMapping()) {
          MakeAnimation(styleEntry.second, styleEntry.first);
        }
      } else if (const auto &valueNode = manager->GetValueAnimatedNode(entry.second)) {
        const auto &facade = StringToFacadeType(entry.first);
        if (facade != FacadeType::None) {
          MakeAnimation(entry.second, facade);
        }
      }
    }
  }

  StartAnimations();
}

static void EnsureUIElementDirtyForRender(xaml::UIElement uiElement) {
  auto compositeMode = uiElement.CompositeMode();
  switch (compositeMode) {
    case xaml::Media::ElementCompositeMode::SourceOver:
    case xaml::Media::ElementCompositeMode::MinBlend:
      uiElement.CompositeMode(xaml::Media::ElementCompositeMode::Inherit);
      break;
    default:
      uiElement.CompositeMode(xaml::Media::ElementCompositeMode::SourceOver);
      break;
  }
  uiElement.CompositeMode(compositeMode);
}

void PropsAnimatedNode::StartAnimations() {
  if (m_expressionAnimations.size()) {
    if (const auto uiElement = GetUIElement()) {
      // Work around for https://github.com/microsoft/microsoft-ui-xaml/issues/2511
      EnsureUIElementDirtyForRender(uiElement);
      uiElement.RotationAxis(m_rotationAxis);
      for (const auto &anim : m_expressionAnimations) {
        if (anim.second.Target() == L"Translation.X") {
          m_subchannelPropertySet.StartAnimation(L"TranslationX", anim.second);
          uiElement.StartAnimation(m_translationCombined);
        } else if (anim.second.Target() == L"Translation.Y") {
          m_subchannelPropertySet.StartAnimation(L"TranslationY", anim.second);
          uiElement.StartAnimation(m_translationCombined);
        } else if (anim.second.Target() == L"Scale.X") {
          m_subchannelPropertySet.StartAnimation(L"ScaleX", anim.second);
          uiElement.StartAnimation(m_scaleCombined);
        } else if (anim.second.Target() == L"Scale.Y") {
          m_subchannelPropertySet.StartAnimation(L"ScaleY", anim.second);
          uiElement.StartAnimation(m_scaleCombined);
        } else {
          uiElement.StartAnimation(anim.second);
        }
      }
      if (m_needsCenterPointAnimation) {
        if (!m_centerPointAnimation) {
          m_centerPointAnimation = Microsoft::ReactNative::GetCompositor().CreateExpressionAnimation();
          m_centerPointAnimation.Target(L"CenterPoint");
          m_centerPointAnimation.SetReferenceParameter(
              L"centerPointPropertySet", GetShadowNodeBase()->EnsureTransformPS());
          m_centerPointAnimation.Expression(L"centerPointPropertySet.center");
        }

        uiElement.StartAnimation(m_centerPointAnimation);
      }
    } else {
      if (const auto manager = m_manager.lock()) {
        manager->AddDelayedPropsNode(Tag(), m_context);
      }
    }
  }
}

void PropsAnimatedNode::DisposeCompletedAnimation(int64_t valueTag) {
  /*
  if (m_expressionAnimations.count(valueTag)) {
    if (const auto target = GetUIElement()) {
      // We should start and stop the expression animations if there are
      // no active animations. Suspending the active expression animations
      // while they are not in use causes subsequent key frame animations
      // which target the providing property set to never fire their completed
      // events. I can't explain this and for now and commenting out the code.
      // Fixing this will prevent memory bloat as the current solutions never
      // stops the expression animations that are built as a part of this
      // animation solution. Tracked by issue #3280.
      target.StopAnimation(m_expressionAnimations.at(valueTag));
      m_suspendedExpressionAnimationTags.push_back(valueTag);
    }
  }
  */
}

void PropsAnimatedNode::ResumeSuspendedAnimations(int64_t valueTag) {
  /*
  const auto iterator =
      std::find(m_suspendedExpressionAnimationTags.begin(), m_suspendedExpressionAnimationTags.end(), valueTag);
  if (iterator != m_suspendedExpressionAnimationTags.end()) {
    if (const auto target = GetUIElement()) {
      // See comment above, tracked by issue #3280
      target.StartAnimation(m_expressionAnimations.at(valueTag));
      m_suspendedExpressionAnimationTags.erase(iterator);
    }
  }
  */
}

void PropsAnimatedNode::MakeAnimation(int64_t valueNodeTag, FacadeType facadeType) {
  if (const auto manager = m_manager.lock()) {
    if (const auto valueNode = manager->GetValueAnimatedNode(valueNodeTag)) {
      const auto animation = Microsoft::ReactNative::GetCompositor().CreateExpressionAnimation();
      animation.SetReferenceParameter(L"ValuePropSet", valueNode->PropertySet());
      animation.Expression(
          static_cast<winrt::hstring>(L"ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
          ValueAnimatedNode::s_offsetName);
      switch (facadeType) {
        case FacadeType::Opacity:
          animation.Target(L"Opacity");
          break;
        case FacadeType::Rotation:
          m_rotationAxis = {0, 0, 1};
          animation.Expression(
              static_cast<winrt::hstring>(L"(ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
              ValueAnimatedNode::s_offsetName + L") * 180 / PI");
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::RotationX:
          animation.Expression(
              static_cast<winrt::hstring>(L"(ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
              ValueAnimatedNode::s_offsetName + L") * 180 / PI");
          m_rotationAxis = {1, 0, 0};
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::RotationY:
          animation.Expression(
              static_cast<winrt::hstring>(L"(ValuePropSet.") + ValueAnimatedNode::s_valueName + L" + ValuePropSet." +
              ValueAnimatedNode::s_offsetName + L") * 180 / PI");
          m_rotationAxis = {0, 1, 0};
          animation.Target(L"Rotation");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::Scale:
          animation.Expression(
              static_cast<winrt::hstring>(L"Vector3(ValuePropSet.") + ValueAnimatedNode::s_valueName +
              L" + ValuePropSet." + ValueAnimatedNode::s_offsetName + L", ValuePropSet." +
              ValueAnimatedNode::s_valueName + L" + ValuePropSet." + ValueAnimatedNode::s_offsetName + L", 0)");
          animation.Target(L"Scale");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::ScaleX:
          animation.Target(L"Scale.X");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::ScaleY:
          animation.Target(L"Scale.Y");
          m_needsCenterPointAnimation = true;
          break;
        case FacadeType::TranslateX:
          animation.Target(L"Translation.X");
          break;
        case FacadeType::TranslateY:
          animation.Target(L"Translation.Y");
          break;
        case FacadeType::Perspective:
          // TODO: implement perspective animations, tracked by issue #2680
          return;
        case FacadeType::Progress:
          // TODO: implement progress animations, tracked by issue #3283
          return;
        default:
          assert(false);
      }
      m_expressionAnimations.insert({valueNode->Tag(), animation});
      valueNode->AddDependentPropsNode(Tag());
    }
  }
}

Microsoft::ReactNative::ShadowNodeBase *PropsAnimatedNode::GetShadowNodeBase() {
  if (const auto uiManager = Microsoft::ReactNative::GetNativeUIManager(*m_context).lock()) {
    if (const auto nativeUIManagerHost = uiManager->getHost()) {
      return static_cast<Microsoft::ReactNative::ShadowNodeBase *>(
          nativeUIManagerHost->FindShadowNodeForTag(m_connectedViewTag));
    }
  }
  return nullptr;
}

xaml::UIElement PropsAnimatedNode::GetUIElement() {
  if (IsRS5OrHigher()) {
    if (const auto shadowNodeBase = GetShadowNodeBase()) {
      if (const auto shadowNodeView = shadowNodeBase->GetView()) {
        return shadowNodeView.as<xaml::UIElement>();
      }
    }
  }
  return nullptr;
}
} // namespace Microsoft::ReactNative
