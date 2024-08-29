#include <SDL3/SDL.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "debug.h"
#include "audioplayer.h"

namespace SGI {
  AudioPlayer* AudioPlayer::_instance = nullptr;

  void AudioPlayer::initialize()
  {
    if(_instance == nullptr) {
      _instance = new AudioPlayer();
    }
  }

  AudioPlayer::AudioPlayer()
  {
    if (!SDL_Init(SDL_INIT_AUDIO)) {
      ERROR(AUDIOPLAYER, "Failed to initialize SDL Audio: %s", SDL_GetError());
      return;
    }

    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.format = SDL_AUDIO_F32;
    desiredSpec.channels = 2;
    desiredSpec.freq = 44100;

    _stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desiredSpec, AudioPlayer::_audioCallback, nullptr);
    if (!_stream) {
      ERROR(AUDIOPLAYER, "Failed to open audio stream: %s", SDL_GetError());
      return;
    }

    if (!SDL_GetAudioStreamFormat(_stream, &_obtainedSpec, NULL)) {
      ERROR(AUDIOPLAYER, "Failed to get audio spec: %s", SDL_GetError());
      SDL_DestroyAudioStream(_stream);
      return;
    }

    SDL_ResumeAudioStreamDevice(_stream);

    _deviceId = SDL_GetAudioStreamDevice(_stream);
    _deviceName = std::string(SDL_GetAudioDeviceName(_deviceId));
    LOG(AUDIOPLAYER, "Opened device %s, freq: %d, chan: %d, format: 0x%X", _deviceName.c_str(), _obtainedSpec.freq, _obtainedSpec.channels, _obtainedSpec.format);

