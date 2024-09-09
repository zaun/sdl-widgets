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
    widget->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 100);
    widget->setConstraintMinimum(SGI::Widget::ConstraintType::Height, 36);
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

  bool FlatButton::processEvent(const SDL_Event *event)
  {
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      if (_disabled) {
        return false;
      }
    }

    return Widget::processEvent(event);
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

  void FlatButton::setDisabled(bool value) 
  {
    _disabled = value;
    setStyle(_style);
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

  void FlatButton::setIcon(const std::string& value)
  {
    if (_icon != value) {
      _icon = value;
      _updateLabel();
    }
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
    setStyle(_style);
  }

  void FlatButton::setRadius(int value)
  {
    if (value < 0) {
      return;
    }

    _radius = value;
  }

  void FlatButton::setStyle(const Style value)
  {
    _style = value;

    if (_disabled) {
      switch (_style) {
        case Style::Primary:
            _textColor = _theme.buttons.primary.disabledColor.text;
            _textColorHover = _theme.buttons.primary.disabledColor.text;
            _textColorPressed = _theme.buttons.primary.disabledColor.text;
            _borderColor = _theme.buttons.primary.disabledColor.border;
            _borderColorHover = _theme.buttons.primary.disabledColor.border;
            _borderColorPressed = _theme.buttons.primary.disabledColor.border;
            _fillColor = _theme.buttons.primary.disabledColor.fill;
            _fillColorHover = _theme.buttons.primary.disabledColor.fill;
            _fillColorPressed = _theme.buttons.primary.disabledColor.fill;
          break;
        case Style::Secondary:
            _textColor = _theme.buttons.secondary.disabledColor.text;
            _textColorHover = _theme.buttons.secondary.disabledColor.text;
            _textColorPressed = _theme.buttons.secondary.disabledColor.text;
            _borderColor = _theme.buttons.secondary.disabledColor.border;
            _borderColorHover = _theme.buttons.secondary.disabledColor.border;
            _borderColorPressed = _theme.buttons.secondary.disabledColor.border;
            _fillColor = _theme.buttons.secondary.disabledColor.fill;
            _fillColorHover = _theme.buttons.secondary.disabledColor.fill;
            _fillColorPressed = _theme.buttons.secondary.disabledColor.fill;
          break;
        case Style::Info:
            _textColor = _theme.buttons.info.disabledColor.text;
            _textColorHover = _theme.buttons.info.disabledColor.text;
            _textColorPressed = _theme.buttons.info.disabledColor.text;
            _borderColor = _theme.buttons.info.disabledColor.border;
            _borderColorHover = _theme.buttons.info.disabledColor.border;
            _borderColorPressed = _theme.buttons.info.disabledColor.border;
            _fillColor = _theme.buttons.info.disabledColor.fill;
            _fillColorHover = _theme.buttons.info.disabledColor.fill;
            _fillColorPressed = _theme.buttons.info.disabledColor.fill;
          break;
        case Style::Warning:
            _textColor = _theme.buttons.warning.disabledColor.text;
            _textColorHover = _theme.buttons.warning.disabledColor.text;
            _textColorPressed = _theme.buttons.warning.disabledColor.text;
            _borderColor = _theme.buttons.warning.disabledColor.border;
            _borderColorHover = _theme.buttons.warning.disabledColor.border;
            _borderColorPressed = _theme.buttons.warning.disabledColor.border;
            _fillColor = _theme.buttons.warning.disabledColor.fill;
            _fillColorHover = _theme.buttons.warning.disabledColor.fill;
            _fillColorPressed = _theme.buttons.warning.disabledColor.fill;
          break;
        case Style::Danger:
            _textColor = _theme.buttons.danger.disabledColor.text;
            _textColorHover = _theme.buttons.danger.disabledColor.text;
            _textColorPressed = _theme.buttons.danger.disabledColor.text;
            _borderColor = _theme.buttons.danger.disabledColor.border;
            _borderColorHover = _theme.buttons.danger.disabledColor.border;
            _borderColorPressed = _theme.buttons.danger.disabledColor.border;
            _fillColor = _theme.buttons.danger.disabledColor.fill;
            _fillColorHover = _theme.buttons.danger.disabledColor.fill;
            _fillColorPressed = _theme.buttons.danger.disabledColor.fill;
          break;
      }
    } else if (_outline) {
      switch (_style) {
        case Style::Primary:
            _textColor = _theme.buttons.primary.defaultColor.text;
            _textColorHover = _theme.buttons.primary.hoverColor.text;
            _textColorPressed = _theme.buttons.primary.pressedColor.text;
            _borderColor = _theme.buttons.primary.defaultColor.fill;
            _borderColorHover = _theme.buttons.primary.hoverColor.border;
            _borderColorPressed = _theme.buttons.primary.pressedColor.border;
            _fillColor = _theme.buttons.primary.defaultColor.fill;
            _fillColorHover = _theme.buttons.primary.hoverColor.fill;
            _fillColorPressed = _theme.buttons.primary.pressedColor.fill;
          break;
        case Style::Secondary:
            _textColor = _theme.buttons.secondary.defaultColor.text;
            _textColorHover = _theme.buttons.secondary.hoverColor.text;
            _textColorPressed = _theme.buttons.secondary.pressedColor.text;
            _borderColor = _theme.buttons.secondary.defaultColor.fill;
            _borderColorHover = _theme.buttons.secondary.hoverColor.border;
            _borderColorPressed = _theme.buttons.secondary.pressedColor.border;
            _fillColor = _theme.buttons.secondary.defaultColor.fill;
            _fillColorHover = _theme.buttons.secondary.hoverColor.fill;
            _fillColorPressed = _theme.buttons.secondary.pressedColor.fill;
          break;
        case Style::Info:
            _textColor = _theme.buttons.info.defaultColor.text;
            _textColorHover = _theme.buttons.info.hoverColor.text;
            _textColorPressed = _theme.buttons.info.pressedColor.text;
            _borderColor = _theme.buttons.info.defaultColor.fill;
            _borderColorHover = _theme.buttons.info.hoverColor.border;
            _borderColorPressed = _theme.buttons.info.pressedColor.border;
            _fillColor = _theme.buttons.info.defaultColor.fill;
            _fillColorHover = _theme.buttons.info.hoverColor.fill;
            _fillColorPressed = _theme.buttons.info.pressedColor.fill;
          break;
        case Style::Warning:
            _textColor = _theme.buttons.warning.defaultColor.text;
            _textColorHover = _theme.buttons.warning.hoverColor.text;
            _textColorPressed = _theme.buttons.warning.pressedColor.text;
            _borderColor = _theme.buttons.warning.defaultColor.fill;
            _borderColorHover = _theme.buttons.warning.hoverColor.border;
            _borderColorPressed = _theme.buttons.warning.pressedColor.border;
            _fillColor = _theme.buttons.warning.defaultColor.fill;
            _fillColorHover = _theme.buttons.warning.hoverColor.fill;
            _fillColorPressed = _theme.buttons.warning.pressedColor.fill;
          break;
        case Style::Danger:
            _textColor = _theme.buttons.danger.defaultColor.text;
            _textColorHover = _theme.buttons.danger.hoverColor.text;
            _textColorPressed = _theme.buttons.danger.pressedColor.text;
            _borderColor = _theme.buttons.danger.defaultColor.fill;
            _borderColorHover = _theme.buttons.danger.hoverColor.border;
            _borderColorPressed = _theme.buttons.danger.pressedColor.border;
            _fillColor = _theme.buttons.danger.defaultColor.fill;
            _fillColorHover = _theme.buttons.danger.hoverColor.fill;
            _fillColorPressed = _theme.buttons.danger.pressedColor.fill;
          break;
      }
    } else {
      switch (_style) {
        case Style::Primary:
            _textColor = _theme.buttons.primary.defaultColor.text;
            _textColorHover = _theme.buttons.primary.hoverColor.text;
            _textColorPressed = _theme.buttons.primary.pressedColor.text;
            _borderColor = _theme.buttons.primary.defaultColor.border;
            _borderColorHover = _theme.buttons.primary.hoverColor.border;
            _borderColorPressed = _theme.buttons.primary.pressedColor.border;
            _fillColor = _theme.buttons.primary.defaultColor.fill;
            _fillColorHover = _theme.buttons.primary.hoverColor.fill;
            _fillColorPressed = _theme.buttons.primary.pressedColor.fill;
          break;
        case Style::Secondary:
            _textColor = _theme.buttons.secondary.defaultColor.text;
            _textColorHover = _theme.buttons.secondary.hoverColor.text;
            _textColorPressed = _theme.buttons.secondary.pressedColor.text;
            _borderColor = _theme.buttons.secondary.defaultColor.border;
            _borderColorHover = _theme.buttons.secondary.hoverColor.border;
            _borderColorPressed = _theme.buttons.secondary.pressedColor.border;
            _fillColor = _theme.buttons.secondary.defaultColor.fill;
            _fillColorHover = _theme.buttons.secondary.hoverColor.fill;
            _fillColorPressed = _theme.buttons.secondary.pressedColor.fill;
          break;
        case Style::Info:
            _textColor = _theme.buttons.info.defaultColor.text;
            _textColorHover = _theme.buttons.info.hoverColor.text;
            _textColorPressed = _theme.buttons.info.pressedColor.text;
            _borderColor = _theme.buttons.info.defaultColor.border;
            _borderColorHover = _theme.buttons.info.hoverColor.border;
            _borderColorPressed = _theme.buttons.info.pressedColor.border;
            _fillColor = _theme.buttons.info.defaultColor.fill;
            _fillColorHover = _theme.buttons.info.hoverColor.fill;
            _fillColorPressed = _theme.buttons.info.pressedColor.fill;
          break;
        case Style::Warning:
            _textColor = _theme.buttons.warning.defaultColor.text;
            _textColorHover = _theme.buttons.warning.hoverColor.text;
            _textColorPressed = _theme.buttons.warning.pressedColor.text;
            _borderColor = _theme.buttons.warning.defaultColor.border;
            _borderColorHover = _theme.buttons.warning.hoverColor.border;
            _borderColorPressed = _theme.buttons.warning.pressedColor.border;
            _fillColor = _theme.buttons.warning.defaultColor.fill;
            _fillColorHover = _theme.buttons.warning.hoverColor.fill;
            _fillColorPressed = _theme.buttons.warning.pressedColor.fill;
          break;
        case Style::Danger:
            _textColor = _theme.buttons.danger.defaultColor.text;
            _textColorHover = _theme.buttons.danger.hoverColor.text;
            _textColorPressed = _theme.buttons.danger.pressedColor.text;
            _borderColor = _theme.buttons.danger.defaultColor.border;
            _borderColorHover = _theme.buttons.danger.hoverColor.border;
            _borderColorPressed = _theme.buttons.danger.pressedColor.border;
            _fillColor = _theme.buttons.danger.defaultColor.fill;
            _fillColorHover = _theme.buttons.danger.hoverColor.fill;
            _fillColorPressed = _theme.buttons.danger.pressedColor.fill;
          break;
      }
    }
  }

  void FlatButton::setTheme(std::string name)
  {
    _theme = _getTheme(name);
    setStyle(_style);
  }

  void FlatButton::_render(double deltaTime)
  {
    if (getRenderer() == nullptr) {
      return;
    }

    if (_textTexture == nullptr && !_label.empty()) {
      _updateLabel();
    }

    if (_iconSize == 0 && !_icon.empty()) {
      _updateLabel();
    }

    SDL_Rect ca = getContentArea();

    int iconSpace = _label.empty() ? 0 : 4;

    SDL_FRect iconPos;
    iconPos.x = ca.x + (ca.w - _textWidth - _iconSize - iconSpace) / 2;
    iconPos.y = ca.y + (ca.h - _iconSize) / 2;
    iconPos.h = _iconSize;
    iconPos.w = _iconSize;

    SDL_FRect labelPos;
    labelPos.x = iconPos.x + iconPos.w;
    labelPos.y = ca.y + (ca.h - _textHeight) / 2;
    labelPos.h = _textHeight;
    labelPos.w = _textWidth;


    if (_mouseButtonState[1]) {
      SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
      _drawRoundedRect(getRenderer().get(), _bounds, _borderColorPressed, _fillColorPressed, true, 1, _radius);
      SDL_SetRenderClipRect(getRenderer().get(), &ca);
      SDL_RenderTexture(getRenderer().get(), _textTexturePressed, NULL, &labelPos);
      if (!_icon.empty()) {
        SDL_SetTextureColorMod(_root->getTexture(_icon)->texture.get(), _textColorPressed.r, _textColorPressed.g, _textColorPressed.b);
        _root->renderTexture(_icon, iconPos);
      }
    } else if (isMouseOver()) {
      SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
      _drawRoundedRect(getRenderer().get(), _bounds, _borderColorHover, _fillColorHover, true, 1, _radius);
      SDL_SetRenderClipRect(getRenderer().get(), &ca);
      SDL_RenderTexture(getRenderer().get(), _textTextureHover, NULL, &labelPos);
      if (!_icon.empty()) {
        SDL_SetTextureColorMod(_root->getTexture(_icon)->texture.get(), _textColorHover.r, _textColorHover.g, _textColorHover.b);
        _root->renderTexture(_icon, iconPos);
      }
    } else {
      SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
      _drawRoundedRect(getRenderer().get(), _bounds, _borderColor, _fillColor, !_outline, 1, _radius);
      SDL_SetRenderClipRect(getRenderer().get(), &ca);
      SDL_RenderTexture(getRenderer().get(), _textTexture, NULL, &labelPos);
      if (!_icon.empty()) {
        SDL_SetTextureColorMod(_root->getTexture(_icon)->texture.get(), _textColor.r, _textColor.g, _textColor.b);
        _root->renderTexture(_icon, iconPos);
      }
    }
  }

  void FlatButton::_updateLabel()
  {
    if (getRenderer() == nullptr) {
      return;
    }

    _constraints.width.preferredValue = -1;
    _constraints.height.preferredValue = -1;

    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
    _textTexture = nullptr;

    if (_fontName.empty()|| getRenderer() == nullptr) {
      return;
    }

    if (_label.empty() && _icon.empty()) {
      return;
    }

    int iconWidth = 0;
    _iconSize = 0;
    _textHeight = 0;
    _textWidth = 0;

    if (!_label.empty()) {
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
      _constraints.width.preferredValue = _textWidth + _padding.left + _padding.right;
    } else {
      // only an icon, figure out what _textHeight to use
      int width;
      int height;
      FontBook::measure(_fontName, _fontSize, "Q", &width, &height);
      _textWidth = 0;
      _textHeight = height;
    }

    if (!_icon.empty()) {
      TextureDataPtr data = _root->getTexture(_icon);
      if (data) {
        _iconSize = data->width * (_textHeight/ data->height);
        iconWidth = _iconSize + 4;
      }
    }

    _constraints.width.preferredValue = _textWidth + iconWidth + _padding.left + _padding.right + 2;
    _constraints.height.preferredValue = _textHeight + _padding.top + _padding.bottom + 2;
    _dirty = true;
  }
}
