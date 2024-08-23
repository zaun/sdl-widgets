#ifndef SGI_CONTAINER_H
#define SGI_CONTAINER_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "widget.h"

namespace SGI {
  class Container : public Widget {
  public:
    friend class Panel;
    friend class Window;

    friend bool operator==(const Widget& lhs, const Widget& rhs);

    enum ChildDirection {
      Column,
      Row,
      Stack,
    };

    enum ChildSpacing {
      Between, // items are evenly distributed in the line; first item is on the start line, last item on the end line
      Around, //  items are evenly distributed in the line with equal space around them
    };

    ~Container() { };

    /**
     * Add a child widget
     * 
     * Adds a child wiget to the current widget. Duplicates of the same
     * widget will not be added
     *
     * \param widget the widget to be added.
     */
    void addChild(std::shared_ptr<Widget> widget);

    /**
     * Find all Widgets in the Window with the requested name
     * 
     * This iww search the tree of Widgets in the Window and return
     * all the Widgets with the requeted name.
     * 
     * \param name the name to search for
     * \returns a vector of matching Widgets
     */
    std::vector<std::shared_ptr<Widget>> find(const std::string& name) const;

    /**
     * Finds the 1st Widget in the Window with the requested name
     * 
     * \param name the name to search for
     * \returns the Widgets or nullptr
     */
    std::shared_ptr<Widget> findFirst(const std::string& name) const;

    /**
     * Process a SDL_Event
     */
    bool processEvent(const SDL_Event *event) override;

    /**
     * Remove a child widget
     * 
     * Removes the widget as a child if it is a child, if it is not
     * a child nothing happes.
     *
     * \param widget the widget to be removed.
     */
    void removeChild(std::shared_ptr<Widget> widget);

    void setConstraint(ConstraintType constraint, int minValue, int maxValue) override;

    void setDirection(ChildDirection direction);

    void setPadding(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom) override;

    /**
     * Sets the type spacing around child widgets
     * 
     * \param type the spacing type.
     */
    void setSpacing(ChildSpacing type);

    void setTheme(std::string name);

  protected:
    Container();

    void _render(double deltaTime) override;
    void _renderOverlay(double deltaTime) override;

  private:
    std::vector<std::shared_ptr<Widget>> _children;
    ChildDirection _childDirection = Row;

    ChildSpacing _spacingType = ChildSpacing::Between;
    Constraint _spaceingConstraint;
    int _spacing = 0;

    std::string _themeName = "Dark";

    void _calculateChildrenBounds();
    int _calculateChildrenBoundsRow();
    int _calculateChildrenBoundsColumn();

    void _findRecursive(const std::string& name, std::vector<std::shared_ptr<Widget>>& result) const;

    bool _overflowTop = false;
    bool _overflowBottom = false;
    bool _overflowLeft = false;
    bool _overflowRight = false;

  };
  using ContainerPtr = std::shared_ptr<SGI::Container>;
}

#endif // SGI_CONTAINER_H
