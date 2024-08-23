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

    struct Color {
      SDL_Color defaultColor;
      SDL_Color hoverColor;
      SDL_Color pressedColor;
    };

    struct ButtonColors {
      Color primary;
      Color secondary;
      Color warning;
      Color info;
      Color danger;
    };

    struct Theme {
      std::string name;
      SDL_Color primaryBackground;
      SDL_Color secondaryBackground;
      SDL_Color primaryText;
      SDL_Color secondaryText;
      SDL_Color primaryBorder;
      SDL_Color focusBorder;
      SDL_Color primaryFill;
      SDL_Color secondaryFill;
      SDL_Color primarySelection;
      SDL_Color textSelection;
      SDL_Color accent;
      ButtonColors buttons;
    };
    
    std::unordered_map<std::string, Theme> _themes;

    const Theme& _getTheme(const std::string& themeName) const;

    void _drawArc(SDL_Renderer* renderer, int centerX, int centerY, int radius, int startAngle, int endAngle, int lineWidth, SDL_Color lineColor, SDL_Color fillColor, bool filled);
    void _drawChevron(SDL_Renderer* renderer, SDL_Point center, int size, double angle, SDL_Color lineColor, SDL_Color fillColor, bool filled, int lineWidth);
    void _drawRoundedRect(SDL_Renderer* renderer, SDL_Rect bounds, SDL_Color lineColor, SDL_Color fillColor, bool filled, int lineWidth, int cornerRadius);

    SDL_Color _darkTextColor = {225, 225, 225, 255};
    SDL_Color _darkBorderColor = {74, 80, 86, 255};
    SDL_Color _darkFillColor = {44, 48, 53, 255};
    SDL_Color _darkSelectColor = {71, 92, 122, 255};

    SDL_Color _lightTextColor = {33, 37, 41, 255};
    SDL_Color _lightBorderColor = {239, 240, 242, 255};
    SDL_Color _lightFillColor = {225, 225, 225, 255};
    SDL_Color _lightSelectColor = {187, 214, 251, 255};

  private:
    SDL_FPoint _rotatePoint(SDL_FPoint point, SDL_Point origin, double angle);
    double _degreesToRadians(double degrees);
    SDL_Color _hexToSDLColor(const std::string& hex) const;
  };
}

#endif // SGI_FLAT_H
