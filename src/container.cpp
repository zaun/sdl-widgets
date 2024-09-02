#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "container.h"
#include "flat.h"
#include "widget.h"

namespace SGI {
  std::shared_ptr<Container> Container::create()
  {
    std::shared_ptr<Container> widget = std::make_shared<Container>(Container());
    widget->_self = widget;

    return widget;
  }

  Container::Container() : Widget() { };

  void Container::addChild(WidgetPtr widget)
  {
    if (widget->_root != nullptr) {
      ERROR(WIDGET, "Can't add widget %s to more than one window.", widget->_name.c_str());
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
      if (!stop) {
        if(_children[i]->processEvent(event)) {
          stop = true;
        }
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

  void Container::setConstraint(Widget::ConstraintType constraint, int minValue, int maxValue)
  {
    Widget::setConstraint(constraint, minValue, maxValue);
    _calculateChildrenBounds();
  }

  void Container::setConstraint(Container::ConstraintType constraint, int minValue, int maxValue)
  {
    switch(constraint) {
      case Container::ConstraintType::Height:
        Widget::setConstraint(Widget::ConstraintType::Width, minValue, maxValue); 
        break;
      case Container::ConstraintType::Width:
        Widget::setConstraint(Widget::ConstraintType::Height, minValue, maxValue); 
        break;
      case Container::ConstraintType::Spacing:
        _spaceingConstraint.minValue = minValue;
        _spaceingConstraint.maxValue = maxValue;
        break;
    }
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

  void Container::setTheme(const std::string& name)
  {
    if (_name == "WindowSidebarContainer") {
      LOG(SIDEBAR, "%s", name.c_str());
    }
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
      int width = _children[i]->_constraints.width.minValue;
      int height = _children[i]->_constraints.height.minValue;

      if (_children[i]->_constraints.width.preferredValue > _children[i]->_constraints.width.minValue &&
          _children[i]->_constraints.width.preferredValue < _children[i]->_constraints.width.maxValue) {
        width = _children[i]->_constraints.width.preferredValue;
      }
      if (_children[i]->_constraints.height.preferredValue > _children[i]->_constraints.height.minValue &&
          _children[i]->_constraints.height.preferredValue < _children[i]->_constraints.height.maxValue) {
        height = _children[i]->_constraints.height.preferredValue;
      }

      SDL_Rect childRect;
      childRect.x = 0;
      childRect.y = 0;
      childRect.w = width;
      childRect.h = height;
      _children[i]->_setBounds(childRect);
      widthLeft -= childRect.w;
      heightLeft -= childRect.h;
      if (childRect.w == 0) {
        widthZero += 1;
      }
      if (childRect.h == 0) {
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

    std::vector<SDL_Rect> childRects;
    for (size_t i = 0; i < numChildren; ++i) {
      SDL_Rect childRect;
      childRect.x = _children[i]->_bounds.x;
      childRect.y = _children[i]->_bounds.y;
      childRect.w = _children[i]->_bounds.w;
      childRect.h = _children[i]->_bounds.h;
      childRects.push_back(childRect);
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
          if (childRects[i].w + growBy <= _children[i]->_constraints.width.maxValue &&
              contentWidth + childRects[i].w + growBy <= parentWidth) {
            if (_children[i]->_constraints.width.preferredValue == -1 ||
                childRects[i].w + growBy < _children[i]->_constraints.width.preferredValue) {
              childRects[i].w += growBy;
              didGrow = true;
            }
          } else if (childRects[i].w + growBy > _children[i]->_constraints.width.maxValue &&
                    childRects[i].w < _children[i]->_constraints.width.maxValue &&
                    contentWidth + _children[i]->_constraints.width.maxValue <= parentWidth) {
            childRects[i].w = _children[i]->_constraints.width.maxValue;
          }

          contentWidth += childRects[i].w;
          if (i != 0 || _spacingType == ChildSpacing::Around) {
            contentWidth += _spacing;
          }

          if (didGrow && childRects[i].w < _children[i]->_constraints.width.maxValue && contentWidth < parentWidth) {
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
          contentWidth += childRects[i].w;
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
      childRects[i].h = parentHeight > _children[i]->_constraints.height.maxValue ? _children[i]->_constraints.height.maxValue : parentHeight;

      if (i == 0) {
        childRects[i].x = _contentArea.x;
        if (_spacingType == ChildSpacing::Around) {
          childRects[i].x += _spacing;
        }
      } else {
        childRects[i].x = childRects[i - 1].x + childRects[i - 1].w;
        childRects[i].x += _spacing;
      }

      childRects[i].y = _contentArea.y + (parentHeight - childRects[i].h) / 2;
      _children[i]->_setBounds(childRects[i]);
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

    std::vector<SDL_Rect> childRects;
    for (size_t i = 0; i < numChildren; ++i) {
      SDL_Rect childRect;
      childRect.x = _children[i]->_bounds.x;
      childRect.y = _children[i]->_bounds.y;
      childRect.w = _children[i]->_bounds.w;
      childRect.h = _children[i]->_bounds.h;
      childRects.push_back(childRect);
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
          if (childRects[i].h + growBy <= _children[i]->_constraints.height.maxValue &&
              contentHeight + childRects[i].h + growBy <= parentHeight) {
            if (_children[i]->_constraints.height.preferredValue == -1 ||
                childRects[i].h + growBy < _children[i]->_constraints.height.preferredValue) {
              childRects[i].h += growBy;
              didGrow = true;
            }
          } else if (childRects[i].h + growBy > _children[i]->_constraints.height.maxValue &&
                     childRects[i].h < _children[i]->_constraints.height.maxValue &&
                     contentHeight + childRects[i].h + _children[i]->_constraints.height.maxValue <= parentHeight) {
            childRects[i].h = _children[i]->_constraints.height.maxValue;
          }

          contentHeight += childRects[i].h;
          if (i != 0 || _spacingType == ChildSpacing::Around) {
            contentHeight += _spacing;
          }

          if (didGrow && childRects[i].h < _children[i]->_constraints.height.maxValue && contentHeight < parentHeight) {
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
          contentHeight += childRects[i].h;
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
      childRects[i].w = parentWidth > _children[i]->_constraints.width.maxValue ? _children[i]->_constraints.width.maxValue : parentWidth;

      childRects[i].y = (i == 0)
        ? _contentArea.y + (_spacingType == ChildSpacing::Around ? _spacing : 0)
        : childRects[i - 1].y + childRects[i - 1].h + _spacing;

      childRects[i].x = _contentArea.x + (parentWidth - childRects[i].w) / 2;
      _children[i]->_setBounds(childRects[i]);
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
    bool dirty = false;
    for (size_t i = 0; i < _children.size(); ++i) {
      _children[i]->_render(deltaTime);
      if (_children[i]->_dirty) {
        _children[i]->_dirty = false;
        dirty = true;
      }
      SDL_SetRenderClipRect(_renderer.get(), NULL);
    }

    if (dirty) {
      _calculateChildrenBounds();
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

  void Container::_setBounds(SDL_Rect& bounds)
  {
    Widget::_setBounds(bounds);
    _calculateChildrenBounds();
  }

}
