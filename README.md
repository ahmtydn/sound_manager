
# SoundManager Flutter Plugin

[![Buy Me a Coffee](https://img.shields.io/badge/Buy%20Me%20a%20Coffee-donate-yellow)](https://buymeacoffee.com/ahmtydn)
![ViewCount](https://views.whatilearened.today/views/github/your-repo/sound_manager.svg)

## Description

The `SoundManager` plugin provides functionality to manage the volume of applications on a Windows platform. It allows you to mute applications, set the volume for specific applications, and control the volume of all applications except a specified one.

![SoundManager Flutter Plugin - visual selection](https://github.com/user-attachments/assets/a93303e6-7937-48c7-9b69-2a02976602bb)


## Installation

Add the following to your `pubspec.yaml` file:

```yaml
dependencies:
  sound_manager: ^0.0.2
```

## Usage

Import the `SoundManager` class:

```dart
import 'package:sound_manager/sound_manager.dart';
```

### Example

```dart
void main() {
  final soundManager = SoundManager();

  // Stop all other audio players
  soundManager.stop();

  // Mute a specific application
  soundManager.muteApplication("ApplicationName");

  // Set the volume of a specific application
  soundManager.setVolume("ApplicationName", 0.5);

  // Set the volume of all applications except a specific one
  soundManager.setVolumeForAllExcept("ApplicationName", 0.2);

  // Reset the volume of all applications to their original levels
  soundManager.resetVolume();
}
```

## Platform Support

Currently, this plugin supports only the Windows platform.

## Contributions

Contributions are welcome! Please submit a pull request or open an issue to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
