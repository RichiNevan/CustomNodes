#pragma once

#include <AudioApiTurboModulesJSI.h>

#include <jsi/jsi.h>
#include <memory>
#include <string>

namespace facebook::react {

class NativeCustomNodesModule
    : public NativeCustomNodesModuleCxxSpec<NativeCustomNodesModule> {
public:
  NativeCustomNodesModule(std::shared_ptr<CallInvoker> jsInvoker);
  void injectCustomProcessorInstaller(jsi::Runtime &runtime);

private:
  jsi::Function createInstaller(jsi::Runtime &runtime);
  jsi::Function createOscillatorInstaller(jsi::Runtime &runtime);
  jsi::Function createMartigliInstaller(jsi::Runtime &runtime);
  jsi::Function createBinauralInstaller(jsi::Runtime &runtime);
  jsi::Function createSymmetryInstaller(jsi::Runtime &runtime);
  jsi::Function createMartigliBinauralInstaller(jsi::Runtime &runtime);
  jsi::Function createNoiseInstaller(jsi::Runtime &runtime);
};

} // namespace facebook::react
