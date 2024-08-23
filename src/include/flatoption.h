#ifndef SGI_FLAT_OPTION_H
#define SGI_FLAT_OPTION_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "widget.h"

namespace SGI {
  class FlatOption : public virtual Widget, public virtual Flat {
  public:

    enum TextJustification {
      Left,
      Center,
      Right
    };

    static std::shared_ptr<FlatOption> create();
    static std::shared_ptr<FlatOption> create(std::string label);

    ~FlatOption();

    std::string getFontName();
    double getFontSize();
    std::string getLabel();
    bool getValue();

    bool processEvent(const SDL_Event *event) override;

    void setFontName(const std::string& fontName);
    void setFontSize(double fontSize);
    void setLabel(const std::string& value);
    void setValue(bool value);

    void setTheme(std::string name) override;
  
  protected:
    FlatOption() { };

  private:
    SDL_Color _textColor = {225, 225, 225, 255};
    SDL_Color _borderColor = {225, 225, 225, 255};
    SDL_Color _fillColor = {225, 225, 225, 255};

    std::string _fontName = "default";
    double _fontSize = 16;

    std::string _label = "";
    bool _value;

    SDL_Texture* _textTexture = nullptr;
    float _textWidth = 0;
    float _textHeight = 0;

    void _render(double deltaTime) override;
    void _updateLabel();

  };
  using FlatOptionPtr = std::shared_ptr<SGI::FlatOption>;
}

#endif // SGI_FLAT_OPTION_H
