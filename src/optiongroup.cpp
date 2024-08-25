#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "container.h"
#include "debug.h"
#include "optiongroup.h"
#include "widget.h"
#include "window.h"

namespace SGI {
  std::shared_ptr<OptionGroup> OptionGroup::create()
  {
    std::shared_ptr<OptionGroup> widget = std::make_shared<OptionGroup>(OptionGroup());
    widget->_self = widget;
    widget->setPadding(10, 10, 10, 10);

    return widget;
  }

  std::string OptionGroup::addChangeListener(const OptionGroupCallback& handler)
  {
    std::string id;
    
    do {
      id = _generateShortCode();
    } while (_changeHandelers.find(id) != _changeHandelers.end());
    _changeHandelers[id] = handler;

    return id;
  }

  void OptionGroup::removeChangeListener(const std::string& id)
  {
    _changeHandelers.erase(id);
  }

  void OptionGroup::addChild(FlatOptionPtr option)
  {
    _functionIds[option->getUUID()] = option->addChangeListener([this](WindowPtr window, WidgetPtr widget)->bool {
      if (!_multiSelect) {
        for (const auto& child : _children) {
          if (child->getUUID() == widget->getUUID()) {
            continue;
          }

          if (auto optionWidget = std::dynamic_pointer_cast<FlatOption>(child)) {
            optionWidget->setValue(false);
          }
        }
      }

      std::vector<FlatOptionPtr> selected;
      for (const auto& child : _children) {
        if (auto optionWidget = std::dynamic_pointer_cast<FlatOption>(child)) {
          if (optionWidget->getValue()) {
            selected.push_back(optionWidget);
          }
        }
      }

      for (const auto& [id, handler] : _changeHandelers) {
        handler(_root, selected);
      }
      return false;
    });
    Container::addChild(option);
  }

  void OptionGroup::removeChild(FlatOptionPtr option)
  {
    auto it = std::find(_children.begin(), _children.end(), option);
    if (it != _children.end()) {
      option->removeChangeListener(_functionIds[option->getUUID()]);
    } 

    Container::removeChild(option);
  }

  void OptionGroup::setMultiselect(bool multiselect)
  {
    _multiSelect = multiselect;
  }


  void OptionGroup::addChild(WidgetPtr widget)
  {
    ERROR(OPTIONGROUP, "This should not be called with non FlatOption widgets.");
    throw;
  }

  void OptionGroup::removeChild(WidgetPtr widget)
  {

  }

}