#include "include/sound_manager/sound_manager_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "sound_manager_plugin.h"

void SoundManagerPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  sound_manager::SoundManagerPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
