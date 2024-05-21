import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'sound_manager_platform_interface.dart';

/// An implementation of [SoundManagerPlatform] that uses method channels.
class MethodChannelSoundManager extends SoundManagerPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('sound_manager');

  @override
  Future<void> stop() async {
    await methodChannel.invokeMethod<void>('stopOtherPlayers');
  }

  @override
  Future<void> muteApplication(String appName) async {
    await methodChannel
        .invokeMethod<void>('muteApplication', {'app_name': appName});
  }

  @override
  Future<void> setVolume(String appName, double volume) async {
    await methodChannel.invokeMethod<void>(
        'setVolume', {'app_name': appName, 'volume': volume});
  }

  @override
  Future<void> setVolumeForAllExcept(String appName, double volume) async {
    await methodChannel.invokeMethod<void>(
        'setVolumeForAllExcept', {'app_name': appName, 'volume': volume});
  }

  @override
  Future<void> resetVolume() async {
    await methodChannel.invokeMethod<void>('restoreOriginalVolumes');
  }
}
