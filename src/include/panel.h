#ifndef SGI_PANEL_H
#define SGI_PANEL_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "container.h"

namespace SGI {
  class Panel : public Container {
  public:
    static std::shared_ptr<Panel> create();

    ~Panel() { };

    void setBackgroundTexture(const std::string textureName);
  
  protected:
    Panel() { };

  private:
    std::string _backgroundTexture;

    void _render(double deltaTime) override;
    void _renderOverlay(double deltaTime) override;

  };
  using PanelPtr = std::shared_ptr<SGI::Panel>;
}

#endif // SGI_PANEL_H
