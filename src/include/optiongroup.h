#ifndef SGI_PANEL_H
#define SGI_PANEL_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flatoption.h"
#include "container.h"
#include "window.h"

namespace SGI {
  class OptionGroup : public Container {
  public:
    static std::shared_ptr<OptionGroup> create();

    using OptionGroupCallback = std::function<bool(WindowPtr, std::vector<FlatOptionPtr>)>;

    ~OptionGroup() { };

    std::string addChangeListener(const OptionGroupCallback& handler);
    void removeChangeListener(const std::string& id);

    /**
     * Add a child FlatOption wiget
     * 
     * Adds a child FlatOption wiget to the current widget. Duplicates
     * of the same widget will not be added
     *
     * \param widget the FlatOption widget to be added.
     */
    void addChild(FlatOptionPtr widget);

    /**
     * Remove a child FlatOption widget
     * 
     * Removes the FlatOption widget as a child if it is a child, if
     * it is not a child nothing happes.
     *
     * \param widget the FlatOption widget to be removed.
     */
    void removeChild(FlatOptionPtr widget);

    void setMultiselect(bool multiselect);
  
  protected:
    OptionGroup() { };

    std::unordered_map<std::string, std::string> _functionIds;

    void addChild(WidgetPtr widget) override;
    void removeChild(WidgetPtr widget) override;

  private:
    bool _multiSelect = false;

    std::unordered_map<std::string, OptionGroupCallback> _changeHandelers;

  };
  using OptionGroupPtr = std::shared_ptr<OptionGroup>;
}

#endif // SGI_PANEL_H
