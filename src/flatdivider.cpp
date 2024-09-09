#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatdivider.h"

namespace SGI {
  std::shared_ptr<FlatDivider> FlatDivider::create()
  {
    std::shared_ptr<FlatDivider> widget = std::make_shared<FlatDivider>(FlatDivider());
    widget->_self = widget;
    widget->setPadding(12, 12, 6, 6);

    return widget;
  }

  FlatDivider::FlatDivider()
  {
    _constraints.height.preferredValue = _size + _padding.top + _padding.bottom;
  };


  FlatDivider::Direction FlatDivider::getDirection()
  {
    return _direction;
  }

  int FlatDivider::getSize()
  {
    return _size;
  }

  void FlatDivider::setDirection(FlatDivider::Direction direction)
  {
    if (_direction != direction) {
      _direction = direction;
      if (_direction == Direction::Horizontal) {
        _constraints.width.preferredValue = -1;
        _constraints.height.preferredValue = _size + _padding.top + _padding.bottom;
      } else {
        _constraints.width.preferredValue = _size + _padding.top + _padding.bottom;
        _constraints.height.preferredValue = -1;
      }
      _dirty = true;
    }
  }

  void FlatDivider::setSize(int value)
  {
    if (_size != value)  {
      _size = value;
      if (_direction == Direction::Horizontal) {
        _constraints.width.preferredValue = -1;
        _constraints.height.preferredValue = _size + _padding.top + _padding.bottom;
      } else {
        _constraints.width.preferredValue = _size + _padding.top + _padding.bottom;
        _constraints.height.preferredValue = -1;
      }
      _dirty = true;
    }
  }

  void FlatDivider::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _borderColor = theme.colors.borderColor;
  }
  
  void FlatDivider::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }

    SDL_Rect ca = getContentArea();

    SDL_FRect line;
    if (_direction == Direction::Horizontal) {
      line.x = ca.x;
      line.y = ca.y + ((ca.h - _size) / 2);
      line.w = ca.w;
      line.h = _size;
    } else {
      line.x = ca.x + ((ca.w - _size) / 2);
      line.y = ca.y;
      line.w = _size;
      line.h = ca.h;
    }
  
    SDL_SetRenderClipRect(getRenderer().get(), &ca);
    SDL_SetRenderDrawColor(getRenderer().get(), _borderColor.r, _borderColor.g, _borderColor.b, _borderColor.a);
    SDL_RenderFillRect(getRenderer().get(), &line);
    SDL_SetRenderClipRect(getRenderer().get(), nullptr);

    Widget::_render(deltaTime);
  }
}