#include <cmath>
#include <SDL3/SDL.h>
#include "debug.h"
#include "flat.h"

namespace SGI {
  Flat::Flat()
  {
    _themes["Light"] = Theme{
      "Light",
      ButtonStyles{
        { _hexToSDLColor("#0055FF"), _hexToSDLColor("#0044CC"), _hexToSDLColor("#003399"), _hexToSDLColor("#0044CC") }, // Primary
        { _hexToSDLColor("#CCCCCC"), _hexToSDLColor("#BBBBBB"), _hexToSDLColor("#AAAAAA"), _hexToSDLColor("#BBBBBB") }, // Secondary
        { _hexToSDLColor("#FFCC00"), _hexToSDLColor("#D4A100"), _hexToSDLColor("#B88600"), _hexToSDLColor("#D4A100") }, // Tertiary
        { _hexToSDLColor("#00CC99"), _hexToSDLColor("#009966"), _hexToSDLColor("#007744"), _hexToSDLColor("#009966") }, // Success
        { _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC0000"), _hexToSDLColor("#990000"), _hexToSDLColor("#CC0000") }  // Danger
      },
      Colors{
        _hexToSDLColor("#3399FF"), _hexToSDLColor("#2E89E6"), _hexToSDLColor("#FFFFFF"), _hexToSDLColor("#D1D1D1"),
        _hexToSDLColor("#E0E0E0"), _hexToSDLColor("#0044CC"), _hexToSDLColor("#000000"), _hexToSDLColor("#004EAA") // Text Select Color adjusted
      },
      Colors{
        _hexToSDLColor("#CCCCCC"), _hexToSDLColor("#B8B8B8"), _hexToSDLColor("#F0F0F0"), _hexToSDLColor("#D4D4D4"),
        _hexToSDLColor("#F0F0F0"), _hexToSDLColor("#BBBBBB"), _hexToSDLColor("#666666"), _hexToSDLColor("#CCCCCC")
      },
      Colors{
        _hexToSDLColor("#F0F0F0"), _hexToSDLColor("#D1D1D1"), _hexToSDLColor("#FFFFFF"), _hexToSDLColor("#D1D1D1"),
        _hexToSDLColor("#F0F0F0"), _hexToSDLColor("#0044CC"), _hexToSDLColor("#000000"), _hexToSDLColor("#004EAA")
      }
    };


    _themes["Dark"] = Theme{
      "Dark",
      ButtonStyles{
        { _hexToSDLColor("#007ACC"), _hexToSDLColor("#006BB3"), _hexToSDLColor("#005A99"), _hexToSDLColor("#006BB3") }, // Primary
        { _hexToSDLColor("#3C3C3C"), _hexToSDLColor("#4A4A4A"), _hexToSDLColor("#5A5A5A"), _hexToSDLColor("#4A4A4A") }, // Secondary
        { _hexToSDLColor("#FFCC00"), _hexToSDLColor("#D4A100"), _hexToSDLColor("#B88600"), _hexToSDLColor("#D4A100") }, // Tertiary
        { _hexToSDLColor("#00CC99"), _hexToSDLColor("#009966"), _hexToSDLColor("#007744"), _hexToSDLColor("#009966") }, // Success
        { _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC0000"), _hexToSDLColor("#990000"), _hexToSDLColor("#CC0000") }  // Danger
      },
      Colors{
        _hexToSDLColor("#E6E6E6"), _hexToSDLColor("#CCCCCC"), _hexToSDLColor("#1E1E1E"), _hexToSDLColor("#2D2D2D"),
        _hexToSDLColor("#252526"), _hexToSDLColor("#006BB3"), _hexToSDLColor("#D4D4D4"), _hexToSDLColor("#6D7991") // Text Select Color adjusted
      },
      Colors{
        _hexToSDLColor("#3C3C3C"), _hexToSDLColor("#363636"), _hexToSDLColor("#2D2D2D"), _hexToSDLColor("#4A4A4A"),
        _hexToSDLColor("#2D2D2D"), _hexToSDLColor("#4A4A4A"), _hexToSDLColor("#A6A6A6"), _hexToSDLColor("#6D7991")
      },
      Colors{
        _hexToSDLColor("#2D2D2D"), _hexToSDLColor("#2A2A2A"), _hexToSDLColor("#1E1E1E"), _hexToSDLColor("#2D2D2D"),
        _hexToSDLColor("#252526"), _hexToSDLColor("#006BB3"), _hexToSDLColor("#D4D4D4"), _hexToSDLColor("#6D7991")
      }
    };


    _themes["Alien Green"] = Theme{
      "Alien Green",
      ButtonStyles{
        { _hexToSDLColor("#00FF00"), _hexToSDLColor("#00CC00"), _hexToSDLColor("#009900"), _hexToSDLColor("#00CC00") }, // Primary
        { _hexToSDLColor("#006600"), _hexToSDLColor("#004C00"), _hexToSDLColor("#003300"), _hexToSDLColor("#004C00") }, // Secondary
        { _hexToSDLColor("#FFCC00"), _hexToSDLColor("#D4A100"), _hexToSDLColor("#B88600"), _hexToSDLColor("#D4A100") }, // Tertiary
        { _hexToSDLColor("#66FF66"), _hexToSDLColor("#4DB84D"), _hexToSDLColor("#33CC33"), _hexToSDLColor("#4DB84D") }, // Success
        { _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC0000"), _hexToSDLColor("#990000"), _hexToSDLColor("#CC0000") }  // Danger
      },
      Colors{
        _hexToSDLColor("#00FF00"), _hexToSDLColor("#00E600"), _hexToSDLColor("#001A00"), _hexToSDLColor("#002200"),
        _hexToSDLColor("#003300"), _hexToSDLColor("#00CC00"), _hexToSDLColor("#A6FFA6"), _hexToSDLColor("#00734E") // Text Select Color adjusted
      },
      Colors{
        _hexToSDLColor("#00FF00"), _hexToSDLColor("#00E600"), _hexToSDLColor("#002200"), _hexToSDLColor("#004400"),
        _hexToSDLColor("#004400"), _hexToSDLColor("#4DB84D"), _hexToSDLColor("#66CC66"), _hexToSDLColor("#00734E")
      },
      Colors{
        _hexToSDLColor("#00FF00"), _hexToSDLColor("#00E600"), _hexToSDLColor("#001A00"), _hexToSDLColor("#002200"),
        _hexToSDLColor("#003300"), _hexToSDLColor("#00CC00"), _hexToSDLColor("#A6FFA6"), _hexToSDLColor("#00734E")
      }
    };


    _themes["Techie Blue"] = Theme{
      "Techie Blue",
      ButtonStyles{
        { _hexToSDLColor("#00BFFF"), _hexToSDLColor("#0099CC"), _hexToSDLColor("#006699"), _hexToSDLColor("#0099CC") }, // Primary
        { _hexToSDLColor("#004080"), _hexToSDLColor("#003366"), _hexToSDLColor("#00264D"), _hexToSDLColor("#003366") }, // Secondary
        { _hexToSDLColor("#FFA500"), _hexToSDLColor("#D48800"), _hexToSDLColor("#CC7000"), _hexToSDLColor("#D48800") }, // Tertiary
        { _hexToSDLColor("#66CFFF"), _hexToSDLColor("#4DB8E6"), _hexToSDLColor("#3399CC"), _hexToSDLColor("#4DB8E6") }, // Success
        { _hexToSDLColor("#FF4D4D"), _hexToSDLColor("#FF1A1A"), _hexToSDLColor("#B30000"), _hexToSDLColor("#FF1A1A") }  // Danger
      },
      Colors{
        _hexToSDLColor("#00BFFF"), _hexToSDLColor("#00ACED"), _hexToSDLColor("#001F33"), _hexToSDLColor("#00264D"),
        _hexToSDLColor("#003366"), _hexToSDLColor("#0099CC"), _hexToSDLColor("#80D4FF"), _hexToSDLColor("#005480") // Text Select Color adjusted
      },
      Colors{
        _hexToSDLColor("#00BFFF"), _hexToSDLColor("#00ACED"), _hexToSDLColor("#00264D"), _hexToSDLColor("#004080"),
        _hexToSDLColor("#004080"), _hexToSDLColor("#4DB8E6"), _hexToSDLColor("#80CFFF"), _hexToSDLColor("#005480")
      },
      Colors{
        _hexToSDLColor("#00BFFF"), _hexToSDLColor("#00ACED"), _hexToSDLColor("#001F33"), _hexToSDLColor("#00264D"),
        _hexToSDLColor("#003366"), _hexToSDLColor("#0099CC"), _hexToSDLColor("#80D4FF"), _hexToSDLColor("#005480")
      }
    };


    _themes["Devil Red"] = Theme{
      "Devilish Red",
      ButtonStyles{
        { _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC0000"), _hexToSDLColor("#990000"), _hexToSDLColor("#CC0000") }, // Primary
        { _hexToSDLColor("#660000"), _hexToSDLColor("#4D0000"), _hexToSDLColor("#330000"), _hexToSDLColor("#4D0000") }, // Secondary
        { _hexToSDLColor("#FFA500"), _hexToSDLColor("#CC8400"), _hexToSDLColor("#996300"), _hexToSDLColor("#CC8400") }, // Tertiary
        { _hexToSDLColor("#FF6666"), _hexToSDLColor("#E64D4D"), _hexToSDLColor("#CC3333"), _hexToSDLColor("#E64D4D") }, // Success
        { _hexToSDLColor("#000000"), _hexToSDLColor("#262626"), _hexToSDLColor("#333333"), _hexToSDLColor("#262626") }  // Danger
      },
      Colors{
        _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC2900"), _hexToSDLColor("#330000"), _hexToSDLColor("#590000"),
        _hexToSDLColor("#4D0000"), _hexToSDLColor("#CC0000"), _hexToSDLColor("#FF9999"), _hexToSDLColor("#800000") // Text Select Color adjusted
      },
      Colors{
        _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC2900"), _hexToSDLColor("#590000"), _hexToSDLColor("#800000"),
        _hexToSDLColor("#660000"), _hexToSDLColor("#E64D4D"), _hexToSDLColor("#FF6666"), _hexToSDLColor("#800000")
      },
      Colors{
        _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC2900"), _hexToSDLColor("#330000"), _hexToSDLColor("#590000"),
        _hexToSDLColor("#4D0000"), _hexToSDLColor("#CC0000"), _hexToSDLColor("#FF9999"), _hexToSDLColor("#800000")
      }
    };

    _themes["Pastel"] = Theme{
      "Pastel",
      ButtonStyles{
        { _hexToSDLColor("#FFB3B3"), _hexToSDLColor("#FF9999"), _hexToSDLColor("#FF6666"), _hexToSDLColor("#FF9999") }, // Primary
        { _hexToSDLColor("#FFFF99"), _hexToSDLColor("#FFFF66"), _hexToSDLColor("#FFFF33"), _hexToSDLColor("#FFFF66") }, // Secondary
        { _hexToSDLColor("#FFCCFF"), _hexToSDLColor("#FF99FF"), _hexToSDLColor("#FF66FF"), _hexToSDLColor("#FF99FF") }, // Tertiary
        { _hexToSDLColor("#99FF99"), _hexToSDLColor("#66FF66"), _hexToSDLColor("#33FF33"), _hexToSDLColor("#66FF66") }, // Success
        { _hexToSDLColor("#FF3333"), _hexToSDLColor("#CC0000"), _hexToSDLColor("#990000"), _hexToSDLColor("#CC0000") }  // Danger
      },
      Colors{
        _hexToSDLColor("#FFB3B3"), _hexToSDLColor("#E680B2"), _hexToSDLColor("#FFFFFF"), _hexToSDLColor("#FFD9D9"),
        _hexToSDLColor("#FFE6E6"), _hexToSDLColor("#FF9999"), _hexToSDLColor("#FF3333"), _hexToSDLColor("#FF9999") // Text Select Color adjusted
      },
      Colors{
        _hexToSDLColor("#FFB3B3"), _hexToSDLColor("#80E6B2"), _hexToSDLColor("#FFFFE6"), _hexToSDLColor("#FFFF99"),
        _hexToSDLColor("#FFFFE6"), _hexToSDLColor("#FFFF66"), _hexToSDLColor("#FF6666"), _hexToSDLColor("#FFFF99")
      },
      Colors{
        _hexToSDLColor("#FFE6E6"), _hexToSDLColor("#B2E680"), _hexToSDLColor("#FFFFFF"), _hexToSDLColor("#FFE6FF"),
        _hexToSDLColor("#FFFFFF"), _hexToSDLColor("#FFB3FF"), _hexToSDLColor("#FF3333"), _hexToSDLColor("#FF9999")
      }
    };
  }

  const Flat::Theme& Flat::_getTheme(const std::string& themeName) const
  {
    auto it = _themes.find(themeName);
    if (it != _themes.end()) {
      return it ->second;
    }
    LOG(FLAT, "Theme \"%s\" not found, using Dark", themeName.c_str());
    return _themes.at("Dark");
  }
  
  void Flat::_drawArc(SDL_Renderer* renderer, int centerX, int centerY, int radius, int startAngle, int endAngle, int lineWidth, SDL_Color lineColor, SDL_Color fillColor, bool filled)
  {
    startAngle = startAngle % 360;
    endAngle = endAngle % 360;

    if (endAngle < startAngle) {
      endAngle += 360;
    }

    // Draw filled arc if needed
    if (filled) {
      SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
      for (int angle = startAngle; angle <= endAngle; ++angle) {
        for (int r = 0; r <= radius; ++r) {
          int x = static_cast<int>(centerX + r * cos(_degreesToRadians(angle)));
          int y = static_cast<int>(centerY + r * sin(_degreesToRadians(angle)));
          SDL_RenderPoint(renderer, x, y);
        }
      }
    }

    // Draw border arc
    SDL_SetRenderDrawColor(renderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    for (int angle = startAngle; angle <= endAngle; ++angle) {
      int x = static_cast<int>(centerX + radius * cos(_degreesToRadians(angle)));
      int y = static_cast<int>(centerY + radius * sin(_degreesToRadians(angle)));
      SDL_RenderPoint(renderer, x, y);
    }
  }

  void Flat::_drawChevron(SDL_Renderer* renderer, SDL_Point center, int size, double angle, SDL_Color lineColor, SDL_Color fillColor, bool filled, int lineWidth)
  {
    SDL_FPoint points[3];

    int halfSize = size / 2;
    int height = static_cast<int>(halfSize * std::sqrt(3));

    // Define the points of the chevron relative to the center
    points[0] = { static_cast<float>(center.x), static_cast<float>(center.y - (2 * height / 3))};
    points[1] = { static_cast<float>(center.x - halfSize), static_cast<float>(center.y + (height / 3))};
    points[2] = { static_cast<float>(center.x + halfSize), static_cast<float>(center.y + (height / 3))};

    for (int i = 0; i < 3; ++i) {
      points[i] = _rotatePoint(points[i], center, angle);
    }
    
    if (filled) {
      // Use SDL_RenderGeometry for filled rendering
      SDL_Vertex vertices[3];
      for (int i = 0; i < 3; ++i) {
        vertices[i].position.x = static_cast<float>(points[i].x);
        vertices[i].position.y = static_cast<float>(points[i].y);
        vertices[i].color.r = static_cast<float>(fillColor.r / 255.0);
        vertices[i].color.g = static_cast<float>(fillColor.g / 255.0);
        vertices[i].color.b = static_cast<float>(fillColor.b / 255.0);
        vertices[i].color.a = static_cast<float>(fillColor.a / 255.0);
      }
      SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);
    }

    // Set the line color
    SDL_SetRenderDrawColor(renderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);

    // Draw the outline of the chevron
    for (int i = 0; i < 3; ++i) {
        SDL_RenderLine(renderer, points[i].x, points[i].y, points[(i + 1) % 3].x, points[(i + 1) % 3].y);
    }
  }

  void Flat::_drawRoundedRect(SDL_Renderer* renderer, SDL_Rect bounds, SDL_Color lineColor, SDL_Color fillColor, bool filled, int lineWidth, int cornerRadius)
  {
    // Draw filled rounded rectangle first if filled is true
    if (filled) {
      // Set fill color and draw the filled rectangle without the corners
      SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);

      // Draw the central rectangle
      SDL_FRect innerRect = { static_cast<float>(bounds.x) + static_cast<float>(cornerRadius), static_cast<float>(bounds.y), static_cast<float>(bounds.w) - 2 * static_cast<float>(cornerRadius), static_cast<float>(bounds.h) };
      SDL_RenderFillRect(renderer, &innerRect);

      // Draw the vertical sides
      SDL_FRect leftRect = { static_cast<float>(bounds.x), static_cast<float>(bounds.y) + static_cast<float>(cornerRadius), static_cast<float>(cornerRadius), static_cast<float>(bounds.h) - 2 * static_cast<float>(cornerRadius) };
      SDL_FRect rightRect = { static_cast<float>(bounds.x) + static_cast<float>(bounds.w) - static_cast<float>(cornerRadius), static_cast<float>(bounds.y) + static_cast<float>(cornerRadius), static_cast<float>(cornerRadius), static_cast<float>(bounds.h) - 2 * static_cast<float>(cornerRadius) };
      SDL_RenderFillRect(renderer, &leftRect);
      SDL_RenderFillRect(renderer, &rightRect);
    }

    SDL_SetRenderDrawColor(renderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);

    SDL_FRect topRect = { static_cast<float>(bounds.x) + static_cast<float>(cornerRadius), static_cast<float>(bounds.y), static_cast<float>(bounds.w) - 2 * static_cast<float>(cornerRadius), static_cast<float>(lineWidth) };
    SDL_FRect bottomRect = { static_cast<float>(bounds.x) + static_cast<float>(cornerRadius), static_cast<float>(bounds.y) + static_cast<float>(bounds.h) - static_cast<float>(lineWidth), static_cast<float>(bounds.w) - 2 * static_cast<float>(cornerRadius), static_cast<float>(lineWidth) };
    SDL_FRect leftBorderRect = { static_cast<float>(bounds.x), static_cast<float>(bounds.y) + static_cast<float>(cornerRadius), static_cast<float>(lineWidth), static_cast<float>(bounds.h) - 2 * static_cast<float>(cornerRadius) };
    SDL_FRect rightBorderRect = { static_cast<float>(bounds.x) + static_cast<float>(bounds.w) - static_cast<float>(lineWidth), static_cast<float>(bounds.y) + static_cast<float>(cornerRadius), static_cast<float>(lineWidth), static_cast<float>(bounds.h) - 2 * static_cast<float>(cornerRadius) };
    SDL_RenderFillRect(renderer, &topRect);
    SDL_RenderFillRect(renderer, &bottomRect);
    SDL_RenderFillRect(renderer, &leftBorderRect);
    SDL_RenderFillRect(renderer, &rightBorderRect);
  
    int arcRadius = cornerRadius - lineWidth / 2;
    _drawArc(renderer, static_cast<float>(bounds.x + cornerRadius), static_cast<float>(bounds.y + cornerRadius), arcRadius, 180, 270, lineWidth, lineColor, fillColor, filled);
    _drawArc(renderer, static_cast<float>(bounds.x + bounds.w - cornerRadius), static_cast<float>(bounds.y + cornerRadius), arcRadius, 270, 360, lineWidth, lineColor, fillColor, filled);
    _drawArc(renderer, static_cast<float>(bounds.x + cornerRadius), static_cast<float>(bounds.y + bounds.h - cornerRadius), arcRadius, 90, 180, lineWidth, lineColor, fillColor, filled);
    _drawArc(renderer, static_cast<float>(bounds.x + bounds.w - cornerRadius), static_cast<float>(bounds.y + bounds.h - cornerRadius), arcRadius, 0, 90, lineWidth, lineColor, fillColor, filled);
  }

  SDL_FPoint Flat::_rotatePoint(SDL_FPoint point, SDL_Point origin, double angle)
  {
    double radAngle = _degreesToRadians(angle);
    double s = sin(radAngle);
    double c = cos(radAngle);

    SDL_FPoint rotated;
    rotated.x = static_cast<float>(c * (point.x - origin.x) - s * (point.y - origin.y) + origin.x);
    rotated.y = static_cast<float>(s * (point.x - origin.x) + c * (point.y - origin.y) + origin.y);

    return rotated;
  }

  double Flat::_degreesToRadians(double degrees)
  {
    return degrees * M_PI / 180.0;
  }

  SDL_Color Flat::_hexToSDLColor(const std::string& hex) const
  {
    SDL_Color color = {0, 0, 0, 255};

    if (hex.length() == 7 && hex[0] == '#') {
      color.r = std::stoi(hex.substr(1, 2), nullptr, 16);
      color.g = std::stoi(hex.substr(3, 2), nullptr, 16);
      color.b = std::stoi(hex.substr(5, 2), nullptr, 16);
    }
    return color;
  }

}
