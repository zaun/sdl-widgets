#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "debug.h"
#include "defaultfont.h"
#include "fontbook.h"

namespace SGI {
  FontBook* FontBook::_instance = nullptr;

  void FontBook::initialize()
  {
    if(_instance == nullptr) {
        _instance = new FontBook();
    }
  }

  FontBook::FontBook()
  {
    TTF_Init();
    if (atexit([](){
      TTF_Quit();
      LOG(APP, "SDL_ttf has been Shutdown");
    }) != 0) {
      ERROR(App, "atexit Error: Failed to register TTF_Init");
      TTF_Quit();
    }

    const char *basePath = SDL_GetBasePath();
    if (basePath) {
      _fontpath = std::string(basePath);
    } else {
      _fontpath = "/";
    }
  };

  FontBook::~FontBook()
  {
    for (const auto& it : _fonts) {
      TTF_CloseFont(it.second);
    }
    _fonts.clear();
  };

  void FontBook::setFontPath(const std::string path) {
    initialize();

    std::string _fontpath = path;
    if (_instance->_fontpath.empty() || _instance->_fontpath.back() != '/') {
      _instance->_fontpath += "/";
    }
  }

  void FontBook::addFont(const std::string id, const std::string fontFile)
  {
    initialize();

    _instance->_fontFiles[id] = _instance->_fontpath + fontFile;
  }

  void FontBook::addFontSize(const std::string id, int ptSize)
  {
    initialize();

    std::string key = id + "-" + std::to_string(ptSize);
    TTF_Font* font;

    if (id == "default") {
      SDL_IOStream* data = SDL_IOFromMem(defaultFont, 874988);
      font = TTF_OpenFontIO(data, true, ptSize);
    } else {
      font = TTF_OpenFont(_instance->_fontFiles[id].c_str(), ptSize);
    }

    if (font) {
      if (_instance->_fonts.find(key) != _instance->_fonts.end()) {
        TTF_CloseFont(_instance->_fonts[key]);
        _instance->_fonts.erase(key);
      }
      _instance->_fonts[key] = font;
      if (id == "default") {
        LOG(FONTBOOK, "Added font %s", key.c_str());
      } else {
        LOG(FONTBOOK, "Added font %s from %s", key.c_str(), _instance->_fontFiles[id].c_str());
      }
    } else {
      ERROR(FONTBOOK, "Error opening font: %s", TTF_GetError());
    }
  }

  bool FontBook::measure(const std::string name, int ptSize, const std::string text, int *width, int *heigt)
  {
    initialize();

    std::string key = name + "-" + std::to_string(ptSize);

    auto it = _instance->_fonts.find(key);
    if (it == _instance->_fonts.end()) {
      addFontSize(name, ptSize);
      it = _instance->_fonts.find(key);
    }

    int status = TTF_SizeUTF8(_instance->_fonts[key], text.c_str(), width, heigt);
    if (status == -1) {
      ERROR(FONTBOOK, "Error measures text: %s", TTF_GetError());
      return false;
    }

    return true;
  }

  std::shared_ptr<SDL_Surface> FontBook::render(const std::string name, int ptSize, const std::string text, const SDL_Color &fg, bool bold, bool italic, bool underline, bool strikethrough, bool overline)
  {
    initialize();

    std::string key = name + "-" + std::to_string(ptSize);

    auto it = _instance->_fonts.find(key);
    if (it == _instance->_fonts.end()) {
      addFontSize(name, ptSize);
      it = _instance->_fonts.find(key);
    }

    int style = TTF_STYLE_NORMAL;
    if (bold) {
      style += TTF_STYLE_BOLD;
    }
    if (italic) {
      style += TTF_STYLE_ITALIC;
    }
    if (underline) {
      style += TTF_STYLE_UNDERLINE;
    }
    if (strikethrough) {
      style += TTF_STYLE_STRIKETHROUGH;
    }

    TTF_SetFontStyle(_instance->_fonts[key], style);
    SDL_Surface *surface = TTF_RenderUTF8_Blended(_instance->_fonts[key], text.c_str(), fg);
    if (surface == nullptr) {
      return nullptr;
    }

    if (overline) {
      int ascent = TTF_FontAscent(_instance->_fonts[key]);
      int overlineY = surface->h - ascent - 1;

      if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
      }

      Uint32 overlineColor = SDL_MapSurfaceRGBA(surface, fg.r, fg.g, fg.b, fg.a);
      for (int x = 0; x < surface->w; ++x) {
        Uint32 *pixels = (Uint32 *)((Uint8 *)surface->pixels + overlineY * surface->pitch);
        pixels[x] = overlineColor;
      }

      if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
      }
    }

    std::shared_ptr<SDL_Surface> output(surface, SDL_DestroySurface);

    return output;
  }

}
