#ifndef SGI_AUDIOPLAYER_H
#define SGI_AUDIOPLAYER_H

#include <map>
#include <SDL3/SDL.h>
#include <string>
#include "widget.h"

namespace SGI {
  class AudioPlayer {
  public:
    void operator=(const AudioPlayer &) = delete;

    static bool isPlaying(const std::string& id);
    static bool load(const std::string& id, const std::string& filename);

    static void pause(const std::string& id);
    static void play(const std::string& id, bool repeating = false);
    static void stop(const std::string& id);

    static void setRepeating(const std::string& id, bool value);
    static void setResourcePath(const std::string path);

    static void unload(const std::string& id);

  private:
    AudioPlayer();
    ~AudioPlayer();

    static void initialize();

    static AudioPlayer* _instance;

    struct AudioData {
      Uint8* pos;
      Uint8* start;
      Uint32 length;
      Uint32 totalLength;
      bool playing;
      bool repeating;
    };

    static void _audioCallback(void *userdata, SDL_AudioStream *astream, int additional_amount, int total_amount);

    std::string _resourcePath;

    SDL_AudioSpec _obtainedSpec;
    SDL_AudioStream* _stream;
    SDL_AudioDeviceID _deviceId;
    std::string _deviceName;
    std::map<std::string, AudioData> _audioDataMap;

  };
}

#endif // SGI_BUTTON_H
