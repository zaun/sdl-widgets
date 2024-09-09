#ifndef SGI_FLAT_BUTTON_H
#define SGI_FLAT_BUTTON_H

#include <map>
#include <SDL3/SDL.h>
#include <string>
#include "widget.h"
#include "flat.h"

namespace SGI {
  class FlatButton : public virtual Widget, public virtual Flat {
  public:
    static std::shared_ptr<FlatButton> create();
    static std::shared_ptr<FlatButton> create(std::string label);

    enum Style {
      Primary,
      Secondary,
      Warning,
      Info,
      Danger,
    };

    ~FlatButton();

    std::string getFontName();
    double getFontSize();
    std::string getLabel();

    bool processEvent(const SDL_Event *event) override;

    void setBorderColor(const SDL_Color &color, const SDL_Color &colorHover, const SDL_Color &colorPressed);
    void setDisabled(bool value);
    void setFontName(const std::string& fontName);
    void setFontSize(double fontSize);
    void setIcon(const std::string& value);
    void setLabel(const std::string& value);
    void setOutline(bool value);
    void setRadius(int value);
    void setStyle(const Style value);

    void setTheme(std::string name) override;

  protected:
    FlatButton() { };

  private:
    bool _outline = false;
    Flat::Theme _theme;
    Style _style = Style::Primary;
    bool _disabled = false;

    SDL_Color _textColor = {255, 255, 255, 255};
    SDL_Color _textColorHover = {255, 255, 255, 255};
    SDL_Color _textColorPressed = {255, 255, 255, 255};
    SDL_Color _borderColor = {13, 110, 253, 255};
    SDL_Color _borderColorHover = {10, 88, 202, 255};
    SDL_Color _borderColorPressed = {10, 83, 190, 255};
    SDL_Color _fillColor = {13, 110, 253, 255};
    SDL_Color _fillColorHover = {11, 94, 215, 255};
    SDL_Color _fillColorPressed = {10, 88, 202, 255};

    std::string _fontName = "default";
    double _fontSize = 16;

    std::string _icon = "";
    float _iconSize = 0;

    std::string _label = "";
    SDL_Texture* _textTexture = nullptr;
    SDL_Texture* _textTextureHover = nullptr;
    SDL_Texture* _textTexturePressed = nullptr;
    float _textWidth = 0;
    float _textHeight = 0;

    int _radius = 6;

    void _render(double deltaTime) override;

    void _updateLabel();

  };
  using FlatButtonPtr = std::shared_ptr<SGI::FlatButton>;
}

#endif // SGI_FLAT_BUTTON_H
