#pragma once

#include "MartigliNode.h"
#include <audioapi/HostObjects/AudioNodeHostObject.h>

#include <memory>
#include <vector>
#include <cstdio>

namespace audioapi {
using namespace facebook;

class MartigliNodeHostObject : public AudioNodeHostObject {
public:
  explicit MartigliNodeHostObject(
      const std::shared_ptr<MartigliNode> &node)
      : AudioNodeHostObject(node) {
    printf("MartigliNodeHostObject: Creating MartigliNodeHostObject\n");
    
    // Add all property getters
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, mf0));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, ma));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, mp0));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, mp1));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, md));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, inhaleDur));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, exhaleDur));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, waveformM));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, volume));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, panOsc));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, panOscPeriod));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, panOscTrans));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, animationValue));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, isPaused));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, shouldStart));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, shouldPause));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, shouldResume));
    
    // Add all property setters
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, mf0));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, ma));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, mp0));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, mp1));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, md));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, inhaleDur));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, exhaleDur));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, waveformM));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, volume));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, panOsc));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, panOscPeriod));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, panOscTrans));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, shouldStart));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, shouldPause));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, shouldResume));
  }

  ~MartigliNodeHostObject() override {
      printf("MartigliNodeHostObject: Destroying MartigliNodeHostObject\n");
  }

  // Property getters
  JSI_PROPERTY_GETTER(mf0) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->mf0};
  }

  JSI_PROPERTY_GETTER(ma) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->ma};
  }

  JSI_PROPERTY_GETTER(mp0) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->mp0};
  }

  JSI_PROPERTY_GETTER(mp1) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->mp1};
  }

  JSI_PROPERTY_GETTER(md) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->md};
  }

  JSI_PROPERTY_GETTER(inhaleDur) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->inhaleDur};
  }

  JSI_PROPERTY_GETTER(exhaleDur) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->exhaleDur};
  }

  JSI_PROPERTY_GETTER(waveformM) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->waveformM};
  }

  JSI_PROPERTY_GETTER(volume) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->volume};
  }

  JSI_PROPERTY_GETTER(panOsc) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->panOsc};
  }

  JSI_PROPERTY_GETTER(panOscPeriod) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->panOscPeriod};
  }

  JSI_PROPERTY_GETTER(panOscTrans) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->panOscTrans};
  }

  JSI_PROPERTY_GETTER(animationValue) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->animationValue};
  }

  JSI_PROPERTY_GETTER(isPaused) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->isPaused};
  }

  JSI_PROPERTY_GETTER(shouldStart) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->shouldStart};
  }

  JSI_PROPERTY_GETTER(shouldPause) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->shouldPause};
  }

  JSI_PROPERTY_GETTER(shouldResume) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    return {martigliNode->shouldResume};
  }

  // Property setters
  JSI_PROPERTY_SETTER(mf0) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->mf0 = value.getNumber();
  }

  JSI_PROPERTY_SETTER(ma) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->ma = value.getNumber();
  }

  JSI_PROPERTY_SETTER(mp0) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->mp0 = value.getNumber();
  }

  JSI_PROPERTY_SETTER(mp1) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->mp1 = value.getNumber();
  }

  JSI_PROPERTY_SETTER(md) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->md = value.getNumber();
  }

  JSI_PROPERTY_SETTER(inhaleDur) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->inhaleDur = value.getNumber();
  }

  JSI_PROPERTY_SETTER(exhaleDur) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->exhaleDur = value.getNumber();
  }

  JSI_PROPERTY_SETTER(waveformM) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->waveformM = value.getNumber();
  }

  JSI_PROPERTY_SETTER(volume) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->volume = value.getNumber();
  }

  JSI_PROPERTY_SETTER(panOsc) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->panOsc = value.getNumber();
  }

  JSI_PROPERTY_SETTER(panOscPeriod) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->panOscPeriod = value.getNumber();
  }

  JSI_PROPERTY_SETTER(panOscTrans) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->panOscTrans = value.getNumber();
  }

  JSI_PROPERTY_SETTER(shouldStart) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->shouldStart = value.getBool();
  }

  JSI_PROPERTY_SETTER(shouldPause) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->shouldPause = value.getBool();
  }

  JSI_PROPERTY_SETTER(shouldResume) {
    auto martigliNode = std::static_pointer_cast<MartigliNode>(node_);
    martigliNode->shouldResume = value.getBool();
  }
};
} // namespace audioapi
