#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "panel.h"
#include "window.h"

namespace SGI {
  std::shared_ptr<Panel> Panel::create()
  {
    std::shared_ptr<Panel> widget = std::make_shared<Panel>(Panel());
    widget.get()->_self = widget;
    widget.get()->setPadding(10, 10, 10, 10);

    return widget;
  }

  void Panel::setBackgroundTexture(const std::string textureName)
  {
    _backgroundTexture = textureName;
  }

  void Panel::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }

    if (!_backgroundTexture.empty()) {
      _root.get()->renderTexture(_backgroundTexture, SDL_FRect{(float)_bounds.x, (float)_bounds.y, (float)_bounds.w, (float)_bounds.h});
    }

    Container::_render(deltaTime);
  }

  void Panel::_renderOverlay(double deltaTime)
  {
    if (!_root) {
      return;
    }

    Container::_renderOverlay(deltaTime);
  }
}