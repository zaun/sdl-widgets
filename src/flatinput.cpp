#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "flatinput.h"
#include "fontbook.h"
#include "window.h"

namespace SGI {
  std::shared_ptr<FlatInput> FlatInput::create()
  {
    std::shared_ptr<FlatInput> widget = std::make_shared<FlatInput>(FlatInput());
    widget.get()->_self = widget;
    widget.get()->setConstraintFixed(SGI::Widget::ConstraintType::Height, 36);
    widget.get()->setPadding(12, 12, 6, 6);

    return widget;
  }

  FlatInput::~FlatInput()
  {
    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
  }

  std::string FlatInput::addChangeListener(const Widget::Callback& handler)
  {
    std::string id;
    
    do {
      id = _generateShortCode();
    } while (_inputHandelers.find(id) != _inputHandelers.end());
    _inputHandelers[id] = handler;

    return id;
  }

  void FlatInput::removeChangeListener(const std::string& id)
  {
    _inputHandelers.erase(id);
  }

  std::string FlatInput::getFontName()
  {
    return _fontName;
  }

  double FlatInput::getFontSize()
  {
    return _fontSize;
  }

  std::string FlatInput::getValue()
  {
    return _value;
  }

  bool FlatInput::processEvent(const SDL_Event *event)
  {
    bool ret = Widget::processEvent(event);
    switch (event->type) {
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        _selectStart = -1;
        _selectEnd = -1;
        if (isMouseOver() && !_focused) {
          LOG(FLATINPUT, "SDL_StartTextInput called");
          setFocused();
          _cursorBlink = true;
          _cursorTime = 0;
          _cursorIndex = _getCursorIndexFromMouse();
          _selectStart = _cursorIndex;
          _updatePosition();
          SDL_StartTextInput(_root.get()->getWindow().get());
        } else if (isMouseOver() && _focused) {
          _cursorIndex = _getCursorIndexFromMouse();
          _selectStart = _cursorIndex;
          _updatePosition();
        } else if (!isMouseOver() && _focused) {
          setFocused(false);
          SDL_StopTextInput(_root.get()->getWindow().get());
        }
        break;
      }
      case SDL_EVENT_MOUSE_BUTTON_UP: {
        if (isMouseOver() && _focused) {
          _selectEnd = _getCursorIndexFromMouse();
          _cursorIndex = _selectEnd;

          if (_selectStart == _selectEnd) {
            _selectStart = -1;
            _selectEnd = -1;
          }
          if (_selectStart > _selectEnd) {
            int t  = _selectEnd;
            _selectEnd = _selectStart;
            _selectStart = t;
          }
          _updatePosition();
        } else {
          if (_selectStart != -1 || _selectEnd != -1) {
            _selectStart = -1;
            _selectEnd = -1;
            _updatePosition();
          }
        }
        break;
      }
      case SDL_EVENT_MOUSE_MOTION: {
        if (_focused && _mouseButtonState[1] == true) {
          _selectEnd = _getCursorIndexFromMouse();
          _cursorIndex = _selectEnd;

          _updatePosition();
        }
        break;
      }
      case SDL_EVENT_KEY_DOWN: {
        if (!_focused) {
          break;
        }
        if (event->key.key == SDLK_DELETE || event->key.key == SDLK_BACKSPACE) {
          if (_selectStart != -1 && _selectEnd != -1) {
            _removeSelection();
            _updateLabel();
            break;
          }

          if (_cursorIndex > 0) {
            _cursorIndex -= 1;
            _textureOffset = 0;
            _value.erase(_cursorIndex, 1);
            _updateLabel();

            for (const auto& [id, handler] : _inputHandelers) {
              if (handler(_root, _self)) {
                ret = true;
              }
            }
          }
        }
        if (event->key.key == SDLK_LEFT) {
          _selectStart = -1;
          _selectEnd = -1;
          if (_cursorIndex > 0) {
            _cursorIndex -= 1;
            _updatePosition();
            _cursorBlink = true;
            _cursorTime = 0;
          }
        }
        if (event->key.key == SDLK_RIGHT) {
          _selectStart = -1;
          _selectEnd = -1;
          if (_cursorIndex < _value.length()) {
            _cursorIndex += 1;
            _updatePosition();
            _cursorBlink = true;
            _cursorTime = 0;
          }
        }
        break;
      }

      case SDL_EVENT_TEXT_INPUT: {
        if (!_focused) {
          break;
        }

        _removeSelection();

        _value.insert(_cursorIndex, event->text.text);
        _cursorIndex += 1;
        _updateLabel();

        for (const auto& [id, handler] : _inputHandelers) {
          if (handler(_root, _self)) {
            ret = true;
          }
        }
        break;
      }

      default:
        break;
    };

