#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "debug.h"
#include "window.h"

namespace SGI {
  std::shared_ptr<Window> Window::create(const std::string& title, int width, int height)
  {
    std::shared_ptr<Window> widget = std::make_shared<Window>(Window(title, width, height));
    widget->_root = widget;
    widget->_self = widget;

    // Setup the window sidebar container root window
    widget->_sidebarContainer->_root = widget;

    return widget;
  }

  Window::Window(const std::string& title, int width, int height) : Container()
  {
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
      ERROR(App, "IMG_Init Error: %s", SDL_GetError());
    }

    if (atexit([](){
      IMG_Quit();
      LOG(APP, "SDL_image has been shutdown.");
    }) != 0) {
      ERROR(APP, "atexit Error: Failed to register SDL_image");
      IMG_Quit();
    }

    SDL_Window* windowPtr = SDL_CreateWindow(title.c_str(), width, height, 0);
    if (!windowPtr) {
      ERROR(App, "SDL_CreateWindow Error: %s", SDL_GetError());
    }
    _window = std::shared_ptr<SDL_Window>(windowPtr, [](SDL_Window* p) {
      LOG(MEMORY, "SDL_Window has been freed");
      SDL_DestroyWindow(p);
    });
    LOG(MEMORY, "SDL_Window has been created");

    SDL_Renderer* rendererPtr = SDL_CreateRenderer(_window.get(), NULL);
    if (!rendererPtr) {
      ERROR(App, "SDL_CreateRenderer Error: %s", SDL_GetError());
    }
    std::shared_ptr<SDL_Renderer> renderer = std::shared_ptr<SDL_Renderer>(rendererPtr, [](SDL_Renderer* p) {
      LOG(MEMORY, "SDL_Renderer has been freed");
      SDL_DestroyRenderer(p);
    });
    LOG(MEMORY, "SDL_Renderer has been created");

    _renderer = renderer;

    _bounds.x = 0;
    _bounds.y = 0;
    _bounds.w = width;
    _bounds.h = height;

    _sidebarContainer = Container::create();
    _sidebarContainer->setName("WindowSidebarContainer");
    _sidebarBounds.h = _bounds.h;
    _sidebarBounds.w = _bounds.w / 4;
    _sidebarBounds.x = -_sidebarBounds.w;
    _sidebarBounds.y = 0;
    _sidebarContainer->_setBounds(_sidebarBounds);
    _sidebarContainer->setPadding(0, 0, 0, 0);
    _sidebarContainer->_renderer = _renderer;

