#ifndef SGI_FLAT_LABEL_H
#define SGI_FLAT_LABEL_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "widget.h"

namespace SGI {
  class FlatLabel : public virtual Widget, public virtual Flat {
  public:

    enum TextJustification {
      Left,
      Center,
      Right
    };

    static std::shared_ptr<FlatLabel> create();
    static std::shared_ptr<FlatLabel> create(std::string label);

    ~FlatLabel();

    std::string getFontName();
    double getFontSize();
    TextJustification getJustification();
    std::string getLabel();

    void setFontName(const std::string& fontName);
    void setFontSize(double fontSize);
    void setJustification(TextJustification justification);
    void setLabel(const std::string& value);

    void setTheme(std::string name) override;
  
  protected:
    FlatLabel() { };

  private:
    SDL_Color _textColor = {225, 225, 225, 255};

    std::string _fontName = "default";
    double _fontSize = 16;

    std::string _label = "";

    TextJustification _justification = TextJustification::Left;

    SDL_Texture* _textTexture = nullptr;
    float _textWidth = 0;
    float _textHeight = 0;

    void _render(double deltaTime) override;
    void _updateLabel();

  };
  using FlatLabelPtr = std::shared_ptr<SGI::FlatLabel>;
}

#endif // SGI_FLAT_LABEL_H
