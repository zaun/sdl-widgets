#ifndef SGI_FONTBOOK_H
#define SGI_FONTBOOK_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>

namespace SGI {
  class FontBook {
  public:
    void operator=(const FontBook &) = delete;

    static void setFontPath(const std::string path);
    static void addFont(const std::string name, const std::string fontFile);
    static void addFontSize(const std::string name, int ptSize);
    static bool measure(const std::string name, int ptSize, const std::string text, int *width, int *height);
    static std::shared_ptr<SDL_Surface> render(const std::string name, int ptSize, const std::string text, const SDL_Color &fg, bool bold = false, bool italic = false, bool underline = false, bool strikethrough = false, bool overline = false);

  private:
    FontBook();
    ~FontBook();

    static void initialize();

    static FontBook* _instance;

    std::string _fontpath;
    std::map<std::string, std::string> _fontFiles;
    std::map<std::string, TTF_Font*> _fonts;
  };
}

#endif // SGI_FONTBOOK_H
