#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatlabel.h"
#include "fontbook.h"

namespace SGI {
  std::shared_ptr<FlatLabel> FlatLabel::create()
  {
    std::shared_ptr<FlatLabel> widget = std::make_shared<FlatLabel>(FlatLabel());
    widget->_self = widget;
    widget->setPadding(12, 12, 6, 6);

    return widget;
  }

  std::shared_ptr<FlatLabel> FlatLabel::create(std::string label)
  {
    std::shared_ptr<FlatLabel> widget = FlatLabel::create();
    widget->setLabel(label);

    return widget;
  }

  FlatLabel::~FlatLabel()
  {
    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
  }

  std::string FlatLabel::getFontName()
  {
    return _fontName;
  }

  double FlatLabel::getFontSize()
  {
    return _fontSize;
  }

  FlatLabel::TextJustification FlatLabel::getJustification()
  {
    return _justification;
  }

  std::string FlatLabel::getLabel()
  {
    return _label;
  }

  void FlatLabel::setFontName(const std::string& fontName)
  {
    _fontName = fontName;
    _updateLabel();
  }

  void FlatLabel::setFontSize(double fontSize)
  {
    if (fontSize > 0) {
      _fontSize = fontSize;
    } else {
      _fontSize = 1;
    }
    _updateLabel();
  }

  void FlatLabel::setJustification(FlatLabel::TextJustification justification)
  {
    _justification = justification;
  }

  void FlatLabel::setLabel(const std::string& value)
  {
    if (_label != value) {
      _label = value;
      _updateLabel();
    }
  }

  void FlatLabel::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _textColor = theme.primary.textColor;
    _updateLabel();
  }
  
  void FlatLabel::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }

    if (!_textTexture) {
      _updateLabel();
    }

    SDL_Rect ca = getContentArea();
    SDL_FRect labelPos;
    if (_justification == TextJustification::Center) {
      labelPos.x = ca.x + (ca.w - _textWidth) / 2;
    } else if (_justification == TextJustification::Right) {
      labelPos.x = ca.x + ca.w - _textWidth;
    } else {
      labelPos.x = ca.x;
    }
    labelPos.y = ca.y + (ca.h - _textHeight) / 2;
    labelPos.h = _textHeight;
    labelPos.w = _textWidth;

    SDL_SetRenderClipRect(getRenderer().get(), &ca);
    SDL_RenderTexture(getRenderer().get(), _textTexture, NULL, &labelPos);
    SDL_SetRenderClipRect(getRenderer().get(), nullptr);

    Widget::_render(deltaTime);
  }

  void FlatLabel::_updateLabel()
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
    _constraints.width.preferredValue = _textWidth + _padding.left + _padding.right;
    _constraints.height.preferredValue = _textHeight + _padding.top + _padding.bottom;
    _dirty = true;
  }
}