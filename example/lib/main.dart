import 'package:flutter/material.dart';
import 'package:just_audio/just_audio.dart';
import 'package:sound_manager/sound_manager.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Sound Manager Example'),
        ),
        body: const Center(
          child: MyButton(),
        ),
      ),
    );
  }
}

class MyButton extends StatefulWidget {
  const MyButton({super.key});

  @override
  State<MyButton> createState() => _MyButtonState();
}

class _MyButtonState extends State<MyButton> {
  final ValueNotifier<bool> loading = ValueNotifier<bool>(false);
  final audioManagerPlugin = SoundManager();
  final AudioPlayer soundPlayer = AudioPlayer();

  Future<void> _onButtonPressed() async {
    loading.value = true;
    await audioManagerPlugin.setVolumeForAllExcept(
        "sound_manager_example.exe", 0.0);
    soundPlayer.setAsset("assets/sounds/sound1.mp3");

    int playCount = 0;

    soundPlayer.playerStateStream.listen((event) async {
      if (event.processingState == ProcessingState.completed &&
          playCount <= 3) {
        playCount++;
        if (playCount <= 3) {
          await soundPlayer.seek(Duration.zero);
          await soundPlayer.setVolume(1.0);
          await soundPlayer.play();
        } else {
          await Future.delayed(const Duration(seconds: 2));
          await audioManagerPlugin.resetVolume();
          loading.value = false;
        }
      }
    });
    await soundPlayer.seek(Duration.zero);
    await soundPlayer.setVolume(1.0);
    await soundPlayer.play();
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        ValueListenableBuilder(
          valueListenable: loading,
          builder: (context, value, child) {
            return loading.value
                ? const Icon(
                    Icons.notifications_active,
                    size: 100,
                  )
                : InkWell(
                    onTap: _onButtonPressed,
                    child: const Icon(
                      Icons.play_arrow,
                      size: 100,
                    ),
                  );
          },
        ),
      ],
    );
  }
}
