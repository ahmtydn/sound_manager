#include "sound_manager_plugin.h"
#include <windows.h>
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <propvarutil.h>
#include <functiondiscoverykeys_devpkey.h>
#include <Psapi.h>

#include <memory>
#include <sstream>
#include <comdef.h>
#include <unordered_map>

#pragma comment(lib, "Psapi.lib")

namespace sound_manager
{

  // Static map to store original volume levels
  static std::unordered_map<DWORD, float> original_volume_levels;

  // static
  void SoundManagerPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "sound_manager",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<SoundManagerPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    registrar->AddPlugin(std::move(plugin));
  }

  SoundManagerPlugin::SoundManagerPlugin() {}

  SoundManagerPlugin::~SoundManagerPlugin() {}

  void SoundManagerPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (method_call.method_name().compare("stopOtherPlayers") == 0)
    {
      stopOtherPlayers();
      result->Success();
    }
    else if (method_call.method_name().compare("muteApplication") == 0)
    {
      const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      if (arguments)
      {
        auto app_name_it = arguments->find(flutter::EncodableValue("app_name"));
        if (app_name_it != arguments->end())
        {
          auto app_name = std::get<std::string>(app_name_it->second);
          muteApplication(std::wstring(app_name.begin(), app_name.end()));
          result->Success();
          return;
        }
      }
      result->Error("Invalid arguments", "Expected map with app_name key");
    }
    else if (method_call.method_name().compare("setVolume") == 0)
    {
      const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      if (arguments)
      {
        auto app_name_it = arguments->find(flutter::EncodableValue("app_name"));
        auto volume_it = arguments->find(flutter::EncodableValue("volume"));
        if (app_name_it != arguments->end() && volume_it != arguments->end())
        {
          auto app_name = std::get<std::string>(app_name_it->second);
          auto volume = std::get<double>(volume_it->second);
          setVolume(std::wstring(app_name.begin(), app_name.end()), static_cast<float>(volume));
          result->Success();
          return;
        }
      }
      result->Error("Invalid arguments", "Expected map with app_name and volume keys");
    }
    else if (method_call.method_name().compare("setVolumeForAllExcept") == 0)
    {
      const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      if (arguments)
      {
        auto app_name_it = arguments->find(flutter::EncodableValue("app_name"));
        auto volume_it = arguments->find(flutter::EncodableValue("volume"));
        if (app_name_it != arguments->end() && volume_it != arguments->end())
        {
          auto app_name = std::get<std::string>(app_name_it->second);
          auto volume = std::get<double>(volume_it->second);
          setVolumeForAllExcept(std::wstring(app_name.begin(), app_name.end()), static_cast<float>(volume));
          result->Success();
          return;
        }
      }
      result->Error("Invalid arguments", "Expected map with app_name and volume keys");
    }
    else if (method_call.method_name().compare("restoreOriginalVolumes") == 0)
    {
      restoreOriginalVolumes();
      result->Success();
    }
    else
    {
      result->NotImplemented();
    }
  }

  void SoundManagerPlugin::stopOtherPlayers()
  {
    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));

    IMMDevice *defaultDevice = NULL;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    IAudioEndpointVolume *endpointVolume = NULL;
    defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);

    endpointVolume->SetMasterVolumeLevelScalar(0.0f, NULL);

    endpointVolume->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
  }

  void SoundManagerPlugin::muteApplication(const std::wstring &app_name)
  {
    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));

    IMMDevice *defaultDevice = NULL;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    IAudioSessionManager2 *sessionManager = NULL;
    defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&sessionManager);

    IAudioSessionEnumerator *sessionEnumerator = NULL;
    sessionManager->GetSessionEnumerator(&sessionEnumerator);

    int sessionCount = 0;
    sessionEnumerator->GetCount(&sessionCount);

    for (int i = 0; i < sessionCount; ++i)
    {
      IAudioSessionControl *sessionControl = NULL;
      sessionEnumerator->GetSession(i, &sessionControl);

      IAudioSessionControl2 *sessionControl2 = NULL;
      if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void **)&sessionControl2)))
      {
        DWORD processId = 0;
        sessionControl2->GetProcessId(&processId);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (processHandle)
        {
          WCHAR processName[MAX_PATH] = L"<unknown>";
          if (GetModuleBaseName(processHandle, NULL, processName, ARRAYSIZE(processName)))
          {
            if (_wcsicmp(processName, app_name.c_str()) == 0)
            {
              ISimpleAudioVolume *audioVolume = NULL;
              if (SUCCEEDED(sessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)&audioVolume)))
              {
                audioVolume->SetMasterVolume(0.0f, NULL);
                audioVolume->Release();
              }
            }
          }
          CloseHandle(processHandle);
        }
        sessionControl2->Release();
      }
      sessionControl->Release();
    }

    sessionEnumerator->Release();
    sessionManager->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
  }

  void SoundManagerPlugin::setVolume(const std::wstring &app_name, float volume)
  {
    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));

    IMMDevice *defaultDevice = NULL;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    IAudioSessionManager2 *sessionManager = NULL;
    defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&sessionManager);

    IAudioSessionEnumerator *sessionEnumerator = NULL;
    sessionManager->GetSessionEnumerator(&sessionEnumerator);

    int sessionCount = 0;
    sessionEnumerator->GetCount(&sessionCount);

    for (int i = 0; i < sessionCount; ++i)
    {
      IAudioSessionControl *sessionControl = NULL;
      sessionEnumerator->GetSession(i, &sessionControl);

      IAudioSessionControl2 *sessionControl2 = NULL;
      if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void **)&sessionControl2)))
      {
        DWORD processId = 0;
        sessionControl2->GetProcessId(&processId);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (processHandle)
        {
          WCHAR processName[MAX_PATH] = L"<unknown>";
          if (GetModuleBaseName(processHandle, NULL, processName, ARRAYSIZE(processName)))
          {
            if (_wcsicmp(processName, app_name.c_str()) == 0)
            {
              ISimpleAudioVolume *audioVolume = NULL;
              if (SUCCEEDED(sessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)&audioVolume)))
              {
                audioVolume->SetMasterVolume(volume, NULL);
                audioVolume->Release();
              }
            }
          }
          CloseHandle(processHandle);
        }
        sessionControl2->Release();
      }
      sessionControl->Release();
    }

    sessionEnumerator->Release();
    sessionManager->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
  }

  void SoundManagerPlugin::setVolumeForAllExcept(const std::wstring &app_name, float volume)
  {
    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));

    IMMDevice *defaultDevice = NULL;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    IAudioSessionManager2 *sessionManager = NULL;
    defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&sessionManager);

    IAudioSessionEnumerator *sessionEnumerator = NULL;
    sessionManager->GetSessionEnumerator(&sessionEnumerator);

    int sessionCount = 0;
    sessionEnumerator->GetCount(&sessionCount);

    for (int i = 0; i < sessionCount; ++i)
    {
      IAudioSessionControl *sessionControl = NULL;
      sessionEnumerator->GetSession(i, &sessionControl);

      IAudioSessionControl2 *sessionControl2 = NULL;
      if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void **)&sessionControl2)))
      {
        DWORD processId = 0;
        sessionControl2->GetProcessId(&processId);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (processHandle)
        {
          WCHAR processName[MAX_PATH] = L"<unknown>";
          if (GetModuleBaseName(processHandle, NULL, processName, ARRAYSIZE(processName)))
          {
            ISimpleAudioVolume *audioVolume = NULL;
            if (_wcsicmp(processName, app_name.c_str()) != 0)
            {
              if (SUCCEEDED(sessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)&audioVolume)))
              {
                // Save original volume
                float originalVolume = 0.0f;
                audioVolume->GetMasterVolume(&originalVolume);
                original_volume_levels[processId] = originalVolume;

                audioVolume->SetMasterVolume(volume, NULL);
                audioVolume->Release();
              }
            }
          }
          CloseHandle(processHandle);
        }
        sessionControl2->Release();
      }
      sessionControl->Release();
    }

    sessionEnumerator->Release();
    sessionManager->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();
  }

  void SoundManagerPlugin::restoreOriginalVolumes()
  {
    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));

    IMMDevice *defaultDevice = NULL;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

    IAudioSessionManager2 *sessionManager = NULL;
    defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void **)&sessionManager);

    IAudioSessionEnumerator *sessionEnumerator = NULL;
    sessionManager->GetSessionEnumerator(&sessionEnumerator);

    int sessionCount = 0;
    sessionEnumerator->GetCount(&sessionCount);

    for (int i = 0; i < sessionCount; ++i)
    {
      IAudioSessionControl *sessionControl = NULL;
      sessionEnumerator->GetSession(i, &sessionControl);

      IAudioSessionControl2 *sessionControl2 = NULL;
      if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void **)&sessionControl2)))
      {
        DWORD processId = 0;
        sessionControl2->GetProcessId(&processId);

        auto it = original_volume_levels.find(processId);
        if (it != original_volume_levels.end())
        {
          HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
          if (processHandle)
          {
            ISimpleAudioVolume *audioVolume = NULL;
            if (SUCCEEDED(sessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void **)&audioVolume)))
            {
              // Restore original volume
              audioVolume->SetMasterVolume(it->second, NULL);
              audioVolume->Release();
            }
            CloseHandle(processHandle);
          }
        }
        sessionControl2->Release();
      }
      sessionControl->Release();
    }

    sessionEnumerator->Release();
    sessionManager->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
    CoUninitialize();

    // Clear the original volume levels map after restoring
    original_volume_levels.clear();
  }
} // namespace sound_manager
