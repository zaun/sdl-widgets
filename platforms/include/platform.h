#ifndef SGI_PLATFORM_H
#define SGI_PLATFORM_H

#include <SDL3/SDL.h>
#include <string>
#include <vector>

namespace SGI {
  class Platform {
    public:
      static std::string getUserDocumentDirectory();

      class Video {
        public:
          Video(const std::string& absolutePath);
          ~Video();
          
          void play();
          void pause();
          void stop();
          void seek(double seconds);
          double getDuration() const;
          double getCurrentTime() const;
          void setRepeat(bool value);
          void render(SDL_Renderer* renderer, SDL_Rect& contentArea);

        private:
          void* _osPlayer;
      };
  };
} // namespace SGI

#endif // SGI_PLATFORM_H
