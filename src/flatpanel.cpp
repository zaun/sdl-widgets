#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatpanel.h"

namespace SGI {
  std::shared_ptr<FlatPanel> FlatPanel::create()
  {
    std::shared_ptr<FlatPanel> widget = std::make_shared<FlatPanel>(FlatPanel());
    widget->_self = widget;
    widget->setPadding(10, 10, 10, 10);

    return widget;
  }

  void FlatPanel::setBorderColor(const SDL_Color &color) {
    _borderColor = color;
  }

  void FlatPanel::setFillColor(const SDL_Color &color) {
    _fillColor = color;
  }

  void FlatPanel::setOutline(bool value)
  {
    _outline = value;
  }

  void FlatPanel::setRadius(int value)
  {
    if (value < 0) {
      return;
    }

    _radius = value;
  }

  void FlatPanel::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _borderColor = theme.colors.borderColor;
    _fillColor = theme.colors.backgroundColor;
  }
  
  void FlatPanel::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }

    SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
    _drawRoundedRect(getRenderer().get(), _bounds, _borderColor, _fillColor, !_outline, 1, _radius);

    Container::_render(deltaTime);
  }

  void FlatPanel::_renderOverlay(double deltaTime)
  {
    if (!_root) {
      return;
    }

    Container::_renderOverlay(deltaTime);
  }
}