#ifndef SGI_WINDOW_H
#define SGI_WINDOW_H

#include <map>
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "container.h"

namespace SGI {
  class Window : public Container {
  public:
    enum CursorType {
      DEFAULT,
      TEXT,
      WAIT,
      CROSSHAIR,
      PROGRESS,
      NWSE_RESIZE,
      NESW_RESIZE,
      EW_RESIZE,
      NS_RESIZE,
      MOVE,
      NOT_ALLOWED,
      POINTER,
      NW_RESIZE,
      N_RESIZE,
      NE_RESIZE,
      E_RESIZE,
      SE_RESIZE,
      S_RESIZE,
      SW_RESIZE,
      W_RESIZE,
    };

    enum SidebarState {
      CLOSED,
      LEFT_CLOSING,
      LEFT_OPENING,
      LEFT_OPEN,
      RIGHT_CLOSING,
      RIGHT_OPENING,
      RIGHT_OPEN,
    };

    ~Window() { };

    struct TextureSlice {
      int x1, x2, y1, y2;
    };

    struct TextureData {
      std::shared_ptr<SDL_Texture> texture;
      int width;
      int height;
      bool sliced;
      TextureSlice sliceInfo;
    };

    static std::shared_ptr<Window> create(const std::string& title, int width, int height);

    bool addCursor(CursorType cursorType, const std::string& fileName, const SDL_Point& hotspot = {0, 0});

    bool addTexture(const std::string& textureName, const std::string& fileName, const TextureSlice& sliceInfo = {0, 0, 0, 0});

    ContainerPtr getSidebar();

    std::shared_ptr<TextureData> getTexture(const std::string& textureName);

    std::shared_ptr<SDL_Window> getWindow();

    void closeSidebar();

    void openSidebar();

    bool isSidebarOpen();

    bool isMouseOverSidebar();

    bool processEvent(const SDL_Event *event) override;

    void removeCursor(CursorType cursorType);

    void render(bool present = true);

    void renderDebug(bool present = true);

    void renderTexture(std::string textureName, const SDL_FRect& destRect, double angle = 0.0, float scale = 1.0);

    void setBackgroundTexture(const std::string textureName);

    void setCursor(CursorType cursorType);

    void setFocused(std::shared_ptr<Widget>);

    void setResourcePath(std::string path);

    void setTheme(const std::string& name) override;

  protected:
    Window(const std::string& title, int width, int height);

    void _setBounds(SDL_Rect& bounds) override;

  private:
    Uint64 _lastRenderCount = 0;
    std::string _resourcePath;

    std::shared_ptr<SDL_Window> _window;
    WidgetPtr _focused;
    CursorType _cursor = CursorType::DEFAULT;

    SidebarState _sidebarState = SidebarState::CLOSED;
    SDL_Rect _sidebarBounds;
    ContainerPtr _sidebarContainer;

    std::string _backgroundTexture;

    void _render(double deltaTime) override;

    std::map<std::string, std::shared_ptr<TextureData>> _textureCache;
    std::map<CursorType, std::shared_ptr<SDL_Cursor>> _cursorCache;
  };
  using WindowPtr = std::shared_ptr<SGI::Window>;
  using TextureDataPtr = std::shared_ptr<SGI::Window::TextureData>;
}

#endif // SGI_WINDOW_H
