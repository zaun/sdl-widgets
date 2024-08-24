#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatvideo.h"
#include "platform.h"

namespace SGI {
  
  std::shared_ptr<FlatVideo> FlatVideo::create()
  {
    std::shared_ptr<FlatVideo> widget(new FlatVideo(), [](FlatVideo* ptr) { delete ptr; });
    widget->_self = widget;
    widget->_init();

    return widget;
  }

  FlatVideo::FlatVideo()
  {
    _resourcePath = SDL_GetBasePath();
  }

  FlatVideo::~FlatVideo()
  {

  }

  void FlatVideo::loadFile(std::string fileName)
  {
    std::string fullPath = _resourcePath + fileName;
    _video = std::make_unique<Platform::Video>(fullPath);

    if (_callback) {
      _video->setLocationCallback([this](float currentTime, float maxTime) {
        _callback(_root, _self, currentTime, maxTime);
      });
    }
  }

  void FlatVideo::play()
  {
    _video->play();
  }

  void FlatVideo::setLocationCallback(std::function<void(std::shared_ptr<Window>, std::shared_ptr<Widget>, float currentTime, float maxTime)> callback)
  {
    if (!_video) {
      return;
    }
    _callback = callback;

    _video->setLocationCallback([this](float currentTime, float maxTime) {
      _callback(_root, _self, currentTime, maxTime);
    });
  }

  void FlatVideo::setRepeat(bool value)
  {
    _video->setRepeat(value);
  }

  void FlatVideo::setResourcePath(std::string path)
  {
    _resourcePath = path;
    if (_resourcePath.empty() || _resourcePath.back() == '/') {
      _resourcePath += "/";
    }
  }

  void FlatVideo::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _borderColor = theme.primary.borderColor;
    _fillColor = theme.primary.backgroundColor;
  }

  void FlatVideo::_cleanup()
  {

  }

  void FlatVideo::_init()
  {

  }
  
  void FlatVideo::_render(double deltaTime)
  {
    SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
    _drawRoundedRect(getRenderer().get(), _bounds, _borderColor, _fillColor, true, 1, 6);

    SDL_Rect ca = getContentArea();
    _video->render(getRenderer().get(), ca);
  }

}