    return ret;
  }

  void FlatInput::setBorderColor(const SDL_Color &color) {
    _borderColor = color;
  }

  void FlatInput::setFillColor(const SDL_Color &color) {
    _fillColor = color;
  }

  void FlatInput::setSelectColor(const SDL_Color &color)
  {
    _selectColor = color;
  }

  void FlatInput::setFontName(const std::string& fontName)
  {
    _fontName = fontName;
    _updateLabel();
  }

  void FlatInput::setFontSize(double fontSize)
  {
    if (fontSize > 0) {
      _fontSize = fontSize;
    } else {
      _fontSize = 1;
    }
    _updateLabel();
  }

  void FlatInput::setRadius(int value)
  {
    if (value < 0) {
      return;
    }

    _radius = value;
  }

  void FlatInput::setValue(const std::string& value)
  {
    if (_value != value) {
      _value = value;
      _updateLabel();
    }
  }

  void FlatInput::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _textColor = theme.primaryText;
    _borderColor = theme.primaryBorder;
    _fillColor = theme.primaryFill;
    _selectColor = theme.textSelection;
    _updateLabel();
  }
  
  void FlatInput::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }

    if (!_textTexture) {
      _updateLabel();
    }

    if (!_focused) {
      _cursorIndex = 0;
      _cursorPosition = 0;
      _textureOffset = 0;
    }

    SDL_SetRenderClipRect(getRenderer().get(), &_bounds);
    _drawRoundedRect(getRenderer().get(), _bounds, _borderColor, _fillColor, true, 1, _radius);

    if (_selectStart != -1 && _selectEnd != -1 && _selectStart != _selectEnd) {
      SDL_SetRenderDrawColor(getRenderer().get(), _selectColor.r, _selectColor.g, _selectColor.b, _selectColor.a);
      SDL_RenderFillRect(getRenderer().get(), &_selectRect);
    }

    SDL_Rect ca = getContentArea();
    SDL_FRect position;
    SDL_FRect cursorRect;

    if (_cursorPosition > ca.w - _textureOffset) {
      _textureOffset -= _cursorPosition - (ca.w - _textureOffset);
    } else if (_cursorPosition + _textureOffset < 0) {
      _textureOffset -= (_cursorPosition + _textureOffset);
    }

    position.x = ca.x + _textureOffset;
    position.y = ca.y + (ca.h - _textHeight) / 2;
    position.h = _textHeight;
    position.w = _textWidth;

    cursorRect.x = position.x + _cursorPosition;
    cursorRect.y = ca.y;
    cursorRect.w = 1;
    cursorRect.h = ca.h;

    if (_focused && _cursorBlink) {
      SDL_SetRenderDrawColor(getRenderer().get(), _textColor.r, _textColor.g, _textColor.b, _textColor.a);
      SDL_RenderRect(getRenderer().get(), &cursorRect);
    }

    _cursorTime += deltaTime;
    if (_cursorTime > 530) {
      _cursorBlink = !_cursorBlink;
      _cursorTime = 0;
    }
    
    SDL_SetRenderClipRect(getRenderer().get(), &ca);
    SDL_RenderTexture(getRenderer().get(), _textTexture, NULL, &position);
    SDL_SetRenderClipRect(getRenderer().get(), nullptr);

    Widget::_render(deltaTime);
  }

  int FlatInput::_getCursorIndexFromMouse()
  {
    SDL_FPoint mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    SDL_Rect ca = getContentArea();
    int localX = mousePos.x - ca.x - _textureOffset;

    if (localX > _textWidth) {
      return _value.length();
    } else if (localX < 0) {
      return 0;
    }
    
    int position = 0;
    int index = 0;
    while (index < _value.length()) {
      int charWidth;
      int charHeight;
      std::string currentChar = _value.substr(index, 1);

      if (!FontBook::measure(_fontName, _fontSize, currentChar, &charWidth, &charHeight)) {
        LOG(FLATINPUT, "Error getting char info: %s", SDL_GetError());
        break;
      }

      index++;
      position += charWidth;
      if (position > localX) {
        break;
      }
    }
    return index;
  }

  void FlatInput::_removeSelection()
  {
    if (_selectStart != -1 && _selectEnd != -1) {
      if (_selectStart > _selectEnd) {
        _value.erase(_selectEnd, _selectStart - _selectEnd);
        _cursorIndex = _selectEnd;
      } else {
        _value.erase(_selectStart, _selectEnd - _selectStart);
        _cursorIndex = _selectStart;
      }
      _selectStart = -1;
      _selectEnd = -1;
      _updatePosition();
    }
  }

  void FlatInput::_updateLabel()
  {
    if (getRenderer() == nullptr) {
      return;
    }

    if (_textTexture != nullptr) {
      SDL_DestroyTexture(_textTexture);
    }
    _textTexture = nullptr;

    if (_fontName.empty() || _value.empty() || getRenderer() == nullptr) {
      return;
    }

    std::shared_ptr<SDL_Surface> textSurface = FontBook::render(_fontName, _fontSize, _value, _textColor);
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

    _updatePosition();
  }

  void FlatInput::_updatePosition()
  {
    std::string check = _value.substr(0, _cursorIndex);
    if (check.length() > 0) {
      int count;
      if (!FontBook::measure(_fontName, _fontSize, check, &_cursorPosition, &count)) {
        _cursorPosition = 0;
      }
    } else {
      _cursorPosition = 0;
    }

    // While actively dragging these
    // can be reversed.
    int s = _selectStart;
    int e = _selectEnd;
    if (_selectStart > _selectEnd) {
      e = _selectStart;
      s = _selectEnd;
    }

    SDL_Rect ca = getContentArea();
    _selectRect.y = ca.y;
    _selectRect.h = ca.h;
    if (s != -1 && e != -1 && s != e) {
      std::string check = _value.substr(0, s);
      _selectRect.x = ca.x - _textureOffset;
      if (check.length() > 0) {
        int count;
        int width;
        if (FontBook::measure(_fontName, _fontSize, check, &width, &count)) {
          _selectRect.x = ca.x - _textureOffset + width;
        }
      }

      check = _value.substr(0, e);
      _selectRect.w = ca.x - _textureOffset;
      if (check.length() > 0) {
        int count;
        int width;
        if (FontBook::measure(_fontName, _fontSize, check, &width, &count)) {
          _selectRect.w = ca.x - _textureOffset + width - _selectRect.x;
        }
      }
    }
  }
}