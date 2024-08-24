#include <iomanip>
#include <memory>
#include <memory>
#include <random>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <sstream>
#include <string>
#include "debug.h"
#include "window.h"

namespace SGI {
  Widget::Widget() : _renderer(nullptr),  _root(nullptr), _self(nullptr), _focused(false), _mouseOver(false), _name("")
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

    uint32_t part1 = dis(gen);
    uint32_t part2 = dis(gen);
    uint32_t part3 = dis(gen);
    uint32_t part4 = dis(gen);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << part1 << "-";
    ss << std::setw(4) << ((part2 >> 16) & 0xFFFF) << "-";
    ss << std::setw(4) << (((part2 >> 12) & 0x0FFF) | 0x4000) << "-"; // 4xxx
    ss << std::setw(4) << (((part3 >> 16) & 0x3FFF) | 0x8000) << "-"; // 8xxx
    ss << std::setw(4) << (part3 & 0xFFFF);
    ss << std::setw(8) << part4;

    _uuid = ss.str();
  }

  // Overload the equality operator
  bool operator==(const Widget& lhs, const Widget& rhs)
  {
    return lhs._uuid == rhs._uuid;    
  }

  std::string Widget::addMouseListener(const MouseEventType type, const MouseButtonCallback& handler)
  {
    std::string id;
    
    do {
      id = _generateShortCode();
    } while (_mouseHandlers[type].find(id) != _mouseHandlers[type].end());
    _mouseHandlers[type][id] = handler;

    return id;
  }

  SDL_Rect Widget::getBounds()
  {
    return _bounds;
  }

  SDL_Rect Widget::getContentArea()
  {
    return _contentArea;
  }

  std::string Widget::getName()
  {
    return _name;
  }

  std::shared_ptr<SDL_Renderer> Widget::getRenderer()
  {
    return _renderer;
  }

  bool Widget::isFocused()
  {
    return _focused;
  }

  bool Widget::isMouseOver()
  {
    return _mouseOver;
  }

  bool Widget::processEvent(const SDL_Event *event)
  {
    bool stop = false;

    switch (event->type) {
      case SDL_EVENT_MOUSE_MOTION: {
        char i = event->button.button;
        _checkMouseInBounds();
        break;
      }
      
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        _checkMouseInBounds();
        if (_mouseOver == true) {
          _mouseButtonState[event->button.button] = true;
          for (const auto& [id, handler] : _mouseHandlers[MouseEventType::Down]) {
            if (handler(_root, _self, event->button.button)) {
              stop = true;
            }
          }
        }
        break;
      }
      
      case SDL_EVENT_MOUSE_BUTTON_UP: {
        _checkMouseInBounds();
        if (_mouseOver == true) {
          for (const auto& [id, handler] : _mouseHandlers[MouseEventType::Up]) {
            if (handler(_root, _self, event->button.button)) {
              stop = true;
            }
          }
          if (_mouseButtonState[event->button.button] == true) {
            for (const auto& [id, handler] : _mouseHandlers[MouseEventType::Click]) {
              if (handler(_root, _self, event->button.button)) {
                stop = true;
              }
            }
          }
        }
        _mouseButtonState[event->button.button] = false;
        break;
      }
    }

    return stop;
  }

  void Widget::removeMouseListener(const MouseEventType type, const std::string& id)
  {
    auto it = _mouseHandlers[type].find(id);
    if (it != _mouseHandlers[type].end()) {
        _mouseHandlers[type].erase(it);
    }
  }

  void Widget::setConstraint(ConstraintType constraint, int minValue, int maxValue)
  {
    switch(constraint) {
      case Width:
        _constraints.width.minValue = minValue;
        _constraints.width.maxValue = maxValue;
        break;
      case Height:
        _constraints.height.minValue = minValue;
        _constraints.height.maxValue = maxValue;
        break;
    }
  }

  void Widget::setConstraintFixed(ConstraintType constraint, int value)
  {
    setConstraint(constraint, value, value);
  }

  void Widget::setConstraintMinimum(ConstraintType constraint, int value)
  {
    setConstraint(constraint, value, std::numeric_limits<unsigned int>::max());
  }

  void Widget::setConstraintMaximum(ConstraintType constraint, int value)
  {
    setConstraint(constraint, 0, value);
  }

  void Widget::setConstraintAuto(ConstraintType constraint)
  {
    setConstraint(constraint, 0, std::numeric_limits<unsigned int>::max());
  }

  void Widget::setFocused(bool value)
  {
    if (_root && _self && value) {
      _root->setFocused(_self);
    } else {
      _root->setFocused(nullptr);
    }
  }

  void Widget::setName(std::string name)
  {
    _name = name;
  }

  void Widget::setPadding(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom)
  {
    _padding.left = left;
    _padding.right = right;
    _padding.top = top;
    _padding.bottom = bottom;
    _updateContentArea();
  }

  void Widget::_render(double deltaTime)
  {
    // if (_mouseOver) {
    //   SDL_SetRenderDrawColor(getRenderer().get(), 0, 150, 255, 255);
    // } else {
    //   SDL_SetRenderDrawColor(getRenderer().get(), 0, 0, 255, 255);
    // }
    // SDL_FRect rect;
    // rect.x = _bounds.x;
    // rect.y = _bounds.y;
    // rect.w = _bounds.w;
    // rect.h = _bounds.h;
    // SDL_RenderRect(getRenderer().get(), &rect);
  };

  void Widget::_checkMouseInBounds()
  {
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseX > _bounds.x && mouseX < _bounds.x + _bounds.w &&
        mouseY > _bounds.y && mouseY < _bounds.y + _bounds.h) {
      _mouseOver = true;
      for (const auto& [id, handler] : _mouseHandlers[MouseEventType::Enter]) {
        handler(_root, _self, 0);
      }
    } else {
      if (_mouseOver == true) {
        for (const auto& [id, handler] : _mouseHandlers[MouseEventType::Exit]) {
          handler(_root, _self, 0);
        }
      }
      _mouseOver = false;
    }
  }

  std::string Widget::_generateShortCode()
  {
    const char characters[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t charactersCount = sizeof(characters) - 1;
    const size_t codeLength = 6;

    std::string shortCode;
    shortCode.reserve(codeLength);

    std::random_device rd;
    std::uniform_int_distribution<> distribution(0, charactersCount - 1);

    for (size_t i = 0; i < codeLength; ++i) {
        shortCode += characters[distribution(rd)];
    }

    return shortCode;
  }

  void Widget::_updateContentArea()
  {
    _contentArea.x = _bounds.x + _padding.left;
    _contentArea.y = _bounds.y + _padding.top;
    _contentArea.w = _bounds.w - _padding.left - _padding.right;
    _contentArea.h = _bounds.h - _padding.top - _padding.bottom;
  }

}
