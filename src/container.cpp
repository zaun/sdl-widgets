#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "container.h"
#include "flat.h"
#include "widget.h"

namespace SGI {
  Container::Container() : Widget() { };

  void Container::addChild(WidgetPtr widget)
  {
    if (widget->_root != nullptr) {
      ERROR(WIDGET, "Can't add widget to more than one window.");
      return;
    }

    auto it = std::find(_children.begin(), _children.end(), widget);
    if (it == _children.end()) {
      _children.push_back(widget);
      widget->_renderer = _renderer;
      widget->_root = _root;
      LOG(CONTAINER, "Added child %s to %s", widget->getName().c_str(), getName().c_str());
      _calculateChildrenBounds();

      if (auto flatWidget = std::dynamic_pointer_cast<Flat>(widget)) {
        flatWidget->setTheme(_themeName);
      }
    } else {
      ERROR(WIDGET, "Can't add widget to more than one parent.");
    }
  }

  std::vector<std::shared_ptr<Widget>> Container::find(const std::string& name) const
  {
    std::vector<std::shared_ptr<Widget>> result;
    _findRecursive(name, result);
    return result;
  }

  std::shared_ptr<Widget> Container::findFirst(const std::string& name) const
  {
    std::vector<std::shared_ptr<Widget>> result = find(name);

    if (result.empty()) {
      return nullptr;
    } else {
      return result.front();
    }
  }

  bool Container::processEvent(const SDL_Event *event)
  {
    bool stop = false;
    bool overChild = false;

    for (size_t i = 0; i < _children.size(); ++i) {
      if(_children[i]->processEvent(event)) {
        stop = true;
      }
      if (_children[i]->_mouseOver) {
        overChild = true;
      }
    }

    switch (event->type) {
      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        if (_mouseOver == true) {
          if (!overChild) {
            setFocused();
          }
        }
        break;
      }
    }

    if (stop) {
      return stop;
    }

    if (Widget::processEvent(event)) {
      stop = true;
    }

