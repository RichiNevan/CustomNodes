#include "NativeCustomNodesModule.h"
#include "MyOscillatorNodeHostObject.h"
#include "MartigliNodeHostObject.h"
#include "BinauralNodeHostObject.h"
#include "SymmetryNodeHostObject.h"
#include "MartigliBinauralNodeHostObject.h"
#include "NoiseNodeHostObject.h"
#include <iostream>
#include <functional>
#include <memory>
#include <audioapi/HostObjects/BaseAudioContextHostObject.h>
#include "MyOscillatorNode.h"
#include "MartigliNode.h"
#include "BinauralNode.h"
#include "SymmetryNode.h"
#include "MartigliBinauralNode.h"
#include "NoiseNode.h"
#include <cstdio> // For printf debugging

namespace facebook::react {

NativeCustomNodesModule::NativeCustomNodesModule(std::shared_ptr<CallInvoker> jsInvoker)
    : NativeCustomNodesModuleCxxSpec(std::move(jsInvoker)) {
        printf("NativeCustomNodesModule: Initialized\n");
    }

void NativeCustomNodesModule::injectCustomProcessorInstaller(jsi::Runtime &runtime) {
  printf("NativeCustomNodesModule: injectCustomProcessorInstaller called\n");
  auto oscillatorInstaller = createOscillatorInstaller(runtime);
  auto martigliInstaller = createMartigliInstaller(runtime);
  auto binauralInstaller = createBinauralInstaller(runtime);
  auto symmetryInstaller = createSymmetryInstaller(runtime);
  auto martigliBinauralInstaller = createMartigliBinauralInstaller(runtime);
  auto noiseInstaller = createNoiseInstaller(runtime);
  runtime.global().setProperty(runtime, "createMyOscillatorNode", oscillatorInstaller);
  runtime.global().setProperty(runtime, "createMartigliNode", martigliInstaller);
  runtime.global().setProperty(runtime, "createBinauralNode", binauralInstaller);
  runtime.global().setProperty(runtime, "createSymmetryNode", symmetryInstaller);
  runtime.global().setProperty(runtime, "createMartigliBinauralNode", martigliBinauralInstaller);
  runtime.global().setProperty(runtime, "createNoiseNode", noiseInstaller);
  printf("NativeCustomNodesModule: All node installers injected globally\n");
}

jsi::Function NativeCustomNodesModule::createInstaller(jsi::Runtime &runtime) {
    printf("NativeCustomNodesModule: createInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createMyOscillatorNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeCustomNodesModule: createMyOscillatorNode called from JS\n");
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          auto node = std::make_shared<audioapi::MyOscillatorNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::MyOscillatorNodeHostObject>(node);
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }

jsi::Function NativeCustomNodesModule::createOscillatorInstaller(jsi::Runtime &runtime) {
    printf("NativeCustomNodesModule: createOscillatorInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createMyOscillatorNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeCustomNodesModule: createMyOscillatorNode called from JS\n");
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          auto node = std::make_shared<audioapi::MyOscillatorNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::MyOscillatorNodeHostObject>(node);
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }

jsi::Function NativeCustomNodesModule::createMartigliInstaller(jsi::Runtime &runtime) {
    printf("NativeCustomNodesModule: createMartigliInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createMartigliNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeCustomNodesModule: createMartigliNode called from JS\n");
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          auto node = std::make_shared<audioapi::MartigliNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::MartigliNodeHostObject>(node);
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }

jsi::Function NativeCustomNodesModule::createBinauralInstaller(jsi::Runtime &runtime) {
    printf("NativeCustomNodesModule: createBinauralInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createBinauralNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeCustomNodesModule: createBinauralNode called from JS with %zu args\n", count);
        if (count == 0) {
          printf("NativeCustomNodesModule: ERROR - no arguments passed to createBinauralNode\n");
          return jsi::Object::createFromHostObject(runtime, nullptr);
        }
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          printf("NativeCustomNodesModule: Creating BinauralNode with context\n");
          auto node = std::make_shared<audioapi::BinauralNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::BinauralNodeHostObject>(node);
          printf("NativeCustomNodesModule: BinauralNode created successfully\n");
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        printf("NativeCustomNodesModule: ERROR - context is null\n");
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }

jsi::Function NativeCustomNodesModule::createSymmetryInstaller(jsi::Runtime &runtime) {
    printf("NativeCustomNodesModule: createSymmetryInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createSymmetryNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeCustomNodesModule: createSymmetryNode called from JS with %zu args\n", count);
        if (count == 0) {
          printf("NativeCustomNodesModule: ERROR - no arguments passed to createSymmetryNode\n");
          return jsi::Object::createFromHostObject(runtime, nullptr);
        }
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          printf("NativeCustomNodesModule: Creating SymmetryNode with context\n");
          auto node = std::make_shared<audioapi::SymmetryNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::SymmetryNodeHostObject>(node);
          printf("NativeCustomNodesModule: SymmetryNode created successfully\n");
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        printf("NativeCustomNodesModule: ERROR - context is null\n");
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }

jsi::Function NativeCustomNodesModule::createMartigliBinauralInstaller(jsi::Runtime &runtime) {
    printf("NativeCustomNodesModule: createMartigliBinauralInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createMartigliBinauralNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeCustomNodesModule: createMartigliBinauralNode called from JS with %zu args\n", count);
        if (count == 0) {
          printf("NativeCustomNodesModule: ERROR - no arguments passed to createMartigliBinauralNode\n");
          return jsi::Object::createFromHostObject(runtime, nullptr);
        }
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          printf("NativeCustomNodesModule: Creating MartigliBinauralNode with context\n");
          auto node = std::make_shared<audioapi::MartigliBinauralNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::MartigliBinauralNodeHostObject>(node);
          printf("NativeCustomNodesModule: MartigliBinauralNode created successfully\n");
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        printf("NativeCustomNodesModule: ERROR - context is null\n");
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }

jsi::Function NativeCustomNodesModule::createNoiseInstaller(jsi::Runtime &runtime) {
    printf("NativeCustomNodesModule: createNoiseInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createNoiseNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeCustomNodesModule: createNoiseNode called from JS with %zu args\n", count);
        if (count == 0) {
          printf("NativeCustomNodesModule: ERROR - no arguments passed to createNoiseNode\n");
          return jsi::Object::createFromHostObject(runtime, nullptr);
        }
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          printf("NativeCustomNodesModule: Creating NoiseNode with context\n");
          auto node = std::make_shared<audioapi::NoiseNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::NoiseNodeHostObject>(node);
          printf("NativeCustomNodesModule: NoiseNode created successfully\n");
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        printf("NativeCustomNodesModule: ERROR - context is null\n");
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }
} // namespace facebook::react
