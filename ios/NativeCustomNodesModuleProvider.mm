#import "NativeCustomNodesModuleProvider.h"
#import <ReactCommon/CallInvoker.h>
#import <ReactCommon/TurboModule.h>
#import "NativeCustomNodesModule.h"

@implementation NativeCustomNodesModuleProvider

- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params
{
  return std::make_shared<facebook::react::NativeCustomNodesModule>(params.jsInvoker);
}

@end
