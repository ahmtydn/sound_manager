import 'package:flutter_test/flutter_test.dart';
import 'package:sound_manager/sound_manager.dart';
import 'package:sound_manager/sound_manager_platform_interface.dart';
import 'package:sound_manager/sound_manager_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockSoundManagerPlatform
    with MockPlatformInterfaceMixin
    implements SoundManagerPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final SoundManagerPlatform initialPlatform = SoundManagerPlatform.instance;

  test('$MethodChannelSoundManager is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelSoundManager>());
  });

  test('getPlatformVersion', () async {
    SoundManager soundManagerPlugin = SoundManager();
    MockSoundManagerPlatform fakePlatform = MockSoundManagerPlatform();
    SoundManagerPlatform.instance = fakePlatform;

    expect(await soundManagerPlugin.getPlatformVersion(), '42');
  });
}
