#ifndef SGI_CONTAINER_H
#define SGI_CONTAINER_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "widget.h"

namespace SGI {
  class Container : public Widget {
  public:
    friend class OptionGroup;
    friend class Panel;
    friend class Window;

    friend bool operator==(const Widget& lhs, const Widget& rhs);

    enum Direction {
      Column,
      Row,
      Stack,
    };

    enum SpaceContent {
      Around,   // items are evenly distributed in the line with equal space around them
      Between,  // items are evenly distributed in the line; first item is on the start line, last item on the end line
    };

    enum JustifyContent {
      Center,   // items are centered 
      End,      // items are packed toward the end
      Start,    // items are packed toward the start
    };

    enum ConstraintType {
      Width,
      Height,
      Spacing,
    };

    static std::shared_ptr<Container> create();

    ~Container() { };

    /**
     * Add a child widget
     * 
     * Adds a child wiget to the current widget. Duplicates of the same
     * widget will not be added
     *
     * \param widget the widget to be added.
     */
    virtual void addChild(WidgetPtr widget);

    /**
     * Find all Widgets in the Window with the requested name
     * 
     * This iww search the tree of Widgets in the Window and return
     * all the Widgets with the requeted name.
     * 
     * \param name the name to search for
     * \returns a vector of matching Widgets
     */
    std::vector<WidgetPtr> find(const std::string& name) const;

    /**
     * Finds the 1st Widget in the Window with the requested name
     * 
     * \param name the name to search for
     * \returns the Widgets or nullptr
     */
    WidgetPtr findFirst(const std::string& name) const;

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
    virtual void removeChild(WidgetPtr widget);

    void setConstraint(Widget::ConstraintType constraint, int minValue, int maxValue) override;

    void setConstraint(Container::ConstraintType constraint, int minValue, int maxValue);

    void setConstraintFixed(Widget::ConstraintType constraint, int value) override;

    void setConstraintFixed(Container::ConstraintType constraint, int value);

    void setConstraintMinimum(Widget::ConstraintType constraint, int value) override;

    void setConstraintMinimum(Container::ConstraintType constraint, int value);

    void setConstraintMaximum(Widget::ConstraintType constraint, int value) override;

    void setConstraintMaximum(Container::ConstraintType constraint, int value);

    void setConstraintAuto(Widget::ConstraintType constraint) override;

    void setConstraintAuto(Container::ConstraintType constraint);

    void setDirection(Direction direction);

    void setPadding(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom) override;

    /**
     * Sets the type of spacing around child widgets
     * 
     * \param type the spacing type.
     */
    void setSpaceContent(SpaceContent type);

    /**
     * Sets the justification child widgets
     * 
     * \param type the justify type.
     */
    void setJustifyContent(JustifyContent type);

    virtual void setTheme(const std::string& name);

  protected:
    Container();

    void _render(double deltaTime) override;
    void _renderOverlay(double deltaTime) override;
    void _setBounds(SDL_Rect& bounds) override;
    void _setRoot(std::shared_ptr<SGI::Window> root) override;
    void _setRenderer(std::shared_ptr<SDL_Renderer> renderer) override;

  private:
    std::vector<WidgetPtr> _children;
    Direction _Direction = Row;

    SpaceContent _spaceContentType = SpaceContent::Between;
    JustifyContent _justifyContentType = JustifyContent::Start;
    Constraint _spaceingConstraint;
    int _spacing = 0;

    std::string _themeName = "Dark";

    void _calculateChildrenBounds();
    int _calculateChildrenBoundsRow();
    int _calculateChildrenBoundsColumn();
    int _calculateChildrenBoundsStack();

    void _findRecursive(const std::string& name, std::vector<WidgetPtr>& result) const;

    bool _overflowTop = false;
    bool _overflowBottom = false;
    bool _overflowLeft = false;
    bool _overflowRight = false;

  };
  using ContainerPtr = std::shared_ptr<SGI::Container>;
}

#endif // SGI_CONTAINER_H
