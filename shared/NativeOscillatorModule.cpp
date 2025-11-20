#include "NativeOscillatorModule.h"
#include "MyOscillatorNodeHostObject.h"
#include "MartigliNodeHostObject.h"
#include <iostream>
#include <functional>
#include <memory>
#include <audioapi/HostObjects/BaseAudioContextHostObject.h>
#include "MyOscillatorNode.h"
#include "MartigliNode.h"
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
  runtime.global().setProperty(runtime, "createMyOscillatorNode", oscillatorInstaller);
  runtime.global().setProperty(runtime, "createMartigliNode", martigliInstaller);
  printf("NativeOscillatorModule: 'createMyOscillatorNode' and 'createMartigliNode' injected globally\n");
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
} // namespace facebook::react
