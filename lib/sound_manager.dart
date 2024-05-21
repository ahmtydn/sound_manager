
import 'sound_manager_platform_interface.dart';

class SoundManager {
  Future<String?> getPlatformVersion() {
    return SoundManagerPlatform.instance.getPlatformVersion();
  }
}
