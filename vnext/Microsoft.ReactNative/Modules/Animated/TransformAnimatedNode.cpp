// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "FacadeType.h"
#include "TransformAnimatedNode.h"

namespace Microsoft::ReactNative {
TransformAnimatedNode::TransformAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, manager) {
  for (const auto &transform : config.find(s_transformsName).dereference().second) {
    const auto property = transform.find(s_propertyName).dereference().second.asString();
    if (transform.find(s_typeName).dereference().second.asString() == s_animatedName) {
      m_transformConfigs.push_back(TransformConfig{
          property, static_cast<int64_t>(transform.find(s_nodeTagName).dereference().second.asDouble()), 0});
    } else {
      m_transformConfigs.push_back(
          TransformConfig{property, s_unsetNodeTag, transform.find(s_valueName).dereference().second.asDouble()});
    }
  }
}

std::unordered_map<FacadeType, int64_t> TransformAnimatedNode::GetMapping() {
  std::unordered_map<FacadeType, int64_t> mapping;
  for (const auto &config : m_transformConfigs) {
    if (config.nodeTag != s_unsetNodeTag) {
      const auto &facade = StringToFacadeType(config.property);
      if (facade != FacadeType::None) {
        mapping.insert({facade, config.nodeTag});
      }
    }
  }
  return mapping;
}
} // namespace Microsoft::ReactNative
