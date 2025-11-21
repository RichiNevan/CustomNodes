#pragma once
#include <atomic>

namespace audioapi {

// Lightweight singleton for sharing animation values between nodes
// Used to pass Martigli breathing animation to Binaural/Martigli-Binaural panning
class AnimationValueRegistry {
public:
  static AnimationValueRegistry& getInstance() {
    static AnimationValueRegistry instance;
    return instance;
  }
  
  // MartigliNode calls this to publish its animation value
  // Only publishes if isActive is true (controlled by session's isOn parameter)
  void setMartigliAnimationValue(float value, bool isActive) {
    if (isActive) {
      martigliValue_.store(value, std::memory_order_relaxed);
    }
  }
  
  // BinauralNode/Martigli-BinauralNode call this to read the value for panOsc=3
  float getMartigliAnimationValue() const {
    return martigliValue_.load(std::memory_order_relaxed);
  }
  
private:
  AnimationValueRegistry() = default;
  std::atomic<float> martigliValue_{0.0f};
  
  // Prevent copying
  AnimationValueRegistry(const AnimationValueRegistry&) = delete;
  AnimationValueRegistry& operator=(const AnimationValueRegistry&) = delete;
};

} // namespace audioapi
