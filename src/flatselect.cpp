#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatselect.h"
#include "fontbook.h"
#include "window.h"

namespace SGI {
  std::shared_ptr<FlatSelect> FlatSelect::create()
  {
    std::shared_ptr<FlatSelect> widget = std::make_shared<FlatSelect>(FlatSelect());
    widget->_self = widget;
    widget->setConstraintFixed(SGI::Widget::ConstraintType::Height, 36);
    widget->setConstraintAuto(SGI::Widget::ConstraintType::Width);
    widget->setPadding(12, 12, 6, 6);

    return widget;
  }

  FlatSelect::~FlatSelect()
  {
    if (_valueTexture != nullptr) {
      SDL_DestroyTexture(_valueTexture);
    }
    if (_noOptionsTexture != nullptr) {
      SDL_DestroyTexture(_noOptionsTexture);
    }
  }

  std::string FlatSelect::addChangeListener(const Widget::Callback& handler)
  {
    std::string id;
    
    do {
      id = _generateShortCode();
    } while (_inputHandelers.find(id) != _inputHandelers.end());
    _inputHandelers[id] = handler;

    return id;
  }

  void FlatSelect::removeChangeListener(const std::string& id)
  {
    _inputHandelers.erase(id);
  }

  std::string FlatSelect::getFontName()
  {
    return _fontName;
  }

  double FlatSelect::getFontSize()
  {
    return _fontSize;
  }

  std::string FlatSelect::getValue()
  {
    return _value;
  }

