import 'package:flutter/material.dart';

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

class _MyButtonState extends State<MyButton>
    with SingleTickerProviderStateMixin {
  final ValueNotifier<bool> loading = ValueNotifier<bool>(false);
  late AnimationController _controller;

  @override
  void initState() {
    super.initState();
    _controller = AnimationController(
      duration: const Duration(seconds: 1),
      vsync: this,
    )..repeat(reverse: true);
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  Future<void> _onButtonPressed() async {
    loading.value = true;
    await Future.delayed(const Duration(seconds: 5));
    loading.value = false;
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        IconButton(
          icon: const Icon(Icons.play_arrow),
          iconSize: 100.0,
          onPressed: _onButtonPressed,
        ),
        ValueListenableBuilder(
          valueListenable: loading,
          builder: (context, value, child) {
            return AnimatedBuilder(
              animation: _controller,
              child: const Icon(Icons.notifications, size: 100.0),
              builder: (BuildContext context, Widget? widget) {
                return Transform.rotate(
                  angle: value ? _controller.value * 0.02 : 0,
                  child: widget!,
                );
              },
            );
          },
        ),
      ],
    );
  }
}
