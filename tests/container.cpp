#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <cmath>
#include <SDL3/SDL.h>
#include <iostream>

#include "container.h"
#include "window.h"

std::string testImagePath = "";

SDL_Surface* captureScreenshot(SDL_Renderer* renderer) {
  SDL_Surface* screenshot = SDL_RenderReadPixels(renderer, NULL);
  return screenshot;
}

SDL_Surface* loadSurfaceFromBMP(const char* filePath) {
  SDL_Surface* surface = SDL_LoadBMP(filePath);  
  if (surface == nullptr) {
    std::cerr << "Error loading BMP file: " << SDL_GetError() << std::endl;
    return nullptr;
  }
  return surface;
}

bool saveSurfaceToBMP(SDL_Surface* surface, const char* filePath) {
  if (surface == nullptr) {
    std::cerr << "Surface is null, cannot save to file!" << std::endl;
    return false;
  }
  if (SDL_SaveBMP(surface, filePath) != SDL_TRUE) {
    std::cerr << "Error saving surface to BMP: " << SDL_GetError() << std::endl;
    return false;
  }
  return true;
}

float compareSurfaces(SDL_Surface* surface1, SDL_Surface* surface2) {
  if (!surface1 || !surface2) {
    return 0.0f;
  }

  if (surface1->w != surface2->w || surface1->h != surface2->h || surface1->format != surface2->format) {
    return 0.0f;
  }

  int width = surface1->w;
  int height = surface1->h;
  int totalPixels = width * height;
  int matchingPixels = 0;

  SDL_LockSurface(surface1);
  SDL_LockSurface(surface2);

  Uint8* pixels1 = static_cast<Uint8*>(surface1->pixels);
  Uint8* pixels2 = static_cast<Uint8*>(surface2->pixels);
  int bpp = SDL_BITSPERPIXEL(surface1->format);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      Uint8* p1 = pixels1 + y * surface1->pitch + x * bpp;
      Uint8* p2 = pixels2 + y * surface2->pitch + x * bpp;

      Uint32 pixel1 = 0;
      Uint32 pixel2 = 0;

      switch (bpp) {
        case 1:
          pixel1 = *p1;
          pixel2 = *p2;
          break;
        case 2:
          pixel1 = *reinterpret_cast<Uint16*>(p1);
          pixel2 = *reinterpret_cast<Uint16*>(p2);
          break;
        case 3:
          if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            pixel1 = p1[0] << 16 | p1[1] << 8 | p1[2];
            pixel2 = p2[0] << 16 | p2[1] << 8 | p2[2];
          } else {
            pixel1 = p1[0] | p1[1] << 8 | p1[2] << 16;
            pixel2 = p2[0] | p2[1] << 8 | p2[2] << 16;
          }
          break;
        case 4:
          pixel1 = *reinterpret_cast<Uint32*>(p1);
          pixel2 = *reinterpret_cast<Uint32*>(p2);
          break;
      }

      if (pixel1 == pixel2) {
        ++matchingPixels;
      }
    }
  }

  SDL_UnlockSurface(surface1);
  SDL_UnlockSurface(surface2);

  return static_cast<float>(matchingPixels) / static_cast<float>(totalPixels) * 100.0f;
}

bool testRenderer(SDL_Renderer* renderer, std::string testFileName, float accuracy = 0.99f) {
  std::string filePath = testImagePath + "/" + testFileName + ".bmp";

  SDL_Surface* savedSurface = loadSurfaceFromBMP(filePath.c_str());
  if (savedSurface) {
    SDL_Surface* currentSurface = captureScreenshot(renderer);
    if (!currentSurface) {
      SDL_DestroySurface(savedSurface);
      return false;
    }

    float similarity = compareSurfaces(currentSurface, savedSurface);

    SDL_DestroySurface(savedSurface);
    SDL_DestroySurface(currentSurface);

    return similarity >= accuracy;
  } else {
    SDL_Surface* currentSurface = captureScreenshot(renderer);
    if (!currentSurface) {
      return false;
    }

    bool success = saveSurfaceToBMP(currentSurface, filePath.c_str());
    SDL_DestroySurface(currentSurface);

    return success;
  }
}

TEST_CASE("Container Widget", "[widgets]") {
  REQUIRE(SDL_Init(SDL_INIT_VIDEO) == SDL_TRUE);

}
