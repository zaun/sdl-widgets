#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "button.h"
#include "fontbook.h"
#include "window.h"

namespace SGI {
  std::shared_ptr<Button> Button::create()
  {
    std::shared_ptr<Button> widget = std::make_shared<Button>(Button());
    widget->_self = widget;
    widget->setConstraintFixed(SGI::Widget::ConstraintType::Height, 44);

    return widget;
  }

  void Button::_render(double deltaTime)
  {
    if (_mouseButtonState[1]) {
      if (!_buttonPressedTexture.empty()) {
        _root->renderTexture(_buttonPressedTexture, SDL_FRect{(float)_bounds.x, (float)_bounds.y, (float)_bounds.w, (float)_bounds.h});
      }
    } else if (isMouseOver()) {
      if (!_buttonHoverTexture.empty()) {
        _root->renderTexture(_buttonHoverTexture, SDL_FRect{(float)_bounds.x, (float)_bounds.y, (float)_bounds.w, (float)_bounds.h});
      }
    } else {
      if (!_buttonTexture.empty()) {
        _root->renderTexture(_buttonTexture, SDL_FRect{(float)_bounds.x, (float)_bounds.y, (float)_bounds.w, (float)_bounds.h});
      }
    }

    Widget::_render(deltaTime);
  }

  void Button::_updateLabel()
  {
    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
    _textTexture = nullptr;

    if (_fontName.empty() || _lable.empty() || getRenderer() == nullptr) {
      return;
    }

    _textSurface = FontBook::render(_fontName, _fontSize, _lable, _color);
    if(!_textSurface) {
      ERROR(LABEL, "Surface not created: %s", SDL_GetError());
      return;
    }

    _textTexture = SDL_CreateTextureFromSurface(getRenderer().get(), _textSurface.get());
    if (!_textTexture) {
      ERROR(LABEL, "Texture not created: %s", SDL_GetError());
      return;
    }

    SDL_GetTextureSize(_textTexture, &_textWidth, &_textHeight);
  }

}
