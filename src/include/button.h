#ifndef SGI_BUTTON_H
#define SGI_BUTTON_H

#include <map>
#include <SDL3/SDL.h>
#include <string>
#include "widget.h"

namespace SGI {
  class Button : public Widget {
  public:
    static std::shared_ptr<Button> create();

    ~Button() { };  

  protected:
    Button() { };

  private:
    std::string _buttonTexture = "button";
    std::string _buttonHoverTexture = "buttonHover";
    std::string _buttonPressedTexture = "buttonPressed";
    std::string _buttonDisabledTexture = "buttonDisabled";

    SDL_Color _color = { 255, 255, 255, 255 };

    std::string _fontName = "future";
    double _fontSize = 18;

    std::string _lable = "Hello";

    std::shared_ptr<SDL_Surface> _textSurface = nullptr;
    SDL_Texture* _textTexture = nullptr;
    float _textWidth = 0;
    float _textHeight = 0;

    void _render(double deltaTime) override;
    void _updateLabel();
  };
  using ButtonPtr = std::shared_ptr<SGI::Button>;
}

#endif // SGI_BUTTON_H
