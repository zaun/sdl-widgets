#ifndef SGI_FLAT_PANEL_H
#define SGI_FLAT_PANEL_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "container.h"

namespace SGI {
  class FlatPanel : public virtual Container, public virtual Flat {
  public:
    static std::shared_ptr<FlatPanel> create();

    ~FlatPanel() { };

    void setBorderColor(const SDL_Color &color);
    void setFillColor(const SDL_Color &color);
    void setOutline(bool value);
    void setRadius(int value);

    void setTheme(std::string name) override;
  
  protected:
    FlatPanel() { };

  private:
    bool _outline = false;

    SDL_Color _borderColor = {74, 80, 86, 255};
    SDL_Color _fillColor = {44, 48, 53, 255};

    int _radius = 6;

    void _render(double deltaTime) override;
    void _renderOverlay(double deltaTime) override;

  };
  using FlatPanelPtr = std::shared_ptr<SGI::FlatPanel>;
}

#endif // SGI_FLAT_PANEL_H
