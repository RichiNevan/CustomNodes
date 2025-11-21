#pragma once

#include "SymmetryNode.h"
#include <audioapi/HostObjects/AudioNodeHostObject.h>
#include <jsi/jsi.h>
#include <memory>
#include <cstdio>

using namespace facebook;

namespace audioapi {

#define SYMMETRY_PROPERTY(type, name) \
  if (propName == #name) { \
    return jsi::Value(static_cast<double>(node_->name)); \
  }

#define SYMMETRY_PROPERTY_SETTER(type, name) \
  if (propName == #name) { \
    node_->name = static_cast<type>(value.asNumber()); \
    return; \
  }

#define SYMMETRY_PROPERTY_BOOL(name) \
  if (propName == #name) { \
    return jsi::Value(node_->name); \
  }

#define SYMMETRY_PROPERTY_SETTER_BOOL(name) \
  if (propName == #name) { \
    node_->name = value.asBool(); \
    return; \
  }

class SymmetryNodeHostObject : public AudioNodeHostObject {
public:
  explicit SymmetryNodeHostObject(std::shared_ptr<SymmetryNode> node)
      : AudioNodeHostObject(std::static_pointer_cast<AudioNode>(node)), node_(node) {}

  jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId) override {
    auto propName = propNameId.utf8(runtime);

    SYMMETRY_PROPERTY(double, f0)
    SYMMETRY_PROPERTY(double, noctaves)
    SYMMETRY_PROPERTY(int, nnotes)
    SYMMETRY_PROPERTY(double, d)
    SYMMETRY_PROPERTY(int, waveform)
    SYMMETRY_PROPERTY(int, permfunc)
    SYMMETRY_PROPERTY(double, volume)
    SYMMETRY_PROPERTY_BOOL(shouldStart)
    SYMMETRY_PROPERTY_BOOL(shouldPause)
    SYMMETRY_PROPERTY_BOOL(shouldResume)
    SYMMETRY_PROPERTY_BOOL(shouldStop)
    SYMMETRY_PROPERTY(int, frameCount)

    return AudioNodeHostObject::get(runtime, propNameId);
  }

  void set(jsi::Runtime &runtime, const jsi::PropNameID &propNameId, const jsi::Value &value) override {
    auto propName = propNameId.utf8(runtime);

    SYMMETRY_PROPERTY_SETTER(double, f0)
    SYMMETRY_PROPERTY_SETTER(double, noctaves)
    SYMMETRY_PROPERTY_SETTER(int, nnotes)
    SYMMETRY_PROPERTY_SETTER(double, d)
    SYMMETRY_PROPERTY_SETTER(int, waveform)
    SYMMETRY_PROPERTY_SETTER(int, permfunc)
    SYMMETRY_PROPERTY_SETTER(double, volume)
    SYMMETRY_PROPERTY_SETTER_BOOL(shouldStart)
    SYMMETRY_PROPERTY_SETTER_BOOL(shouldPause)
    SYMMETRY_PROPERTY_SETTER_BOOL(shouldResume)
    SYMMETRY_PROPERTY_SETTER_BOOL(shouldStop)

    AudioNodeHostObject::set(runtime, propNameId, value);
  }

private:
  std::shared_ptr<SymmetryNode> node_;
};

#undef SYMMETRY_PROPERTY
#undef SYMMETRY_PROPERTY_SETTER
#undef SYMMETRY_PROPERTY_BOOL
#undef SYMMETRY_PROPERTY_SETTER_BOOL

} // namespace audioapi
