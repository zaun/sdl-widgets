#ifndef SGI_FLAT_LABEL_H
#define SGI_FLAT_LABEL_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "widget.h"

namespace SGI {
  class FlatSlider : public virtual Widget, public virtual Flat {
  public:
    enum class Orientation {
      Horizontal,
      Vertical
    };

    static std::shared_ptr<FlatSlider> create();
    static std::shared_ptr<FlatSlider> create(int min, int max, int value);

    ~FlatSlider();

    int getValue();

    void setMaxValue(int maxValue);
    void setMinValue(int minValue);
    void setOrientation(Orientation orientation);
    void setTheme(std::string name) override;
    void setValue(int value);

  protected:
    FlatSlider() { };

  private:
    SDL_Color _borderColor;
    SDL_Color _fillColor;

    int _minValue = 0;
    int _maxValue = 100;
    int _value = 50;

    Orientation _orientation = Orientation::Horizontal;

    SDL_Rect _handleRect;
    bool _dragging = false;

    void _render(double deltaTime) override;
    void _updateHandlePosition();

  };
  using FlatSliderPtr = std::shared_ptr<SGI::FlatSlider>;
}

#endif // SGI_FLAT_LABEL_H