    return stop;
  }

  void Container::removeChild(WidgetPtr widget)
  {
    auto it = std::find(_children.begin(), _children.end(), widget);
    if (it != _children.end()) {
      _children.erase(it);
      widget->_renderer = nullptr;
      widget->_root = nullptr;
    }
  }

  void Container::setConstraint(ConstraintType constraint, int minValue, int maxValue)
  {
    Widget::setConstraint(constraint, minValue, maxValue);
    _calculateChildrenBounds();
  }

  void Container::setDirection(ChildDirection direction)
  {
    if (_childDirection != direction) {
      _childDirection = direction;
      _calculateChildrenBounds();
    }
  }

  void Container::setPadding(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom)
  {
    Widget::setPadding(left, right, top, bottom);
    _calculateChildrenBounds();
  }

  void Container::setSpacing(ChildSpacing type)
  {
    if (_spacingType != type) {
      _spacingType = type;
      _calculateChildrenBounds();
    }
  }

  void Container::setTheme(std::string name)
  {
    _themeName = name;
    for (const auto& child : _children) {
      if (auto flatWidget = std::dynamic_pointer_cast<Flat>(child)) {
        flatWidget->setTheme(name);
      }
      if (auto containerWidget = std::dynamic_pointer_cast<Container>(child)) {
        containerWidget->setTheme(name);
      }
    }
  }

  void Container::_calculateChildrenBounds()
  {
    _updateContentArea();

    unsigned int numChildren = _children.size();
    if (numChildren == 0) {
      return;
    }

    unsigned int parentWidth = _contentArea.w;
    if (parentWidth < 0) {
      parentWidth = 0;
    }

    unsigned int parentHeight = _contentArea.h;
    if (parentHeight < 0) {
      parentHeight = 0;
    }

    _spacing = _spaceingConstraint.minValue;

    int minSpaceing = _spacing;
    if (_spacingType == ChildSpacing::Around) {
      minSpaceing = (numChildren + 1) * _spacing;
    } else {
      minSpaceing = (numChildren - 1) * _spacing;
    }

    int widthLeft = parentWidth;
    int heightLeft = parentHeight;
    int widthZero = 0;
    int heightZero = 0;
    for (size_t i = 0; i < numChildren; ++i) {
      _children[i]->_bounds.x = 0;
      _children[i]->_bounds.y = 0;
      _children[i]->_bounds.w = _children[i]->_constraints.width.minValue;
      _children[i]->_bounds.h = _children[i]->_constraints.height.minValue;
      widthLeft -= _children[i]->_bounds.w;
      heightLeft -=_children[i]->_bounds.h;
      if (_children[i]->_bounds.w == 0) {
        widthZero += 1;
      }
      if (_children[i]->_bounds.h == 0) {
        heightZero += 1;
      }
    }

    // Reset all children to minimum bounds and margins
    int avgWidth = (widthLeft - minSpaceing) / widthZero;
    int avgHeight = (heightLeft - minSpaceing) / heightZero;
    for (size_t i = 0; i < numChildren; ++i) {
      if (_children[i]->_bounds.w == 0) {
        _children[i]->_bounds.w = avgWidth < _children[i]->_constraints.width.maxValue ? avgWidth : _children[i]->_constraints.width.maxValue;
      }
      if (_children[i]->_bounds.h == 0) {
        _children[i]->_bounds.h = avgHeight < _children[i]->_constraints.height.maxValue ? avgHeight : _children[i]->_constraints.height.maxValue;
      }
    }
    
    if (_childDirection == Row) {
      _calculateChildrenBoundsRow();
    } else {
      _calculateChildrenBoundsColumn();
    }

    // Force children to recalculate thier own children
    for (size_t i = 0; i < numChildren; ++i) {
      if (dynamic_cast<Container*>(_children[i].get())) {
        std::shared_ptr<Container> child = std::static_pointer_cast<Container>(_children[i]);
        child->_calculateChildrenBounds();
      }
    }
  }

  int Container::_calculateChildrenBoundsRow()
  {
    unsigned int parentWidth = _contentArea.w;
    if (parentWidth < 0) {
      parentWidth = 0;
    }
    unsigned int parentHeight = _contentArea.h;
    if (parentHeight < 0) {
      parentHeight = 0;
    }
    unsigned int numChildren = _children.size();

    if (numChildren == 0) {
      return 0;
    }

    _spacing = _spaceingConstraint.minValue;

    // Children are expected to be reset already
    unsigned int contentWidth = 0;
    for (size_t i = 0; i < numChildren; ++i) {
      contentWidth += _children[i]->_bounds.w;
    }
    if (_spacingType == ChildSpacing::Around) {
      contentWidth += (numChildren + 1) * _spacing;
    } else {
      contentWidth += (numChildren - 1) * _spacing;
    }

    int spaceLeft =  parentWidth - contentWidth;
    bool canGrowWidth = true;
    bool canGrowSpacing = true;
    while (spaceLeft > 0 && (canGrowWidth || canGrowSpacing)) {
      if (canGrowWidth) {
        canGrowWidth = false;
        contentWidth = 0;

        int growBy = spaceLeft / numChildren;
        if (growBy == 0 && spaceLeft > 0) {
          growBy = 1;
        }

        for (size_t i = 0; i < numChildren; ++i) {
          bool didGrow = false;
          if (_children[i]->_bounds.w + growBy <= _children[i]->_constraints.width.maxValue &&
              contentWidth + _children[i]->_bounds.w + growBy <= parentWidth) {
            _children[i]->_bounds.w += growBy;
            didGrow = true;
          } else if (_children[i]->_bounds.w + growBy > _children[i]->_constraints.width.maxValue &&
                    _children[i]->_bounds.w < _children[i]->_constraints.width.maxValue &&
                    contentWidth + _children[i]->_constraints.width.maxValue <= parentWidth) {
            _children[i]->_bounds.w = _children[i]->_constraints.width.maxValue;
          }

          contentWidth += _children[i]->_bounds.w;
          if (i != 0 || _spacingType == ChildSpacing::Around) {
            contentWidth += _spacing;
          }

          if (didGrow && _children[i]->_bounds.w < _children[i]->_constraints.width.maxValue && contentWidth < parentWidth) {
            canGrowWidth = true;
          }
        }

        if (_spacingType == ChildSpacing::Around) {
          contentWidth += _spacing;
        }
      } else if (canGrowSpacing) {
        canGrowSpacing = false;
        contentWidth = 0;

        int spaceCount = _spacingType == ChildSpacing::Between ? (numChildren - 1) : (numChildren + 1);
        int growBy = spaceLeft / spaceCount;
        if (growBy == 0 && spaceLeft > 0) {
          growBy = 1;
        }

        if (_spacing + growBy < _spaceingConstraint.maxValue) {
          _spacing += growBy;
        } else if (_spacing < _spaceingConstraint.maxValue) {
          _spacing = _spaceingConstraint.maxValue;
        }

        for (size_t i = 0; i < numChildren; ++i) {
          contentWidth += _children[i]->_bounds.w;
          if (i != 0 || _spacingType == ChildSpacing::Around) {
            contentWidth += _spacing;
          }
        }

        if (_spacingType == ChildSpacing::Around) {
          contentWidth += _spacing;
        }

        if (_spacing < _spaceingConstraint.maxValue && contentWidth < parentWidth && spaceCount > 0) {
          canGrowSpacing = true;
        }
      }

      spaceLeft = parentWidth - contentWidth;
    }

    for (size_t i = 0; i < numChildren; ++i) {
      _children[i]->_bounds.h = parentHeight > _children[i]->_constraints.height.maxValue ? _children[i]->_constraints.height.maxValue : parentHeight;

      if (i == 0) {
        _children[i]->_bounds.x = _contentArea.x;
        if (_spacingType == ChildSpacing::Around) {
          _children[i]->_bounds.x += _spacing;
        }
      } else {
        _children[i]->_bounds.x = _children[i - 1]->_bounds.x + _children[i - 1]->_bounds.w;
        _children[i]->_bounds.x += _spacing;
      }

      _children[i]->_bounds.y = _contentArea.y + (parentHeight - _children[i]->_bounds.h) / 2;
      if (_children[i]->_bounds.y < 0) {
        _children[i]->_bounds.y = 0;
      }

      _children[i]->_updateContentArea();
    }

    return spaceLeft;
  }

  int Container::_calculateChildrenBoundsColumn()
  {
    unsigned int parentWidth = _contentArea.w;
    if (parentWidth < 0) {
      parentWidth = 0;
    }
    unsigned int parentHeight = _contentArea.h;
    if (parentHeight < 0) {
      parentHeight = 0;
    }
    unsigned int numChildren = _children.size();

    if (numChildren == 0) {
      return 0;
    }

    _spacing = _spaceingConstraint.minValue;

    // Children are expected to be reset already
    unsigned int contentHeight = 0;
    for (size_t i = 0; i < numChildren; ++i) {
      contentHeight += _children[i]->_bounds.h;
    }
    if (_spacingType == ChildSpacing::Around) {
      contentHeight += (numChildren + 1) * _spacing;
    } else {
      contentHeight += (numChildren - 1) * _spacing;
    }

    int spaceLeft = parentHeight - contentHeight;
    bool canGrowHeight = true;
    bool canGrowSpacing = true;
    while (spaceLeft > 0 && (canGrowHeight || canGrowSpacing)) {
      if (canGrowHeight) {
        canGrowHeight = false;
        contentHeight = 0;

        int growBy = spaceLeft / numChildren;
        if (growBy == 0 && spaceLeft > 0) {
          growBy = 1;
        }

        for (size_t i = 0; i < numChildren; ++i) {
          bool didGrow = false;
          if (_children[i]->_bounds.h + growBy <= _children[i]->_constraints.height.maxValue &&
              contentHeight + _children[i]->_bounds.h + growBy <= parentHeight) {
            _children[i]->_bounds.h += growBy;
            didGrow = true;
          } else if (_children[i]->_bounds.h + growBy > _children[i]->_constraints.height.maxValue &&
                     _children[i]->_bounds.h < _children[i]->_constraints.height.maxValue &&
                     contentHeight + _children[i]->_bounds.h + _children[i]->_constraints.height.maxValue <= parentHeight) {
            _children[i]->_bounds.h = _children[i]->_constraints.height.maxValue;
          }

          contentHeight += _children[i]->_bounds.h;
          if (i != 0 || _spacingType == ChildSpacing::Around) {
            contentHeight += _spacing;
          }

          if (didGrow && _children[i]->_bounds.h < _children[i]->_constraints.height.maxValue && contentHeight < parentHeight) {
            canGrowHeight = true;
          }
        }

        if (_spacingType == ChildSpacing::Around) {
          contentHeight += _spacing;
        }
      } else if (canGrowSpacing) {
        canGrowSpacing = false;
        contentHeight = 0;

        int spaceCount = _spacingType == ChildSpacing::Between ? (numChildren - 1) : (numChildren + 1);
        int growBy = spaceLeft / spaceCount;
        if (growBy == 0 && spaceLeft > 0) {
          growBy = 1;
        }

        if (_spacing + growBy < _spaceingConstraint.maxValue) {
          _spacing += growBy;
        } else if (_spacing < _spaceingConstraint.maxValue) {
          _spacing = _spaceingConstraint.maxValue;
        }

        for (size_t i = 0; i < numChildren; ++i) {
          contentHeight += _children[i]->_bounds.h;
          if (i != 0 || _spacingType == ChildSpacing::Around) {
            contentHeight += _spacing;
          }
        }

        if (_spacingType == ChildSpacing::Around) {
          contentHeight += _spacing;
        }

        if (_spacing < _spaceingConstraint.maxValue && contentHeight < parentHeight && spaceCount > 0) {
          canGrowSpacing = true;
        }
      }

      spaceLeft = parentHeight - contentHeight;
    }

    for (size_t i = 0; i < numChildren; ++i) {
      _children[i]->_bounds.w = parentWidth > _children[i]->_constraints.width.maxValue ? _children[i]->_constraints.width.maxValue : parentWidth;

      _children[i]->_bounds.y = (i == 0)
        ? _contentArea.y + (_spacingType == ChildSpacing::Around ? _spacing : 0)
        : _children[i - 1]->_bounds.y + _children[i - 1]->_bounds.h + _spacing;

      _children[i]->_bounds.x = _contentArea.x + (parentWidth - _children[i]->_bounds.w) / 2;
      if (_children[i]->_bounds.x < 0) {
        _children[i]->_bounds.x = 0;
      }
      _children[i]->_updateContentArea();
    }

    return spaceLeft;
  }

  void Container::_findRecursive(const std::string& name, std::vector<std::shared_ptr<Widget>>& result) const
  {
    for (const auto& child : _children) {
      if (child) {  
        if (child->_name == name) {
          result.push_back(child);
        }

        auto containerChild = std::dynamic_pointer_cast<Container>(child);
        if (containerChild) {
          containerChild->_findRecursive(name, result);
        }
      }
    }
  }

  void Container::_render(double deltaTime)
  {
    Widget::_render(deltaTime);
    for (size_t i = 0; i < _children.size(); ++i) {
      _children[i]->_render(deltaTime);
      SDL_SetRenderClipRect(_renderer.get(), NULL);
    }
  }

  void Container::_renderOverlay(double deltaTime)
  {
    Widget::_renderOverlay(deltaTime);
    for (size_t i = 0; i < _children.size(); ++i) {
      _children[i]->_renderOverlay(deltaTime);
      SDL_SetRenderClipRect(_renderer.get(), NULL);
    }
  }

}
