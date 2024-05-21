import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'sound_manager_platform_interface.dart';

/// An implementation of [SoundManagerPlatform] that uses method channels.
class MethodChannelSoundManager extends SoundManagerPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('sound_manager');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
