#include "NativeOscillatorModule.h"
#include "MyOscillatorNodeHostObject.h"
#include "MartigliNodeHostObject.h"
#include "BinauralNodeHostObject.h"
#include "SymmetryNodeHostObject.h"
#include <iostream>
#include <functional>
#include <memory>
#include <audioapi/HostObjects/BaseAudioContextHostObject.h>
#include "MyOscillatorNode.h"
#include "MartigliNode.h"
#include "BinauralNode.h"
#include "SymmetryNode.h"
#include <cstdio> // For printf debugging

namespace facebook::react {

NativeOscillatorModule::NativeOscillatorModule(std::shared_ptr<CallInvoker> jsInvoker)
    : NativeOscillatorModuleCxxSpec(std::move(jsInvoker)) {
        printf("NativeOscillatorModule: Initialized\n");
    }

void NativeOscillatorModule::injectCustomProcessorInstaller(jsi::Runtime &runtime) {
  printf("NativeOscillatorModule: injectCustomProcessorInstaller called\n");
  auto oscillatorInstaller = createOscillatorInstaller(runtime);
  auto martigliInstaller = createMartigliInstaller(runtime);
  auto binauralInstaller = createBinauralInstaller(runtime);
  auto symmetryInstaller = createSymmetryInstaller(runtime);
  runtime.global().setProperty(runtime, "createMyOscillatorNode", oscillatorInstaller);
  runtime.global().setProperty(runtime, "createMartigliNode", martigliInstaller);
  runtime.global().setProperty(runtime, "createBinauralNode", binauralInstaller);
  runtime.global().setProperty(runtime, "createSymmetryNode", symmetryInstaller);
  printf("NativeOscillatorModule: 'createMyOscillatorNode', 'createMartigliNode', 'createBinauralNode', and 'createSymmetryNode' injected globally\n");
}

jsi::Function NativeOscillatorModule::createInstaller(jsi::Runtime &runtime) {
    printf("NativeOscillatorModule: createInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createMyOscillatorNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeOscillatorModule: createMyOscillatorNode called from JS\n");
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

jsi::Function NativeOscillatorModule::createOscillatorInstaller(jsi::Runtime &runtime) {
    printf("NativeOscillatorModule: createOscillatorInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createMyOscillatorNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeOscillatorModule: createMyOscillatorNode called from JS\n");
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

jsi::Function NativeOscillatorModule::createMartigliInstaller(jsi::Runtime &runtime) {
    printf("NativeOscillatorModule: createMartigliInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createMartigliNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeOscillatorModule: createMartigliNode called from JS\n");
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

jsi::Function NativeOscillatorModule::createBinauralInstaller(jsi::Runtime &runtime) {
    printf("NativeOscillatorModule: createBinauralInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createBinauralNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeOscillatorModule: createBinauralNode called from JS with %zu args\n", count);
        if (count == 0) {
          printf("NativeOscillatorModule: ERROR - no arguments passed to createBinauralNode\n");
          return jsi::Object::createFromHostObject(runtime, nullptr);
        }
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          printf("NativeOscillatorModule: Creating BinauralNode with context\n");
          auto node = std::make_shared<audioapi::BinauralNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::BinauralNodeHostObject>(node);
          printf("NativeOscillatorModule: BinauralNode created successfully\n");
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        printf("NativeOscillatorModule: ERROR - context is null\n");
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }

jsi::Function NativeOscillatorModule::createSymmetryInstaller(jsi::Runtime &runtime) {
    printf("NativeOscillatorModule: createSymmetryInstaller called\n");
  return jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "createSymmetryNode"),
      0,
      [](jsi::Runtime &runtime, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
        printf("NativeOscillatorModule: createSymmetryNode called from JS with %zu args\n", count);
        if (count == 0) {
          printf("NativeOscillatorModule: ERROR - no arguments passed to createSymmetryNode\n");
          return jsi::Object::createFromHostObject(runtime, nullptr);
        }
        auto object = args[0].getObject(runtime);
        auto context = object.getHostObject<audioapi::BaseAudioContextHostObject>(runtime);
        if (context != nullptr) {
          printf("NativeOscillatorModule: Creating SymmetryNode with context\n");
          auto node = std::make_shared<audioapi::SymmetryNode>(context->context_.get());
          auto nodeHostObject = std::make_shared<audioapi::SymmetryNodeHostObject>(node);
          printf("NativeOscillatorModule: SymmetryNode created successfully\n");
          return jsi::Object::createFromHostObject(runtime, nodeHostObject);
        }
        printf("NativeOscillatorModule: ERROR - context is null\n");
        return jsi::Object::createFromHostObject(runtime, nullptr);
      });
    }
} // namespace facebook::react
