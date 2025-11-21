#pragma once

#include "MartigliNode.h"
#include <audioapi/HostObjects/AudioNodeHostObject.h>
#include <memory>

namespace audioapi {
using namespace facebook;

// Macro to reduce boilerplate for property getters/setters
#define MARTIGLI_PROPERTY(type, name) \
  JSI_PROPERTY_GETTER(name) { \
    return {std::static_pointer_cast<MartigliNode>(node_)->name}; \
  } \
  JSI_PROPERTY_SETTER(name) { \
    std::static_pointer_cast<MartigliNode>(node_)->name = value.get##type(); \
  }

class MartigliNodeHostObject : public AudioNodeHostObject {
public:
  explicit MartigliNodeHostObject(const std::shared_ptr<MartigliNode> &node)
      : AudioNodeHostObject(node) {
    // Add getters
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
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, isOn));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, currentInhaleDur));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, currentExhaleDur));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, currentPeriod));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, shouldStart));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, shouldPause));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, shouldResume));
    addGetters(JSI_EXPORT_PROPERTY_GETTER(MartigliNodeHostObject, shouldStop));
    
    // Add setters
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
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, isOn));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, shouldStart));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, shouldPause));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, shouldResume));
    addSetters(JSI_EXPORT_PROPERTY_SETTER(MartigliNodeHostObject, shouldStop));
  }

  MARTIGLI_PROPERTY(Number, mf0)
  MARTIGLI_PROPERTY(Number, ma)
  MARTIGLI_PROPERTY(Number, mp0)
  MARTIGLI_PROPERTY(Number, mp1)
  MARTIGLI_PROPERTY(Number, md)
  MARTIGLI_PROPERTY(Number, inhaleDur)
  MARTIGLI_PROPERTY(Number, exhaleDur)
  MARTIGLI_PROPERTY(Number, waveformM)
  MARTIGLI_PROPERTY(Number, volume)
  MARTIGLI_PROPERTY(Number, panOsc)
  MARTIGLI_PROPERTY(Number, panOscPeriod)
  MARTIGLI_PROPERTY(Number, panOscTrans)
  MARTIGLI_PROPERTY(Bool, isOn)
  MARTIGLI_PROPERTY(Bool, shouldStart)
  MARTIGLI_PROPERTY(Bool, shouldPause)
  MARTIGLI_PROPERTY(Bool, shouldResume)
  MARTIGLI_PROPERTY(Bool, shouldStop)
  
  JSI_PROPERTY_GETTER(animationValue) {
    return {std::static_pointer_cast<MartigliNode>(node_)->animationValue};
  }
  
  JSI_PROPERTY_GETTER(isPaused) {
    return {std::static_pointer_cast<MartigliNode>(node_)->isPaused};
  }
  
  JSI_PROPERTY_GETTER(currentInhaleDur) {
    return {std::static_pointer_cast<MartigliNode>(node_)->currentInhaleDur};
  }
  
  JSI_PROPERTY_GETTER(currentExhaleDur) {
    return {std::static_pointer_cast<MartigliNode>(node_)->currentExhaleDur};
  }
  
  JSI_PROPERTY_GETTER(currentPeriod) {
    return {std::static_pointer_cast<MartigliNode>(node_)->currentPeriod};
  }
};

#undef MARTIGLI_PROPERTY

} // namespace audioapi