    const char *basePath = SDL_GetBasePath();
    if (basePath) {
      _resourcePath = std::string(basePath);
    } else {
      _resourcePath = "/";
    }
  }

  AudioPlayer::~AudioPlayer()
  {
    for (auto& entry : _audioDataMap) {
      SDL_free(entry.second.start);
    }

    if (_stream) {
      SDL_DestroyAudioStream(_stream);
    }
  }

  bool AudioPlayer::isPlaying(const std::string& id)
  {
    initialize();

    auto it = _instance->_audioDataMap.find(id);
    if (it != _instance->_audioDataMap.end()) {
      return it->second.playing;
    }
    return false;
  }

  bool AudioPlayer::load(const std::string& id, const std::string& filename)
  {
    initialize();

    SDL_IOStream* fio = SDL_IOFromFile((_instance->_resourcePath + filename).c_str(), "r");
    if (!fio) {
      ERROR(AUDIOPLAYER, "Could not open file %s: %s", (_instance->_resourcePath + filename).c_str(), SDL_GetError());
      return false;
    }
    
    char magicBuffer[4];
    do {
      if(SDL_ReadIO(fio, magicBuffer, 4) != 4) {
        ERROR(AUDIOPLAYER, "Failed to read file header.");
      }
    } while (magicBuffer[0] == 0);
    std::string magic = std::string(magicBuffer, 4);

    SDL_SeekIO(fio, 0, SDL_IO_SEEK_SET);

    if (magic == "RIFF") {
      SDL_AudioSpec fileSpec;
      Uint8* fileBuffer;
      Uint32 fileLength;

      if (!SDL_LoadWAV_IO(fio, SDL_TRUE, &fileSpec, &fileBuffer, &fileLength)) {
        ERROR(AUDIOPLAYER, "Failed to load WAV file %s: %s", filename.c_str(), SDL_GetError());
        return false;
      }
      LOG(AUDIOPLAYER, "Loaded %s (%d len), freq: %d chan: %d format: 0x%X", filename.c_str(), fileLength, fileSpec.freq, fileSpec.channels, fileSpec.format);

      Uint8* wavBuffer;
      int wavLength;

      SDL_ConvertAudioSamples(&fileSpec, fileBuffer, fileLength, &_instance->_obtainedSpec, &wavBuffer, &wavLength);
      SDL_free(fileBuffer);

      // Store the audio data in the map
      AudioData audioData;
      audioData.start = wavBuffer;
      audioData.pos = wavBuffer;
      audioData.length = wavLength;
      audioData.totalLength = wavLength;
      audioData.playing = false;
      audioData.repeating = false;

      _instance->_audioDataMap[id] = audioData;
      return true;
    } else if (magic == "OggS") {
      const ov_callbacks callbacks{
        [](void* buffer, size_t elementSize, size_t elementCount, void* dataSource)->size_t {
          return SDL_ReadIO((SDL_IOStream*)dataSource, buffer, elementCount * elementSize);
        },
        [](void* dataSource, ogg_int64_t offset, int origin)->int {
          static const std::vector<SDL_IOWhence> seekDirections{
            SDL_IO_SEEK_SET, SDL_IO_SEEK_CUR, SDL_IO_SEEK_END
          };
          return SDL_SeekIO((SDL_IOStream*)dataSource, offset, seekDirections.at(origin));
        },
        nullptr,
        [](void* dataSource)->long {
          return SDL_TellIO((SDL_IOStream*)dataSource);
        }
      };

      OggVorbis_File vf;
      if (ov_open_callbacks(fio, &vf, NULL, 0, callbacks) < 0) {
        SDL_CloseIO(fio);
        ERROR(AUDIOPLAYER, "Failed to open OGG file %s", filename.c_str());
        return false;
      }

      vorbis_info* vi = ov_info(&vf, -1);

      SDL_AudioSpec fileSpec;
      SDL_zero(fileSpec);
      fileSpec.freq = vi->rate;
      fileSpec.channels = vi->channels;
      fileSpec.format = SDL_AUDIO_S16;


      Uint32 totalSamples = static_cast<Uint32>(ov_pcm_total(&vf, -1)) * vi->channels;
      Uint32 bufferSize = totalSamples * sizeof(float);
      Uint8* buffer = new Uint8[bufferSize];
      Uint8* currentPos = buffer;

      LOG(AUDIOPLAYER, "Loaded %s (%d len), freq: %d chan: %d format: 0x%X", filename.c_str(), bufferSize, fileSpec.freq, fileSpec.channels, fileSpec.format);

      int bitstream;
      long bytesRead;
      while ((bytesRead = ov_read(&vf, (char*)currentPos, bufferSize - (currentPos - buffer), 0, 2, 1, &bitstream)) > 0) {
        currentPos += bytesRead;
      }

      ov_clear(&vf);
      SDL_CloseIO(fio);

      Uint8* oggBuffer;
      int oggLength;

      if (!SDL_ConvertAudioSamples(&fileSpec, buffer, bufferSize, &_instance->_obtainedSpec, &oggBuffer, &oggLength)) {
        ERROR(AUDIOPLAYER, "Failed to convert autiod format %s: %s", filename.c_str(), SDL_GetError());
        delete[] buffer;
        return false;
      }
      delete[] buffer;

      // Store the audio data in the map
      AudioData audioData;
      audioData.start = oggBuffer;
      audioData.pos = oggBuffer;
      audioData.length = oggLength;
      audioData.totalLength = oggLength;
      audioData.playing = false;
      audioData.repeating = false;

      _instance->_audioDataMap[id] = audioData;
      return true;
    }

    SDL_CloseIO(fio);
    ERROR(AUDIOPLAYER, "Failed to load file %s: Unknown file type (%s)", filename.c_str(), magic.c_str());

    return false;
  }

  void AudioPlayer::newBuffer(const std::string& id)
  {
    initialize();

    AudioData audioData;
    audioData.start = nullptr;
    audioData.pos = nullptr;
    audioData.length = 0;
    audioData.totalLength = 0;
    audioData.playing = false;
    audioData.repeating = false;

    _instance->_audioDataMap[id] = audioData;
  }

  bool AudioPlayer::bufferAddTone(const std::string& id, int freq, int duration, int cycles, int delay)
  {
    initialize();

    if (duration == 0 && delay == 0) {
      return false;
    }

    auto it = _instance->_audioDataMap.find(id);
    if (it == _instance->_audioDataMap.end()) {
      ERROR(AUDIOPLAYER, "Buffer ID %s not found", id.c_str());
      return false;
    }
    AudioData& audioData = it->second;

    // LOG(AUDIOPLAYER, "bufferAddTone, freq: %d, dureation: %d, cycles: %d, dealy: %d", freq, duration, cycles, delay);

    int sampleRate = _instance->_obtainedSpec.freq;
    int numSamples = sampleRate * duration / 1000;
    int fadeSamples = sampleRate * 5 / 1000;
    int delaySamples = sampleRate * delay / 1000;
    int cycleSamples = (numSamples + fadeSamples + delaySamples) * cycles;

    Uint8* newBuffer = new Uint8[audioData.totalLength + cycleSamples * sizeof(float)];
    if (!newBuffer) {
      ERROR(AUDIOPLAYER, "Failed to allocate memory for tone buffer");
      return false;
    }

    if (audioData.start) {
      SDL_memcpy(newBuffer, audioData.start, audioData.totalLength);
      delete[] audioData.start;
    }

    float* buffer = reinterpret_cast<float*>(newBuffer + audioData.totalLength);

    for (int i = 0; i < cycles; ++i) {
        for (int j = 0; j < numSamples; ++j) {
            buffer[j] = std::sin(2.0f * M_PI * freq * j / sampleRate);
        }

        // Apply a fade-out on the last few samples to avoid chirps
        for (int j = numSamples - fadeSamples; j < numSamples; ++j) {
            float fadeFactor = static_cast<float>(numSamples - j) / fadeSamples;
            buffer[j] *= fadeFactor;
        }

        buffer += numSamples;
        SDL_memset(buffer, 0, sampleRate * delay / 1000 * sizeof(float));
        buffer += sampleRate * delay / 1000;
    }

    audioData.start = newBuffer;
    audioData.pos = newBuffer;
    audioData.totalLength += cycleSamples * sizeof(float);
    audioData.length = audioData.totalLength;

    return true;
  }

  bool AudioPlayer::bufferAddMusic(const std::string& id, const std::string& music)
  {
    initialize();
    
    int octave = 4;  // Default octave
    int length = 4;  // Default note length (quarter note)
    int tempo = 120; // Default tempo
    float quaterNodeDuration = 60000.0f / tempo;  // Duration of a quarter note in ms
    float multiplier = 7.0f / 8.0f; // Default duration multiplier (MN)
    
    auto it = _instance->_audioDataMap.find(id);
    if (it == _instance->_audioDataMap.end()) {
      ERROR(AUDIOPLAYER, "Buffer ID %s not found", id.c_str());
      return false;
    }
    AudioData& audioData = it->second;
    
    for (size_t i = 0; i < music.length(); ++i) {
      char command = music[i];

      if (command >= 'A' && command <= 'G') {
        bool sharp = (i + 1 < music.length() && (music[i + 1] == '#' || music[i + 1] == '+'));
        bool flat = (i + 1 < music.length() && music[i + 1] == '-');
        if (sharp || flat) ++i;
        
        float frequency = _getNoteFrequency(command, octave, sharp, flat);
        
        if (i + 1 < music.length() && music[i + 1] == '.') {
          multiplier += 0.5f;
          ++i;
        }

        int noteDuration = static_cast<int>(4.0 / length * quaterNodeDuration);
        int duration = static_cast<int>(noteDuration * multiplier);
        int delay = noteDuration - duration;
        // LOG(AUDIOPLAYER, "Length: %d, duration: %d", length, noteDuration);
        bufferAddTone(id, static_cast<int>(frequency), duration, 1, delay);
        multiplier = 7.0f / 8.0f; // Reset to default after note
          
      } else if (command == 'L') {
        // Set the note length
        if (i + 1 < music.length() && isdigit(music[i + 1])) {
          length = music[++i] - '0';
        }
      } else if (command == 'O') {
        // Set the octave
        if (i + 1 < music.length() && isdigit(music[i + 1])) {
          octave = music[++i] - '0';
        }
      } else if (command == 'N') {
        // Play a note by number (0 is a rest)
        if (i + 1 < music.length() && isdigit(music[i + 1])) {
          int noteNumber = stoi(music.substr(i + 1, 2));
          i += 2;
          if (noteNumber == 0) {
            int noteDuration = static_cast<int>(4 / length * quaterNodeDuration);
            int duration = static_cast<int>(noteDuration * multiplier);
            int delay = noteDuration - duration;
            bufferAddTone(id, 0, noteDuration, 1, delay);
          } else {
            // Calculate frequency and add tone
            float frequency = _calculateFrequencyFromNoteNumber(noteNumber);
            bufferAddTone(id, static_cast<int>(frequency), static_cast<int>(quaterNodeDuration * multiplier), 1, 0);
          }
        }
      } else if (command == 'P') {
        // Play a rest
        if (i + 1 < music.length() && isdigit(music[i + 1])) {
          int restLength = music[++i] - '0';
          int noteDuration = static_cast<int>(4 / restLength * quaterNodeDuration);
          int duration = static_cast<int>(noteDuration * multiplier);
          int delay = noteDuration - duration;
          bufferAddTone(id, 0, noteDuration, 1, delay);
        }
      } else if (command == 'M') {
        // Music control (MN, ML, MS)
        if (i + 1 < music.length()) {
          char mode = music[++i];
          if (mode == 'N') multiplier = 7.0f / 8.0f;
          else if (mode == 'L') multiplier = 1.0f;
          else if (mode == 'S') multiplier = 3.0f / 4.0f;
        }
      } else if (command == 'T') {
        // Set the tempo
        if (i + 1 < music.length() && isdigit(music[i + 1])) {
          tempo = stoi(music.substr(i + 1, 3));
          i += 2;
          quaterNodeDuration = 60000.0f / tempo;
        }
      }
    }

    return true;
  }

  void AudioPlayer::play(const std::string& id, bool repeating)
  {
    initialize();

    auto it = _instance->_audioDataMap.find(id);
    if (it != _instance->_audioDataMap.end()) {
      it->second.playing = true;
      it->second.repeating = repeating;
    }
  }

  void AudioPlayer::pause(const std::string& id)
  {
    initialize();

    auto it = _instance->_audioDataMap.find(id);
    if (it != _instance->_audioDataMap.end()) {
      it->second.playing = false;
    }
  }

  void AudioPlayer::stop(const std::string& id)
  {
    initialize();

    auto it = _instance->_audioDataMap.find(id);
    if (it != _instance->_audioDataMap.end()) {
      it->second.playing = false;
      it->second.pos = it->second.start;
      it->second.length = it->second.totalLength;
    }
  }

  void AudioPlayer::setRepeating(const std::string& id, bool value)
  {
    initialize();

    auto it = _instance->_audioDataMap.find(id);
    if (it != _instance->_audioDataMap.end()) {
      it->second.repeating = value;
    }
  }

  void AudioPlayer::setResourcePath(const std::string path)
  {
    initialize();

    std::string _resourcePath = path;
    if (_instance->_resourcePath.empty() || _instance->_resourcePath.back() != '/') {
      _instance->_resourcePath += "/";
    }
  }

  void AudioPlayer::unload(const std::string& id)
  {
    initialize();

    auto it = _instance->_audioDataMap.find(id);
    if (it != _instance->_audioDataMap.end()) {
      SDL_free(it->second.start);
      _instance->_audioDataMap.erase(it);
      LOG(AUDIOPLAYER, "Unloaded audio with id %s", id.c_str());
    }
  }

  void AudioPlayer::_audioCallback(void *userdata, SDL_AudioStream *astream, int additional_amount, int total_amount)
  {
    Uint8* mixBuffer = new Uint8[total_amount];
    SDL_memset(mixBuffer, 0, total_amount);

    for (auto& entry : _instance->_audioDataMap) {
      AudioData& audioData = entry.second;
      if (!audioData.playing || audioData.length == 0) {
        continue;
      }

      Uint32 length = (total_amount > audioData.length) ? audioData.length : total_amount;
      SDL_MixAudio(mixBuffer, audioData.pos, SDL_AUDIO_F32, length, 1.0);

      audioData.pos += length;
      audioData.length -= length;

      if (audioData.length == 0 && audioData.repeating) {
        audioData.pos = audioData.start;
        audioData.length = audioData.totalLength;
      }
    }

    if (!SDL_PutAudioStreamData(astream, mixBuffer, additional_amount)) {
      ERROR(AUDIOPLAYER, "Failed to put audio stream data: %s", SDL_GetError());
    }

    delete[] mixBuffer;
  }

  float AudioPlayer::_getNoteFrequency(char note, int octave, bool sharp, bool flat)
  {
    // Define frequencies for the 4th octave
    static const std::map<char, float> baseFrequencies = {
      {'C', 261.63f}, {'D', 293.66f}, {'E', 329.63f},
      {'F', 349.23f}, {'G', 392.00f}, {'A', 440.00f}, {'B', 493.88f}
    };
    
    float frequency = baseFrequencies.at(note);
    if (sharp) frequency *= pow(2.0, 1.0/12.0); // Raise frequency by one semitone
    if (flat)  frequency /= pow(2.0, 1.0/12.0); // Lower frequency by one semitone

    return frequency * pow(2.0, octave - 4); // Adjust for octave
  }

  float AudioPlayer::_calculateFrequencyFromNoteNumber(int noteNumber)
  {
    // Use A4 (440 Hz) as reference
    int relativeNote = noteNumber - 49; // 49th note is A4
    return 440.0f * pow(2.0f, relativeNote / 12.0f);
  }

}
