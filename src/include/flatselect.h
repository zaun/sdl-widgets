#ifndef SGI_FLAT_SELECT_H
#define SGI_FLAT_SELECT_H

#include <map>
#include <SDL3/SDL.h>
#include <string>
#include "widget.h"
#include "flat.h"

namespace SGI {
  class FlatSelect : public virtual Widget, public virtual Flat {
  public:
    static std::shared_ptr<FlatSelect> create();

    ~FlatSelect();

    std::string addChangeListener(const Widget::Callback& handler);
    void removeChangeListener(const std::string& id);

    std::string getFontName();
    double getFontSize();
    std::string getValue();

    bool processEvent(const SDL_Event *event) override;

    void setBorderColor(const SDL_Color &color);
    void setFillColor(const SDL_Color &color);
    void setFontColor(const SDL_Color &color);
    void setFontName(const std::string& fontName);
    void setFontSize(double fontSize);
    void setNoOptionsLabel(const std::string &value);
    void setOptions(const std::vector<std::string>& options);
    void setRadius(int value);
    void setValue(const std::string& value);

    void setTheme(std::string name) override;

  protected:
    FlatSelect() { };

  private:
    bool _opened = false;

    SDL_Color _textColor = {225, 225, 225, 255};
    SDL_Color _borderColor = {74, 80, 86, 255};
    SDL_Color _fillColor = {44, 48, 53, 255};


    std::string _fontName = "default";
    double _fontSize = 16;

    std::string _value = "";
    std::vector<std::string> _options;

    SDL_Texture* _optionsTexture = nullptr;
    SDL_Rect _optionsBounds;
    SDL_Rect _optionsContent;
    float _optionsWidth = 0;
    float _optionsHeight = 0;
    float _optionItemHeight = 0;
    float _optionsScrollOffset = 0;
    int _optionItemSpace = 3;
    int _mouseOverOptionIndex = 0;

    SDL_Texture* _valueTexture = nullptr;
    float _valueWidth = 0;
    float _valueHeight = 0;

    std::string _noOptionsText = "No data available";
    SDL_Texture* _noOptionsTexture = nullptr;
    float _noOptionsWidth = 0;
    float _noOptionsHeight = 0;

    int _radius = 6;

    bool _isMouseOverOverlay();

    void _render(double deltaTime) override;
    void _renderOverlay(double deltaTime) override;

    void _updateLabel();
    void _updateNoOptionsLadel();
    void _updateOptionsLadel();
    void _updateSelected();

    std::unordered_map<std::string, Callback> _inputHandelers;

  };
  using FlatSelectPtr = std::shared_ptr<SGI::FlatSelect>;
}

#endif // SGI_FLAT_SELECT_H
