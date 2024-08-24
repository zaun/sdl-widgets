#ifndef SGI_FLAT_H
#define SGI_FLAT_H

#include <unordered_map>
#include <SDL3/SDL.h>
#include <string>

namespace SGI {
  
  class Flat {
  public:

    ~Flat() { };

    virtual void setTheme(std::string name) = 0;

  protected:
    Flat();

    struct ButtonStateColors {
      SDL_Color defaultColor;
      SDL_Color hoverColor;
      SDL_Color pressedColor;
      SDL_Color disabledColor;
    };

    struct ButtonStyles {
      ButtonStateColors primary;
      ButtonStateColors secondary;
      ButtonStateColors warning;
      ButtonStateColors info;
      ButtonStateColors danger;
    };

    struct Colors {
      SDL_Color accentBorderColor;
      SDL_Color accentFillColor;
      SDL_Color backgroundColor;
      SDL_Color borderColor;
      SDL_Color fillColor;
      SDL_Color focusedBorderColor;
      SDL_Color textColor;
      SDL_Color textSelectColor;
    };

    struct Theme {
      std::string name;
      ButtonStyles buttons;
      Colors primary;
      Colors secondary;
      Colors terchary;
    };
    
    std::unordered_map<std::string, Theme> _themes;

    const Theme& _getTheme(const std::string& themeName) const;

    void _drawArc(SDL_Renderer* renderer, int centerX, int centerY, int radius, int startAngle, int endAngle, int lineWidth, SDL_Color lineColor, SDL_Color fillColor, bool filled);
    void _drawChevron(SDL_Renderer* renderer, SDL_Point center, int size, double angle, SDL_Color lineColor, SDL_Color fillColor, bool filled, int lineWidth);
    void _drawRoundedRect(SDL_Renderer* renderer, SDL_Rect bounds, SDL_Color lineColor, SDL_Color fillColor, bool filled, int lineWidth, int cornerRadius);

  private:
    SDL_FPoint _rotatePoint(SDL_FPoint point, SDL_Point origin, double angle);
    double _degreesToRadians(double degrees);
    SDL_Color _hexToSDLColor(const std::string& hex) const;
  };
}

#endif // SGI_FLAT_H
