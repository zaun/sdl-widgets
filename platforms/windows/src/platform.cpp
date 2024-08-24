#include "platform.h"
#include <shlobj.h>
#include <stdexcept>
#include <filesystem>
#include <mfapi.h>
#include <mfobjects.h>
#include <mferror.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <wrl.h>
#include <vector>
#include <thread>

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "Mfobjects.lib")
#pragma comment(lib, "Kernel32.lib")

namespace SGI {

  std::string Platform::getUserDocumentDirectory() {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, path))) {
      return std::string(path);
    }
    throw std::runtime_error("Failed to get user document directory");
  }

  class VideoPlayer {
  public:
    VideoPlayer(const std::string& filePath);
    ~VideoPlayer();

    void play();
    void pause();
    void stop();
    void seek(double seconds);
    double getDuration();
    double getCurrentTime();
    void render(SDL_Renderer* renderer, SDL_Rect& contentArea);
    void setLocationCallback(std::function<void(float currentTime, float maxTime)> callback);
    void setRepeat(bool value);

  private:
    Microsoft::WRL::ComPtr<IMFMediaPlayer> mediaPlayer;
    Microsoft::WRL::ComPtr<IMFSourceResolver> sourceResolver;
    Microsoft::WRL::ComPtr<IMFMediaSource> mediaSource;
    Microsoft::WRL::ComPtr<IMFTransform> decoder;
    Microsoft::WRL::ComPtr<IMFMediaType> mediaType;
    std::function<void(float currentTime, float maxTime)> locationCallback;
    bool repeatMode;
    SDL_Texture* texture;

    void setupPlayer(const std::string& filePath);
    void updateLocation();
    void renderFrame(SDL_Renderer* renderer, SDL_Rect& contentArea);
  };

  VideoPlayer::VideoPlayer(const std::string& filePath) : texture(nullptr) {
    MFStartup(MF_VERSION);
    setupPlayer(filePath);
    repeatMode = false;
  }

  VideoPlayer::~VideoPlayer() {
    stop();
    if (texture) {
      SDL_DestroyTexture(texture);
    }
    MFShutdown();
  }

  void VideoPlayer::setupPlayer(const std::string& filePath) {
    // Create Media Source from the file path
    HRESULT hr = MFCreateMediaSource(nullptr, mediaSource.GetAddressOf());
    if (FAILED(hr)) {
      throw std::runtime_error("Failed to create media source");
    }

    // Create the media player
    hr = MFCreateMediaPlayer(mediaSource.Get(), nullptr, mediaPlayer.GetAddressOf());
    if (FAILED(hr)) {
      throw std::runtime_error("Failed to create media player");
    }

    // Configure the decoder
    hr = MFCreateTransform(MFTransformType::MFTransformType_VideoDecoder, decoder.GetAddressOf());
    if (FAILED(hr)) {
      throw std::runtime_error("Failed to create video decoder");
    }

    // Set media type for the decoder
    hr = MFCreateMediaType(mediaType.GetAddressOf());
    if (FAILED(hr)) {
      throw std::runtime_error("Failed to create media type");
    }

    // Set up media type properties here, such as width, height, format, etc.
    // Note: You will need to set the actual media type based on your video source.

    hr = decoder->SetInputType(0, mediaType.Get(), 0);
    if (FAILED(hr)) {
      throw std::runtime_error("Failed to set input type for decoder");
    }
  }

  void VideoPlayer::play() {
    mediaPlayer->Play();
  }

  void VideoPlayer::pause() {
    mediaPlayer->Pause();
  }

  void VideoPlayer::stop() {
    mediaPlayer->Stop();
  }

  void VideoPlayer::seek(double seconds) {
    mediaPlayer->Seek(seconds);
  }

  double VideoPlayer::getDuration() {
    // Get the duration of the media
    return mediaPlayer->GetDuration();
  }

  double VideoPlayer::getCurrentTime() {
    return mediaPlayer->GetCurrentTime();
  }

  void VideoPlayer::renderFrame(SDL_Renderer* renderer, SDL_Rect& contentArea) {
    if (!mediaPlayer) {
      return; // Player not initialized
    }

    // Get the current video frame
    Microsoft::WRL::ComPtr<IMFMediaBuffer> mediaBuffer;
    HRESULT hr = mediaPlayer->GetCurrentFrame(mediaBuffer.GetAddressOf());

    if (SUCCEEDED(hr) && mediaBuffer) {
      // Lock the buffer to access the raw pixel data
      BYTE* pData = nullptr;
      DWORD maxLength = 0;
      DWORD currentLength = 0;

      hr = mediaBuffer->Lock(&pData, &maxLength, &currentLength);
      if (SUCCEEDED(hr) && pData) {
        // Create an SDL texture if it doesn't exist
        if (!texture) {
          texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, /* width */, /* height */);
        }

        // Update the SDL texture with the pixel data from the Media Foundation buffer
        SDL_UpdateTexture(texture, NULL, pData, /* bytes per row */);

        // Render the texture to the screen
        SDL_RenderCopy(renderer, texture, NULL, &contentArea);

        // Unlock the media buffer
        mediaBuffer->Unlock();
      }
    }
  }

  void VideoPlayer::render(SDL_Renderer* renderer, SDL_Rect& contentArea) {
    renderFrame(renderer, contentArea);
  }

  void VideoPlayer::setLocationCallback(std::function<void(float currentTime, float maxTime)> callback) {
    locationCallback = callback;
  }

  void VideoPlayer::setRepeat(bool value) {
    repeatMode = value;
  }

  Platform::Video::Video(const std::string& absolutePath) {
    _osPlayer = new VideoPlayer(absolutePath);
  }

  Platform::Video::~Video() {
    delete (VideoPlayer*)_osPlayer;
  }

  void Platform::Video::play() {
    ((VideoPlayer*)_osPlayer)->play();
  }

  void Platform::Video::pause() {
    ((VideoPlayer*)_osPlayer)->pause();
  }

  void Platform::Video::stop() {
    ((VideoPlayer*)_osPlayer)->stop();
  }

  void Platform::Video::seek(double seconds) {
    ((VideoPlayer*)_osPlayer)->seek(seconds);
  }

  double Platform::Video::getDuration() const {
    return ((VideoPlayer*)_osPlayer)->getDuration();
  }

  double Platform::Video::getCurrentTime() const {
    return ((VideoPlayer*)_osPlayer)->getCurrentTime();
  }

  void Platform::Video::setLocationCallback(std::function<void(float currentTime, float maxTime)> callback) {
    ((VideoPlayer*)_osPlayer)->setLocationCallback(callback);
  }

  void Platform::Video::setRepeat(bool value) {
    ((VideoPlayer*)_osPlayer)->setRepeat(value);
  }

  void Platform::Video::render(SDL_Renderer* renderer, SDL_Rect& contentArea) {
    ((VideoPlayer*)_osPlayer)->render(renderer, contentArea);
  }

} // namespace SGI
