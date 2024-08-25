#ifndef SGI_FLAT_VIDEO_H
#define SGI_FLAT_VIDEO_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include "flat.h"
#include "platform.h"
#include "widget.h"

namespace SGI {
  class FlatVideo : public virtual Widget, public virtual Flat {
  public:
    static std::shared_ptr<FlatVideo> create();

    void loadFile(std::string fullPath);
    void play();
    void stop();
    void seek(int time);
    void setLocationCallback(std::function<void(std::shared_ptr<Window>, std::shared_ptr<Widget>, float currentTime, float maxTime)> callback);
    void setRepeat(bool value);
    void setResourcePath(std::string path);
    void setTheme(std::string name) override;

    ~FlatVideo();

  protected:
    FlatVideo();

  private:
    SDL_Color _borderColor = {74, 80, 86, 255};
    SDL_Color _fillColor = {44, 48, 53, 255};

    std::string _resourcePath;
    std::unique_ptr<Platform::Video> _video;

    std::function<void(std::shared_ptr<Window>, std::shared_ptr<Widget>, float currentTime, float maxTime)> _callback = nullptr;

    void _cleanup();
    void _init();

    void _render(double deltaTime) override;

  };
  using FlatVideoPtr = std::shared_ptr<SGI::FlatVideo>;
}

#endif // SGI_FLAT_VIDEO_H
