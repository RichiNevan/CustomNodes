#pragma once

#include <AudioApiTurboModulesJSI.h>

#include <jsi/jsi.h>
#include <memory>
#include <string>

namespace facebook::react {

class NativeOscillatorModule
    : public NativeOscillatorModuleCxxSpec<NativeOscillatorModule> {
public:
  NativeOscillatorModule(std::shared_ptr<CallInvoker> jsInvoker);
  void injectCustomProcessorInstaller(jsi::Runtime &runtime);

private:
  jsi::Function createInstaller(jsi::Runtime &runtime);
  // you can add more installer for different processors here
};

} // namespace facebook::react
