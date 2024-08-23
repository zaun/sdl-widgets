#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatoption.h"
#include "fontbook.h"

namespace SGI {
  std::shared_ptr<FlatOption> FlatOption::create()
  {
    std::shared_ptr<FlatOption> widget = std::make_shared<FlatOption>(FlatOption());
    widget.get()->_self = widget;
    widget.get()->setPadding(12, 12, 6, 6);

    return widget;
  }

  std::shared_ptr<FlatOption> FlatOption::create(std::string label)
  {
    std::shared_ptr<FlatOption> widget = FlatOption::create();
    widget.get()->setLabel(label);

    return widget;
  }

  FlatOption::~FlatOption()
  {
    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
  }

  std::string FlatOption::getFontName()
  {
    return _fontName;
  }

  double FlatOption::getFontSize()
  {
    return _fontSize;
  }

  std::string FlatOption::getLabel()
  {
    return _label;
  }

  bool FlatOption::getValue()
  {
    return _value;
  }

  bool FlatOption::processEvent(const SDL_Event *event)
  {
    bool ret = Widget::processEvent(event);
    switch (event->type) {
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        if (isMouseOver()) {
          _value = !_value;
        }
      }
    }

    return ret;
  }

  void FlatOption::setFontName(const std::string& fontName)
  {
    _fontName = fontName;
    _updateLabel();
  }

  void FlatOption::setFontSize(double fontSize)
  {
    if (fontSize > 0) {
      _fontSize = fontSize;
    } else {
      _fontSize = 1;
    }
    _updateLabel();
  }

  void FlatOption::setLabel(const std::string& value)
  {
    if (_label != value) {
      _label = value;
      _updateLabel();
    }
  }

  void FlatOption::setValue(bool value)
  {
    if (value != _value) {
      _value = value;
    }
  }

  void FlatOption::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _textColor = theme.primaryText;
    _borderColor = theme.primaryBorder;
    _fillColor = theme.primaryBorder;
    _updateLabel();
  }

  void FlatOption::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }

    if (!_textTexture) {
      _updateLabel();
    }

    int optionSize = _textHeight < 16 ? 16 : _textHeight;

    SDL_Rect ca = getContentArea();
    SDL_FRect labelPos;
    labelPos.x = ca.x + optionSize + 6;
    labelPos.y = ca.y + (ca.h - _textHeight) / 2;
    labelPos.h = _textHeight;
    labelPos.w = _textWidth;

    SDL_Rect optionRect;
    optionRect.x = ca.x;
    optionRect.y = ca.y + (ca.h - optionSize) / 2;
    optionRect.w = optionSize;
    optionRect.h = optionSize;

    SDL_Rect optionValueRect;
    optionValueRect.x = ca.x + 4;
    optionValueRect.y = ca.y + 4 + (ca.h - optionSize) / 2;
    optionValueRect.w = optionSize - 8;
    optionValueRect.h = optionSize - 8;

    SDL_SetRenderClipRect(getRenderer().get(), &ca);
    _drawRoundedRect(getRenderer().get(), optionRect, _borderColor, _textColor, false, 1, optionRect.w / 2);
    if (_value) {
      _drawRoundedRect(getRenderer().get(), optionValueRect, _fillColor, _fillColor, true, 1, optionValueRect.w / 2 - 1);
    }
    SDL_RenderTexture(getRenderer().get(), _textTexture, NULL, &labelPos);
    SDL_SetRenderClipRect(getRenderer().get(), nullptr);

    Widget::_render(deltaTime);
  }

  void FlatOption::_updateLabel()
  {
    if (getRenderer() == nullptr) {
      return;
    }

    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
    _textTexture = nullptr;

    if (_fontName.empty() || _label.empty() || getRenderer() == nullptr) {
      return;
    }

    std::shared_ptr<SDL_Surface> textSurface = FontBook::render(_fontName, _fontSize, _label, _textColor);
    if(!textSurface) {
      ERROR(FLATLABEL, "Surface not created: %s", SDL_GetError());
      return;
    }

    _textTexture = SDL_CreateTextureFromSurface(getRenderer().get(), textSurface.get());
    if (!_textTexture) {
      ERROR(FLATLABEL, "Texture not created: %s", SDL_GetError());
      return;
    }

    SDL_GetTextureSize(_textTexture, &_textWidth, &_textHeight);
  }
}