    _resourcePath = SDL_GetBasePath();
  };

  bool Window::addCursor(CursorType cursorType, const std::string& fileName, const SDL_Point& hotspot) {
    std::string fullPath = _resourcePath + fileName;
    SDL_Surface* surface = IMG_Load(fullPath.c_str());

    SDL_Cursor* cursor = SDL_CreateColorCursor(surface, hotspot.x, hotspot.y);
    if (!cursor) {
      SDL_DestroySurface(surface);
      ERROR(WINDOW, "Cursor not created: %s", SDL_GetError());
      return false;
    }
    SDL_DestroySurface(surface);

    auto cursorPtr = std::shared_ptr<SDL_Cursor>(cursor, [](SDL_Cursor* p) {
      if (p) {
        LOG(MEMORY, "SDL_Cursor has been freed");
        SDL_DestroyCursor(p);
      }
    });

    _cursorCache[cursorType] = cursorPtr;

    return true;
  }

  bool Window::addTexture(const std::string& textureName, const std::string& fileName, const TextureSlice& sliceInfo)
  {
    std::string fullPath = _resourcePath + fileName;
    SDL_Surface* surface = IMG_Load(fullPath.c_str());

    if (!surface) {
      LOG(TEXTURE, "Unable to load image %s: %s", fullPath.c_str(), SDL_GetError());
      return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer().get(), surface);
    SDL_DestroySurface(surface);

    if (!texture) {
      LOG(TEXTURE, "Unable to create texture from %s: %s", fullPath.c_str(), SDL_GetError());
      return false;
    }

    auto textureData = std::make_shared<TextureData>(TextureData{
      std::shared_ptr<SDL_Texture>(texture, [](SDL_Texture* p) {
        if (p) {
          LOG(MEMORY, "SDL_Texture has been freed");
          SDL_DestroyTexture(p);
        }
      }),
      sliceInfo.x1 == 0 && sliceInfo.x2 == 0 && sliceInfo.y1 == 0 && sliceInfo.y1 == 0 ? false : true,
      sliceInfo
    });

    _textureCache[textureName] = textureData;
    LOG(TEXTURE, "Texture %s added", textureName.c_str());
    return true;
  }

  ContainerPtr Window::getSidebar()
  {
    return _sidebarContainer;
  }

  std::shared_ptr<Window::TextureData> Window::getTexture(const std::string& textureName)
  {
    auto textureIt = _textureCache.find(textureName);
    if (textureIt != _textureCache.end()) {
      return textureIt->second;
    }
    return nullptr;
  }   
  
  std::shared_ptr<SDL_Window> Window::getWindow()
  {
    return _window;
  }

  void Window::openSidebar()
  {
    if (_sidebarState == SidebarState::CLOSED) {
      _sidebarState = SidebarState::LEFT_OPENING;
      LOG(WINDOW, "Sidebar opening");
    }
  }

  void Window::closeSidebar()
  {
    if (_sidebarState == SidebarState::LEFT_OPEN || _sidebarState == SidebarState::LEFT_OPENING) {
      _sidebarState = SidebarState::LEFT_CLOSING;
      LOG(WINDOW, "Sidebar closing");
    } else if (_sidebarState == SidebarState::RIGHT_OPEN || _sidebarState == SidebarState::RIGHT_OPENING) {
      _sidebarState = SidebarState::RIGHT_CLOSING;
      LOG(WINDOW, "Sidebar closing");
    }
  }

  bool Window::isSidebarOpen()
  {
    return _sidebarState != SidebarState::CLOSED;
  }

  bool Window::processEvent(const SDL_Event *event)
  {
    SDL_Window* win = SDL_GetWindowFromEvent(event);
    SDL_WindowID idA = SDL_GetWindowID(win);
    SDL_WindowID idB = SDL_GetWindowID(_window.get());

    if (idA != idB) {
      return false;
    }

    return Container::processEvent(event);
  }

  void Window::removeCursor(CursorType cursorType)
  {
    _cursorCache.erase(cursorType);
    if (_cursor == cursorType) {
      setCursor(cursorType);
    }
  }

  void Window::render(bool present)
  {
    SDL_SetRenderDrawBlendMode(getRenderer().get(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(getRenderer().get(), 0, 0, 0, 255);
    SDL_RenderClear(getRenderer().get());

    Uint64 current = SDL_GetTicks();
    double dt = 0;
    if (_lastRenderCount != 0) {
      dt = current - _lastRenderCount;
    }
    _lastRenderCount = current;

    Window::_render(dt / 1000.0);

    if (present) {
      SDL_RenderPresent(getRenderer().get());
    }
  }

  void Window::renderTexture(std::string textureName, const SDL_FRect& destRect, double angle, float scale)
  {
    std::shared_ptr<TextureData> data = getTexture(textureName);
    if (!data) {
      ERROR(TEXTURECACHE, "Texture %s not found", textureName.c_str());
      return;
    }

    if (data->sliced) {
      TextureSlice sliceInfo = data->sliceInfo;
      // TODO: figure out how to rotate this
      SDL_RenderTexture9Grid(getRenderer().get(), data->texture.get(), nullptr, sliceInfo.x1, sliceInfo.x2, sliceInfo.y1, sliceInfo.y2, scale, &destRect);
    } else {
      SDL_FPoint center = {destRect.w / 2, destRect.h / 2};
      SDL_RenderTextureRotated(getRenderer().get(), data->texture.get(), nullptr, &destRect, angle, &center, SDL_FLIP_NONE);
    }
  }

  void Window::setBackgroundTexture(const std::string textureName)
  {
    _backgroundTexture = textureName;
  }


  void Window::setCursor(CursorType cursorType) {
    auto cursorIt = _cursorCache.find(cursorType);
    if (cursorIt != _cursorCache.end()) {
      SDL_SetCursor(cursorIt->second.get());
    } else {
      SDL_Cursor* cursor;

      switch(cursorType) {
        case Window::CursorType::DEFAULT:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
          break;
        case Window::CursorType::TEXT:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
          break;
        case Window::CursorType::WAIT:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
          break;
        case Window::CursorType::CROSSHAIR:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
          break;
        case Window::CursorType::PROGRESS:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_PROGRESS);
          break;
        case Window::CursorType::NWSE_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
          break;
        case Window::CursorType::NESW_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
          break;
        case Window::CursorType::EW_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
          break;
        case Window::CursorType::NS_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
          break;
        case Window::CursorType::MOVE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
          break;
        case Window::CursorType::NOT_ALLOWED:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
          break;
        case Window::CursorType::POINTER:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
          break;
        case Window::CursorType::NW_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NW_RESIZE);
          break;
        case Window::CursorType::N_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_N_RESIZE);
          break;
        case Window::CursorType::NE_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NE_RESIZE);
          break;
        case Window::CursorType::E_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_E_RESIZE);
          break;
        case Window::CursorType::SE_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SE_RESIZE);
          break;
        case Window::CursorType::S_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_S_RESIZE);
          break;
        case Window::CursorType::SW_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SW_RESIZE);
          break;
        case Window::CursorType::W_RESIZE:
          cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_W_RESIZE);
          break;
      }

      auto cursorPtr = std::shared_ptr<SDL_Cursor>(cursor, [](SDL_Cursor* p) {
        if (p) {
          LOG(MEMORY, "SDL_Cursor has been freed");
          SDL_DestroyCursor(p);
        }
      });

      _cursorCache[cursorType] = cursorPtr;
      SDL_SetCursor(cursorPtr.get());
    }

    _cursor = cursorType;
  }

  void Window::setFocused(std::shared_ptr<Widget> widget)
  {
    if (_focused) {
      _focused->_focused = false;
    }
    _focused = nullptr;

    if (widget) {
      _focused = widget;
      _focused->_focused = true;
    }
  }

  void Window::setResourcePath(std::string path) {
    _resourcePath = path;
    if (_resourcePath.empty() || _resourcePath.back() == '/') {
      _resourcePath += "/";
    }
  }

  void Window::setTheme(const std::string& name)
  {
    Container::setTheme(name);
    _sidebarContainer->setTheme(name);
  }

  void Window::_setBounds(SDL_Rect& bounds)
  {
    Container::_setBounds(bounds);

    _sidebarBounds.y = 0;
    _sidebarBounds.h = _bounds.h;
    _sidebarBounds.w = _bounds.w / 4;
    if (_sidebarState == SidebarState::CLOSED) {
      _sidebarBounds.x = -_sidebarBounds.w;
    } else if (_sidebarState == SidebarState::LEFT_OPEN) {
      _sidebarBounds.x = _sidebarBounds.w;
    } else if (_sidebarState == SidebarState::RIGHT_OPEN) {
      _sidebarBounds.x = _bounds.w - _sidebarBounds.w;
    }

    _sidebarContainer->_setBounds(_sidebarBounds);
  }

  void Window::_render(double deltaTime)
  {
    if (!_backgroundTexture.empty()) {
      renderTexture(_backgroundTexture, SDL_FRect{(float)_bounds.x, (float)_bounds.y, (float)_bounds.w, (float)_bounds.h});
    }

    Container::_render(deltaTime);
    Container::_renderOverlay(deltaTime);

    if (_sidebarState == SidebarState::LEFT_OPENING) {
      _sidebarBounds.x += 1500 * deltaTime;
      if (_sidebarBounds.x >= 0) {
        _sidebarBounds.x = 0;
        _sidebarState = SidebarState::LEFT_OPEN;
        LOG(WINDOW, "Sidebar opened");
      }
      _sidebarContainer->_setBounds(_sidebarBounds);
    }

    if (_sidebarState == SidebarState::LEFT_CLOSING) {
      _sidebarBounds.x -= 1500 * deltaTime;
      if (_sidebarBounds.x <= -_sidebarBounds.w) {
        _sidebarBounds.x = -_sidebarBounds.w;
        _sidebarState = SidebarState::CLOSED;
        LOG(WINDOW, "Sidebar closed");
      }
      _sidebarContainer->_setBounds(_sidebarBounds);
    }

    if (_sidebarState != SidebarState::CLOSED) {
      _sidebarContainer->_render(deltaTime);
      _sidebarContainer->_renderOverlay(deltaTime);
    }
  }
}
