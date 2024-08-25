#ifndef SGI_FLAT_INPUT_H
#define SGI_FLAT_INPUT_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "widget.h"

namespace SGI {
  class FlatInput : public virtual Widget, public virtual Flat {
  public:
    static std::shared_ptr<FlatInput> create();

    ~FlatInput();

    std::string addChangeListener(const Widget::Callback& handler);
    void removeChangeListener(const std::string& id);

    std::string getFontName();
    double getFontSize();
    std::string getValue();

    bool processEvent(const SDL_Event *event) override;

    void setBorderColor(const SDL_Color &color);
    void setFillColor(const SDL_Color &color);
    void setSelectColor(const SDL_Color &color);
    void setFontName(const std::string& fontName);
    void setFontSize(double fontSize);
    void setRadius(int value);
    void setValue(const std::string& value);

    void setTheme(std::string name) override;
  
  protected:
    FlatInput() { };

  private:
    SDL_Color _textColor = {225, 225, 225, 255};
    SDL_Color _borderColor = {74, 80, 86, 255};
    SDL_Color _fillColor = {44, 48, 53, 255};
    SDL_Color _selectColor = {71, 92, 122, 255};

    std::string _fontName = "default";
    double _fontSize = 16;

    std::string _value = "";

    int _cursorIndex = 0;
    int _cursorPosition = 0;
    bool _cursorBlink = true;
    double _cursorTime = 0;

    int _radius = 6;

    int _selectStart = -1;
    int _selectEnd = -1;
    SDL_FRect _selectRect;

    SDL_Texture* _textTexture = nullptr;
    float _textureOffset = 0;
    float _textWidth = 0;
    float _textHeight = 0;

    void _render(double deltaTime) override;
    int _getCursorIndexFromMouse();
    void _removeSelection();
    void _updateLabel();
    void _updatePosition();

    std::unordered_map<std::string, Callback> _changeHandelers;

  };
  using FlatInputPtr = std::shared_ptr<SGI::FlatInput>;
}

#endif // SGI_FLAT_INPUT_H
