#ifndef FLUTTER_PLUGIN_SOUND_MANAGER_PLUGIN_H_
#define FLUTTER_PLUGIN_SOUND_MANAGER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace sound_manager {

class SoundManagerPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  SoundManagerPlugin();

  virtual ~SoundManagerPlugin();

  // Disallow copy and assign.
  SoundManagerPlugin(const SoundManagerPlugin&) = delete;
  SoundManagerPlugin& operator=(const SoundManagerPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace sound_manager

#endif  // FLUTTER_PLUGIN_SOUND_MANAGER_PLUGIN_H_
