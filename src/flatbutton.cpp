#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatbutton.h"
#include "fontbook.h"
#include "window.h"

namespace SGI {
  std::shared_ptr<FlatButton> FlatButton::create()
  {
    std::shared_ptr<FlatButton> widget = std::make_shared<FlatButton>(FlatButton());
    widget->_self = widget;
    widget->setConstraintFixed(SGI::Widget::ConstraintType::Width, 100);
    widget->setConstraintFixed(SGI::Widget::ConstraintType::Height, 36);
    widget->setPadding(12, 12, 6, 6);

    return widget;
  }

  std::shared_ptr<FlatButton> FlatButton::create(std::string label)
  {
    std::shared_ptr<FlatButton> widget = FlatButton::create();
    widget->setLabel(label);

    return widget;
  }

  FlatButton::~FlatButton()
  {
    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
    if (_textTextureHover != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
    if (_textTexturePressed != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
  }

  std::string FlatButton::getFontName()
  {
    return _fontName;
  }

  double FlatButton::getFontSize()
  {
    return _fontSize;
  }

  std::string FlatButton::getLabel()
  {
    return _label;
  }

  void FlatButton::setBorderColor(
    const SDL_Color &color,
    const SDL_Color &colorHover,
    const SDL_Color &colorPressed
  ) {
    _borderColor = color;
    _borderColorHover = colorHover;
    _borderColorPressed = colorPressed;
  }

  void FlatButton::setFillColor(
    const SDL_Color &color,
    const SDL_Color &colorHover,
    const SDL_Color &colorPressed
  ) {
    _fillColor = color;
    _fillColorHover = colorHover;
    _fillColorPressed = colorPressed;
  }

  void FlatButton::setFontColor(
    const SDL_Color &color,
    const SDL_Color &colorHover,
    const SDL_Color &colorPressed
  ) {
    _textColor = color;
    _textColorHover = colorHover;
    _textColorPressed = colorPressed;
    _updateLabel();
  }

  void FlatButton::setFontName(const std::string& fontName)
  {
    _fontName = fontName;
    _updateLabel();
  }

  void FlatButton::setFontSize(double fontSize)
  {
    if (fontSize > 0) {
      _fontSize = fontSize;
    } else {
      _fontSize = 1;
    }
    _updateLabel();
  }

  void FlatButton::setLabel(const std::string& value)
  {
    if (_label != value) {
      _label = value;
      _updateLabel();
    }
  }

  void FlatButton::setOutline(bool value)
  {
    _outline = value;
  }

  void FlatButton::setRadius(int value)
  {
    if (value < 0) {
      return;
    }

    _radius = value;
  }

  void FlatButton::setTheme(std::string name)
  {

  }

  void FlatButton::_render(double deltaTime)
  {
    if (getRenderer() == nullptr) {
      return;
    }

    if (_textTexture == nullptr && !_label.empty()) {
      _updateLabel();
    }

    SDL_Rect ca = getContentArea();
    SDL_FRect labelPos;
    labelPos.x = ca.x + (ca.w - _textWidth) / 2;
    labelPos.y = ca.y + (ca.h - _textHeight) / 2;
    labelPos.h = _textHeight;
    labelPos.w = _textWidth;

    if (_mouseButtonState[1]) {
      SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
      _drawRoundedRect(getRenderer().get(), _bounds, _borderColorPressed, _fillColorPressed, true, 1, _radius);
      SDL_SetRenderClipRect(getRenderer().get(), &ca);
      SDL_RenderTexture(getRenderer().get(), _textTexturePressed, NULL, &labelPos);
    } else if (isMouseOver()) {
      SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
      _drawRoundedRect(getRenderer().get(), _bounds, _borderColorHover, _fillColorHover, true, 1, _radius);
      SDL_SetRenderClipRect(getRenderer().get(), &ca);
      SDL_RenderTexture(getRenderer().get(), _textTextureHover, NULL, &labelPos);
    } else {
      SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
      _drawRoundedRect(getRenderer().get(), _bounds, _borderColor, _fillColor, !_outline, 1, _radius);
      SDL_SetRenderClipRect(getRenderer().get(), &ca);
      SDL_RenderTexture(getRenderer().get(), _textTexture, NULL, &labelPos);
    }
  }

  void FlatButton::_updateLabel()
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
      ERROR(FLATBUTTON, "Surface not created: %s", SDL_GetError());
      return;
    }

    _textTexture = SDL_CreateTextureFromSurface(getRenderer().get(), textSurface.get());
    if (!_textTexture) {
      ERROR(FLATBUTTON, "Texture not created: %s", SDL_GetError());
      return;
    }

    textSurface = FontBook::render(_fontName, _fontSize, _label, _textColorHover);
    if(!textSurface) {
      ERROR(FLATBUTTON, "Surface not created: %s", SDL_GetError());
      return;
    }

    _textTextureHover = SDL_CreateTextureFromSurface(getRenderer().get(), textSurface.get());
    if (!_textTextureHover) {
      ERROR(FLATBUTTON, "Texture not created: %s", SDL_GetError());
      return;
    }

    textSurface = FontBook::render(_fontName, _fontSize, _label, _textColorPressed);
    if(!textSurface) {
      ERROR(FLATBUTTON, "Surface not created: %s", SDL_GetError());
      return;
    }

    _textTexturePressed = SDL_CreateTextureFromSurface(getRenderer().get(), textSurface.get());
    if (!_textTexturePressed) {
      ERROR(FLATBUTTON, "Texture not created: %s", SDL_GetError());
      return;
    }

    SDL_GetTextureSize(_textTexture, &_textWidth, &_textHeight);
  }

}
