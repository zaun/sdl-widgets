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
    widget.get()->_self = widget;
    widget.get()->_init();

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
  }

  void FlatVideo::play()
  {
    _video.get()->play();
  }

  void FlatVideo::setRepeat(bool value)
  {
    _video.get()->setRepeat(value);
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
    _borderColor = theme.primaryBorder;
    _fillColor = theme.primaryFill;
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
    _video.get()->render(getRenderer().get(), ca);
  }

}