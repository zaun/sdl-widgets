#ifndef SGI_FLAT_DIVIDER_H
#define SGI_FLAT_DIVIDER_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "widget.h"

namespace SGI {
  class FlatDivider : public virtual Widget, public virtual Flat {
  public:

    enum Direction {
      Horizontal,
      Vertical,
    };

    static std::shared_ptr<FlatDivider> create();

    ~FlatDivider() { };

    Direction getDirection();
    int getSize();
    void setDirection(Direction direction);
    void setSize(int size);

    void setTheme(std::string name) override;
  
  protected:
    FlatDivider();

  private:
    SDL_Color _borderColor = {225, 225, 225, 255};


    Direction _direction = Direction::Horizontal;

    int _size = 1;


    void _render(double deltaTime) override;
  };
  using FlatDividerPtr = std::shared_ptr<SGI::FlatDivider>;
}

#endif // SGI_FLAT_DIVIDER_H
