#ifndef SGI_WIDGET_H
#define SGI_WIDGET_H

#include <functional>
#include <SDL3/SDL.h>
#include <string>
#include <vector>

namespace SGI {
  class Container;
  class Window;

  class Widget {
  public:
    friend class Container;
    friend class Window;

    friend bool operator==(const Widget& lhs, const Widget& rhs);

    struct Constraint {
      unsigned int minValue = 0;
      unsigned int maxValue = std::numeric_limits<unsigned int>::max();
    };

    enum ConstraintType {
      Width,
      Height,
    };

    enum MouseEventType {
      Click,
      Down,
      Enter,
      Exit,
      Up,
    };

    using MouseButtonCallback = std::function<bool(std::shared_ptr<Window>, std::shared_ptr<Widget>, uint8_t)>;
    using Callback = std::function<bool(std::shared_ptr<Window>, std::shared_ptr<Widget>)>;

    ~Widget() {};

    /**
     * Add a handler to be called on mouse button down events
     * 
     * This event is triggered when the mouse button is pressed while
     * within the bounds of the widget.
     *
     * \param handler the handler to call when the event is triggered.
     * \returns an id that can be used to remove the handler.
     */
    std::string addMouseListener(const MouseEventType type, const MouseButtonCallback& handler);

    SDL_Rect getBounds();

    SDL_Rect getContentArea();

    std::string getName();

    std::string getUUID();

    /**
     * Gets the shared pointer the the Window's renderer
     * 
     * \returns the shared pointer to the Windows's renderer
     */
    std::shared_ptr<SDL_Renderer> getRenderer();

    bool isFocused();

    bool isMouseOver();

    /**
     * Process a SDL_Event
     */
    virtual bool processEvent(const SDL_Event *event);

    /**
     * Remve a handler from the list of mouse button down handlers
     *
     * \param id the id of the handler that is to be removed.
     */
    void removeMouseListener(const MouseEventType type, const std::string& id);

    virtual void setConstraint(ConstraintType constraint, int minValue, int maxValue);

    void setConstraintFixed(ConstraintType constraint, int value);

    void setConstraintMinimum(ConstraintType constraint, int value);

    void setConstraintMaximum(ConstraintType constraint, int value);

    void setConstraintAuto(ConstraintType constraint);
    
    void setFocused(bool value = true);

    void setName(std::string name);

    virtual void setPadding(unsigned int left, unsigned int right, unsigned int top, unsigned int bottom);
  
  protected:
    Widget();

    /**
     * The shared pointer to the widget
     * 
     * The create function sets this to the shared pointer
     * it returns.
     */
    std::shared_ptr<SGI::Window> _root;

    /**
     * The root window this widget is a child of
     * 
     * This is assigned by the parent when adding the widget
     * as a child and is set to nullptr by the parent when
     * removing the widget as a child.
     */
    std::shared_ptr<SGI::Widget> _self;

    /**
     * The bounds of the widget
     * 
     * All renders must take place within the bounds
     * of the widget. Anything rendered outside of the
     * bounds may be over written in unexpected ways.
     */
    SDL_Rect _bounds;

    struct {
      unsigned int top = 0;
      unsigned int bottom = 0;
      unsigned int left = 0;
      unsigned int right = 0;
    } _padding;

    bool _focused;

    std::unordered_map<Uint8, bool> _mouseButtonState;

    std::string _generateShortCode();
    virtual void _render(double deltaTime);
    virtual void _renderOverlay(double deltaTime) { };
    
    virtual void _setBounds(SDL_Rect& bounds);

  private:
    void _checkMouseInBounds();

    struct {
      Constraint width;
      Constraint height;
    } _constraints;

    /**
     * The contentArea of the widget
     * 
     * The area is the inside the bounds of the Widget 
     * excluding the padding.
     */
    SDL_Rect _contentArea;

    std::unordered_map<MouseEventType, std::unordered_map<std::string, MouseButtonCallback>> _mouseHandlers;

    bool _mouseOver;

    /**
     * A user defined name for the widet
     * 
     * This is a use defined name and allows for a user to find a
     * specific widget or widgets within a window.
     */
    std::string _name;

    /**
     * The renderer to use
     * 
     * This is assigned by the parent when adding the widget
     * as a child and is set to nullptr by the parent when
     * removing the widget as a child.
     */
    std::shared_ptr<SDL_Renderer> _renderer = nullptr;

    void _updateContentArea();

    /**
     * The UUIDv4 of the widget, it is used when compairing widgets
     */
    std::string _uuid;

  };
  using WidgetPtr = std::shared_ptr<SGI::Widget>;
}

#endif // SGI_WIDGET_H