  bool FlatSelect::processEvent(const SDL_Event *event)
  {
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
      if (isMouseOver()) {
        _opened = !_opened;
      } else if (_opened && !_isMouseOverOverlay()) {
        _opened = false;
      }
    } else if (event->type == SDL_EVENT_MOUSE_WHEEL && _opened) {
      if (_isMouseOverOverlay() && _optionsHeight > _optionsContent.h) {
        _optionsScrollOffset -= event->wheel.y * 3;
        if (_optionsScrollOffset < 0) {
          _optionsScrollOffset = 0;
        }
        if (_optionsScrollOffset > _optionsHeight - _optionsContent.h) {
          _optionsScrollOffset = _optionsHeight - _optionsContent.h;
        }
        _updateSelected();
      }
    } else if (event->type == SDL_EVENT_MOUSE_MOTION) {
      if (_isMouseOverOverlay()) {
        _updateSelected();
      } else {
        _mouseOverOptionIndex = -1;
      }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      if (_opened && _isMouseOverOverlay() && _mouseOverOptionIndex != -1) {
        _value = _options[_mouseOverOptionIndex];
        _updateLabel();
        _opened = false;
        _mouseOverOptionIndex = -1;
        _optionsScrollOffset = 0;

        for (const auto& [id, handler] : _inputHandelers) {
          handler(_root, _self);
        }
      }
    }
    return Widget::processEvent(event);
  }


  void FlatSelect::setBorderColor(const SDL_Color &color) {
    _borderColor = color;
  }

  void FlatSelect::setFillColor(const SDL_Color &color) {
    _fillColor = color;
  }

  void FlatSelect::setFontColor(const SDL_Color &color) {
    _textColor = color;
    _updateLabel();
    _updateNoOptionsLadel();
    _updateOptionsLadel();
  }

  void FlatSelect::setFontName(const std::string& fontName)
  {
    _fontName = fontName;
    _updateLabel();
    _updateNoOptionsLadel();
    _updateOptionsLadel();
  }

  void FlatSelect::setFontSize(double fontSize)
  {
    if (fontSize > 0) {
      _fontSize = fontSize;
    } else {
      _fontSize = 1;
    }
    _updateLabel();
    _updateNoOptionsLadel();
    _updateOptionsLadel();
  }

  void FlatSelect::setNoOptionsLabel(const std::string &value)
  {
    if (value.empty()) {
      _noOptionsText = "No data available";
    } else {
      _noOptionsText = value;
    }
    _updateNoOptionsLadel();
  }

  void FlatSelect::setOptions(const std::vector<std::string>& options)
  {
    _options = options;
    setValue(_options[0]);
    _updateOptionsLadel();
  }

  void FlatSelect::setRadius(int value)
  {
    if (value < 0) {
      return;
    }
    _radius = value;
  }

  void FlatSelect::setValue(const std::string& value)
  {
    if (_value != value) {
      _value = value;
      _updateLabel();
    }
  }

  void FlatSelect::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _textColor = theme.primary.textColor;
    _borderColor = theme.primary.borderColor;
    _fillColor = theme.primary.fillColor;
    _updateLabel();
    _updateOptionsLadel();
  }

  bool FlatSelect::_isMouseOverOverlay()
  {
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseX > _optionsBounds.x && mouseX < _optionsBounds.x + _optionsBounds.w &&
        mouseY > _optionsBounds.y && mouseY < _optionsBounds.y + _optionsBounds.h) {
      return true;
    }
    return false;
  }

  void FlatSelect::_render(double deltaTime)
  {
    if (getRenderer() == nullptr) {
      return;
    }

    if (_valueTexture == nullptr && !_value.empty()) {
      _updateLabel();
    }

    SDL_Rect ca = getContentArea();
    SDL_FRect labelPos;
    labelPos.x = ca.x;
    labelPos.y = ca.y;
    labelPos.h = _valueHeight;
    labelPos.w = _valueWidth;

    SDL_Rect labelClip;
    labelClip.x = ca.x;
    labelClip.y = ca.y;
    labelClip.w = ca.w - ca.h - _padding.right;
    labelClip.h = ca.h;

    SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
    _drawRoundedRect(getRenderer().get(), _bounds, _borderColor, _fillColor, true, 1, _radius);
    SDL_SetRenderDrawColor(getRenderer().get(), _borderColor.r, _borderColor.g, _borderColor.b, _borderColor.a);
    SDL_RenderLine(getRenderer().get(), ca.x + ca.w - ca.h, ca.y, ca.x + ca.w - ca.h, ca.y + ca.h);

    SDL_Point center;
    center.x = ca.x + ca.w - (ca.h / 2) + (_padding.right / 2);
    center.y = ca.y + (ca.h / 2);
    _drawChevron(getRenderer().get(), center, ca.h / 3 * 2, _opened ? 0 : 180, _borderColor, _borderColor, true, 1);

    SDL_SetRenderClipRect(getRenderer().get(), &labelClip);
    SDL_RenderTexture(getRenderer().get(), _valueTexture, NULL, &labelPos);
    SDL_SetRenderClipRect(getRenderer().get(), nullptr);

    Widget::_render(deltaTime);
  }

  void FlatSelect::_renderOverlay(double deltaTime)
  {
    if (getRenderer() == nullptr) {
      return;
    }
    
    if (!_opened) {
      return;
    }

    if (_noOptionsTexture == nullptr && !_noOptionsText.empty()) {
      _updateNoOptionsLadel();
    }

    if (_optionsTexture == nullptr && !_options.empty()) {
      _updateOptionsLadel();
    }

    _optionsBounds.x = _bounds.x;
    _optionsBounds.y = _bounds.y + _bounds.h;
    _optionsBounds.w = _bounds.w;

    if (_options.size() == 0) {
      _optionsBounds.h = _noOptionsHeight;
    } else {
      int displayItems = (_options.size() < 5 ? _options.size() : 5);
      _optionsBounds.h = _optionItemHeight * displayItems + ((displayItems - 1) * _optionItemSpace);
    }
    _optionsBounds.h += _padding.top + _padding.bottom;

    SDL_SetRenderClipRect(getRenderer().get(), &_optionsBounds);
    _drawRoundedRect(getRenderer().get(), _optionsBounds, _borderColor, _fillColor, true, 1, _radius);

    _optionsContent.x = _optionsBounds.x + _padding.left;
    _optionsContent.y = _optionsBounds.y + _padding.top;
    _optionsContent.w = _optionsBounds.w - _padding.left - _padding.right;
    _optionsContent.h = _optionsBounds.h - _padding.top - _padding.bottom;

    SDL_FRect optionsPos;
    optionsPos.x = _optionsContent.x;
    optionsPos.y = _optionsContent.y - (int)_optionsScrollOffset;
    optionsPos.h = _options.size() == 0 ? _noOptionsHeight : _optionsHeight;
    optionsPos.w = _options.size() == 0 ? _noOptionsWidth : _optionsWidth;

    if (_mouseOverOptionIndex >= 0) {
      SDL_FRect highlight;
      highlight.x = optionsPos.x - _padding.left / 2;
      highlight.y = optionsPos.y + (_mouseOverOptionIndex * (_optionItemHeight + _optionItemSpace));
      highlight.w = _optionsContent.w + (_padding.left / 2) + (_padding.right / 2);
      highlight.h = _optionItemHeight;

      SDL_SetRenderDrawColor(getRenderer().get(), _borderColor.r, _borderColor.g, _borderColor.b, _borderColor.a);
      SDL_RenderFillRect(getRenderer().get(), &highlight);
    }

    SDL_SetRenderClipRect(getRenderer().get(), &_optionsContent);
    SDL_RenderTexture(getRenderer().get(), _options.size() == 0 ? _noOptionsTexture : _optionsTexture, NULL, &optionsPos);
  }

  void FlatSelect::_updateLabel()
  {
    if (_valueTexture != nullptr) {
      SDL_DestroyTexture(_valueTexture);
    }
    _valueTexture = nullptr;
    _valueHeight = 0;
    _valueWidth = 0;

    if (getRenderer() == nullptr) {
      return;
    }

    if (_fontName.empty() || _value.empty() || getRenderer() == nullptr) {
      return;
    }

    std::shared_ptr<SDL_Surface> textSurface = FontBook::render(_fontName, _fontSize, _value, _textColor);
    if(!textSurface) {
      ERROR(FlatSelect, "Surface not created: %s", SDL_GetError());
      return;
    }

    _valueTexture = SDL_CreateTextureFromSurface(getRenderer().get(), textSurface.get());
    if (!_valueTexture) {
      ERROR(FlatSelect, "Texture not created: %s", SDL_GetError());
      return;
    }

    SDL_GetTextureSize(_valueTexture, &_valueWidth, &_valueHeight);
  }

  void FlatSelect::_updateNoOptionsLadel()
  {
    if (_noOptionsTexture != nullptr) {
      SDL_DestroyTexture(_noOptionsTexture);
    }
    _noOptionsTexture = nullptr;
    _noOptionsHeight = 0;
    _noOptionsWidth = 0;

    if (getRenderer() == nullptr) {
      return;
    }

    if (_fontName.empty() || _noOptionsText.empty() || getRenderer() == nullptr) {
      return;
    }

    std::shared_ptr<SDL_Surface> textSurface = FontBook::render(_fontName, _fontSize, _noOptionsText, _textColor);
    if(!textSurface) {
      ERROR(FlatSelect, "Surface not created: %s", SDL_GetError());
      return;
    }

    _noOptionsTexture = SDL_CreateTextureFromSurface(getRenderer().get(), textSurface.get());
    if (!_noOptionsTexture) {
      ERROR(FlatSelect, "No options texture not created: %s", SDL_GetError());
      return;
    }
    SDL_GetTextureSize(_noOptionsTexture, &_noOptionsWidth, &_noOptionsHeight);
 }

  void FlatSelect::_updateOptionsLadel()
  {
    if (_optionsTexture != nullptr) {
      SDL_DestroyTexture(_optionsTexture);
    }
    _optionsTexture = nullptr;
    _optionItemHeight = 0;
    _optionsHeight = 0;
    _optionsWidth = 0;
    _optionsScrollOffset = 0;

    if (getRenderer() == nullptr) {
      return;
    }

    if (_fontName.empty() || _options.empty() || getRenderer() == nullptr) {
      return;
    }

    for (size_t i = 0; i < _options.size(); ++i) {
      int w, h;
      FontBook::measure(_fontName, _fontSize, _options[i], &w, &h);
      _optionsHeight += h;
      if (i < _options.size() - 1) {
        _optionsHeight += _optionItemSpace;
      }
      if (h > _optionItemHeight) {
        _optionItemHeight = h;
      }
      if (w > _optionsWidth) {
        _optionsWidth = w;
      }
    }

    _optionsTexture = SDL_CreateTexture(getRenderer().get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, _optionsWidth, _optionsHeight);
    if (!_optionsTexture) {
      ERROR(FlatSelect, "No options texture not created: %s", SDL_GetError());
      return;
    }

    SDL_SetRenderTarget(getRenderer().get(), _optionsTexture);
    SDL_SetRenderDrawColor(getRenderer().get(), 0, 0, 0, 0);
    SDL_RenderClear(getRenderer().get());

    float currentY = 0;
    for (size_t i = 0; i < _options.size(); ++i) {
      std::shared_ptr<SDL_Surface> surface = FontBook::render(_fontName, _fontSize, _options[i], _textColor);
      SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer().get(), surface.get());
      
      SDL_FRect dstRect;
      dstRect.x = 0;
      dstRect.y = currentY;
      dstRect.w = surface->w;
      dstRect.h = surface->h;
      SDL_RenderTexture(getRenderer().get(), texture, nullptr, &dstRect);
      
      currentY += _optionItemHeight;
      if (i != _options.size() - 1) {
        currentY += _optionItemSpace;
      }
      SDL_DestroyTexture(texture);
    }
    SDL_SetRenderTarget(getRenderer().get(), nullptr);
  }

  void FlatSelect::_updateSelected()
  {
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    int mouseYOffset = mouseY - _optionsContent.y + _optionsScrollOffset;
    if (mouseYOffset < 0) {
      mouseYOffset = 0;
    }
    _mouseOverOptionIndex = mouseYOffset / (_optionItemHeight + _optionItemSpace);
  }
}
