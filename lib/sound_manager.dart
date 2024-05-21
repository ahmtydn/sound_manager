import 'sound_manager_platform_interface.dart';

class SoundManager {
  Future<void> stop() {
    return SoundManagerPlatform.instance.stop();
  }

  Future<void> muteApplication(String appName) {
    return SoundManagerPlatform.instance.muteApplication(appName);
  }

  Future<void> setVolume(String appName, double volume) {
    return SoundManagerPlatform.instance.setVolume(appName, volume);
  }

  Future<void> setVolumeForAllExcept(String appName, double volume) {
    return SoundManagerPlatform.instance.setVolumeForAllExcept(appName, volume);
  }

  Future<void> resetVolume() {
    return SoundManagerPlatform.instance.resetVolume();
  }
}
