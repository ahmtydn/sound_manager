import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'sound_manager_method_channel.dart';

abstract class SoundManagerPlatform extends PlatformInterface {
  /// Constructs a SoundManagerPlatform.
  SoundManagerPlatform() : super(token: _token);

  static final Object _token = Object();

  static SoundManagerPlatform _instance = MethodChannelSoundManager();

  /// The default instance of [SoundManagerPlatform] to use.
  ///
  /// Defaults to [MethodChannelSoundManager].
  static SoundManagerPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [SoundManagerPlatform] when
  /// they register themselves.
  static set instance(SoundManagerPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<void> stop() {
    throw UnimplementedError('stop() has not been implemented.');
  }

  Future<void> muteApplication(String appName) {
    throw UnimplementedError('muteApplication() has not been implemented.');
  }

  Future<void> setVolumeForAllExcept(String appName, double volume) {
    throw UnimplementedError(
        'setVolumeForAllExcept() has not been implemented.');
  }

  Future<void> setVolume(String appName, double volume) {
    throw UnimplementedError('setVolume() has not been implemented.');
  }

  Future<void> resetVolume() {
    throw UnimplementedError('resetVolume() has not been implemented.');
  }
}
