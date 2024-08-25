#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatslider.h"
#include "fontbook.h"

namespace SGI {
  std::shared_ptr<FlatSlider> FlatSlider::create()
  {
    std::shared_ptr<FlatSlider> widget = std::make_shared<FlatSlider>(FlatSlider());
    widget->_self = widget;

    return widget;
  }

  std::shared_ptr<FlatSlider> FlatSlider::create(int min, int max, int value)
  {
    std::shared_ptr<FlatSlider> widget = FlatSlider::create();
    widget->setMinValue(min);
    widget->setMaxValue(max);
    widget->setValue(0);

    return widget;
  }

  FlatSlider::~FlatSlider()
  {

  }

  std::string FlatSlider::addChangeListener(const Widget::Callback& handler)
  {
    std::string id;
    
    do {
      id = _generateShortCode();
    } while (_changeHandelers.find(id) != _changeHandelers.end());
    _changeHandelers[id] = handler;

    return id;
  }

  void FlatSlider::removeChangeListener(const std::string& id)
  {
    _changeHandelers.erase(id);
  }

  int FlatSlider::getValue()
  {
    return _value;
  }

  bool FlatSlider::processEvent(const SDL_Event *event)
  {
    SDL_Point mousePoint;
    mousePoint.x = event->button.x;
    mousePoint.y = event->button.y;

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      if (SDL_PointInRect(&mousePoint, &_handleRect)) {
        _dragging = true;
        setFocused(true);
      }
    }
    
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
      _dragging = false;
    }
    
    if (event->type == SDL_EVENT_MOUSE_MOTION && _dragging) {
      if (_orientation == Orientation::Horizontal) {
        double newValue = _minValue + ((mousePoint.x - getContentArea().x) / (double)getContentArea().w) * (_maxValue - _minValue);
        setValue(newValue);
        for (const auto& [id, handler] : _changeHandelers) {
          handler(_root, _self);
        }
      } else {
        double newValue = _minValue + ((mousePoint.y - getContentArea().y) / (double)getContentArea().h) * (_maxValue - _minValue);
        setValue(newValue);
        for (const auto& [id, handler] : _changeHandelers) {
          handler(_root, _self);
        }
      }
      return true;
    }
    
    Widget::processEvent(event);
    return false;
  }

  void FlatSlider::setMaxValue(int maxValue)
  {
    _maxValue = maxValue;
    setValue(_value);
  }

  void FlatSlider::setMinValue(int minValue)
  {
    _minValue = minValue;
    setValue(_value);
  }

  void FlatSlider::setOrientation(Orientation orientation)
  {
    _orientation = orientation;
  }

  void FlatSlider::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _trackBorder = theme.primary.borderColor;
    _trackFill = theme.primary.fillColor;
    _handleBorder = theme.primary.accentBorderColor;
    _handleFill = theme.primary.accentFillColor;
  }

  void FlatSlider::setValue(int value)
  {
    _value = std::clamp(value, _minValue, _maxValue);
  }
  
  void FlatSlider::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }
    SDL_Rect ca = getContentArea();

    if (_orientation == Orientation::Horizontal) {

      _handleRect.x = ca.x + (float)(_value - _minValue) / (_maxValue - _minValue) * (ca.w - _handleSize);
      _handleRect.y = ca.y + (ca.h - _handleSize) / 2;
      _handleRect.w = _handleSize;
      _handleRect.h = _handleSize;

      SDL_Rect trackRect;
      trackRect.x = ca.x;
      trackRect.y = ca.y + (ca.h - _trackSize) / 2;
      trackRect.w = ca.w;
      trackRect.h  = _trackSize;
      _drawRoundedRect(getRenderer().get(), trackRect, _trackBorder, _trackFill, true, 2, _trackSize / 2);
    } else {
      _handleRect.x = ca.x + (ca.w - _handleSize) / 2;
      _handleRect.y = ca.y + (_value - _minValue) / (_maxValue - _minValue) * (ca.h - _handleSize);
      _handleRect.w = _handleSize;
      _handleRect.h = _handleSize;
    }

    _drawRoundedRect(getRenderer().get(), _handleRect, _handleBorder, _handleFill, true, 2, _handleSize / 2);

    Widget::_render(deltaTime);
  }
}