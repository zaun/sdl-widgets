#ifndef SGI_FLAT_TEXT_H
#define SGI_FLAT_TEXT_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "widget.h"

namespace SGI {
  class FlatText : public virtual Widget, public virtual Flat {
  public:

    static std::shared_ptr<FlatText> create();
    static std::shared_ptr<FlatText> create(std::string value);

    ~FlatText();

    std::string getFontName();
    double getFontSize();
    std::string getValue();

    bool processEvent(const SDL_Event *event) override;

    void setFontName(const std::string& fontName);
    void setFontSize(double fontSize);
    void setValue(const std::string& value);

    void setTheme(std::string name) override;
  
  protected:
    FlatText() { };

  private:
    struct Token {
      SDL_Color textColor;
      SDL_Color bgColor;
      std::string fontName;
      int fontPoints;
      std::string content;
      int width;
      bool bold;
      bool italic;
      bool underline;
      bool strikethrough;
      bool overline;
    };
    std::vector<Token> _tokens;

    struct DisplayTextures {
      SDL_Texture* texture;
      int width;
      int height;
    };
    std::vector<std::vector<DisplayTextures>> _lineTextures;
    int _totalHeight = 0;
    int _totalOffset = 0;

    std::string _fontName = "default";
    double _fontSize = 16;

    std::string _value = "";

    SDL_Color _textColor = {225, 225, 225, 255};

    void _render(double deltaTime) override;
    void _createTokens(const std::string& str, SDL_Color defaultTextColor, SDL_Color defaultBgColor, const std::string& defaultFontName, int defaultFontSize);
    void _updateContent();

  };
  using FlatTextPtr = std::shared_ptr<SGI::FlatText>;
}

#endif // SGI_FLAT_TEXT_H
