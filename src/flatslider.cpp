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
    widget.get()->_self = widget;

    return widget;
  }

  std::shared_ptr<FlatSlider> FlatSlider::create(int min, int max, int value)
  {
    std::shared_ptr<FlatSlider> widget = FlatSlider::create();
    widget.get()->setMinValue(min);
    widget.get()->setMaxValue(max);
    widget.get()->setValue(0);

    return widget;
  }

  FlatSlider::~FlatSlider()
  {

  }

  int FlatSlider::getValue()
  {
    return _value;
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
    _borderColor = theme.primaryBorder;
    _fillColor = theme.primaryFill;
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
    Widget::_render(deltaTime);
  }

  void FlatSlider::_updateHandlePosition()
  {
    SDL_Rect ca = getContentArea();
    if (_orientation == Orientation::Horizontal) {
      int handleWidth = ca.w / 10;
      _handleRect.w = handleWidth;
      _handleRect.h = ca.h;
      _handleRect.x = ca.x + (_value - _minValue) / (_maxValue - _minValue) * (ca.w - handleWidth);
      _handleRect.y = ca.y;
    } else {
      int handleHeight = ca.h / 10;
      _handleRect.w = ca.w;
      _handleRect.h = handleHeight;
      _handleRect.x = ca.x;
      _handleRect.y = ca.y + (_value - _minValue) / (_maxValue - _minValue) * (ca.h - handleHeight);
    }
  }
}