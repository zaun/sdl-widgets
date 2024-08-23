#include <iostream>
#include <memory>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <sstream>
#include <vector>
#include "debug.h"
#include "flattext.h"
#include "fontbook.h"

namespace SGI {
  std::shared_ptr<FlatText> FlatText::create()
  {
    std::shared_ptr<FlatText> widget = std::make_shared<FlatText>(FlatText());
    widget.get()->_self = widget;

    return widget;
  }

  std::shared_ptr<FlatText> FlatText::create(std::string value)
  {
    std::shared_ptr<FlatText> widget = FlatText::create();
    widget.get()->setValue(value);

    return widget;
  }

  FlatText::~FlatText()
  {
    for (auto& line : _lineTextures) {
      for (auto& displayTexture : line) {
        if (displayTexture.texture != nullptr) {
          SDL_DestroyTexture(displayTexture.texture);
        }
      }
    }
    _lineTextures.clear();
  }

  std::string FlatText::getFontName()
  {
    return _fontName;
  }

  double FlatText::getFontSize()
  {
    return _fontSize;
  }

  std::string FlatText::getValue()
  {
    return _value;
  }

  void FlatText::setFontName(const std::string& fontName)
  {
    _fontName = fontName;
    _updateContent();
  }

  void FlatText::setFontSize(double fontSize)
  {
    if (fontSize > 0) {
      _fontSize = fontSize;
    } else {
      _fontSize = 1;
    }
    _updateContent();
  }
  void FlatText::setValue(const std::string& value)
  {
    if (_value != value) {
      _value = value;
      _createTokens(_value, _textColor, {0, 0, 0, 0}, _fontName, _fontSize);
      _updateContent();
    }
  }

  void FlatText::setTheme(std::string name)
  {
    Flat::Theme theme = _getTheme(name);
    _textColor = theme.primaryText;
    _createTokens(_value, _textColor, {0, 0, 0, 0}, _fontName, _fontSize);
    _updateContent();
  }
  
  void FlatText::_render(double deltaTime)
  {
    if (!_root) {
      return;
    }

    if (_lineTextures.empty()) {
      _updateContent();
    }

    SDL_Rect ca = getContentArea();
    SDL_SetRenderClipRect(getRenderer().get(), &ca);

    int yOffset = ca.y;
    for (const auto& line : _lineTextures) {
      if (line.size() > 0) {
        int maxHeight = 0;
        for (const auto& displayTexture : line) {
          if (displayTexture.height > maxHeight) {
            maxHeight = displayTexture.height;
          }
        }

        int xOffset = ca.x;
        for (const auto& displayTexture : line) {
          int yPos = yOffset + maxHeight - displayTexture.height;

          SDL_FRect posRect;
          posRect.x = xOffset;
          posRect.y = yPos;
          posRect.w = displayTexture.width;
          posRect.h = displayTexture.height;

          SDL_RenderTexture(getRenderer().get(), displayTexture.texture, nullptr, &posRect);
          xOffset += displayTexture.width;
        }
        yOffset += maxHeight;
      } else {
        yOffset += _fontSize;
      }
    }
    SDL_SetRenderClipRect(getRenderer().get(), nullptr);

    Widget::_render(deltaTime);
  }

  void FlatText::_createTokens(const std::string& str, SDL_Color defaultTextColor, SDL_Color defaultBgColor, const std::string& defaultFontName, int defaultFontSize)
  {
    _tokens.clear();
    Token currentToken{defaultTextColor, defaultBgColor, defaultFontName, defaultFontSize, ""};
    size_t pos = 0;
    size_t prevPos = 0;

    while (pos < str.size()) {
      if (str[pos] == ':' && str[pos + 1] == '[') {
        if (!currentToken.content.empty()) {
          int height;
          FontBook::measure(currentToken.fontName, currentToken.fontPoints, currentToken.content, &currentToken.width, &height);
          _tokens.push_back(currentToken);
          currentToken.content.clear();
        }

        // Find the closing bracket
        size_t endPos = str.find("]:", pos);
        if (endPos == std::string::npos) {
          break;
        }

        // Get the directive inside :[ ]:
        std::string directive = str.substr(pos + 2, endPos - pos - 2);

        std::istringstream iss(directive);
        std::string command;
        std::getline(iss, command, ';');

        if (command == "fg") {
          std::string rStr, gStr, bStr;
          std::getline(iss, rStr, ';');
          std::getline(iss, gStr, ';');
          std::getline(iss, bStr, ';');

          if (rStr == "0" && gStr.empty() && bStr.empty()) {
            currentToken.textColor = defaultTextColor;
          } else {
            int r = std::stoi(rStr);
            int g = std::stoi(gStr);
            int b = std::stoi(bStr);
            currentToken.textColor = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255};
          }
        } else if (command == "bg") {
          std::string rStr, gStr, bStr;
          std::getline(iss, rStr, ';');
          std::getline(iss, gStr, ';');
          std::getline(iss, bStr, ';');

          if (rStr == "0" && gStr.empty() && bStr.empty()) {
            currentToken.bgColor = defaultTextColor;
          } else {
            int r = std::stoi(rStr);
            int g = std::stoi(gStr);
            int b = std::stoi(bStr);
            currentToken.bgColor = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255};
          }
        } else if (command == "fn") {
          std::getline(iss, currentToken.fontName, ';');
        } else if (command == "fp") {
          int fontPoints;
          iss >> fontPoints;
          currentToken.fontPoints = fontPoints;
        } else if (command == "fs") {  // Font style command
          std::string styles;
          std::getline(iss, styles, ';');

          for (char style : styles) {
            switch (style) {
              case 'B':
                currentToken.bold = true;
                break;
              case 'b':
                currentToken.bold = false;
                break;
              case 'I':
                currentToken.italic = true;
                break;
              case 'i':
                currentToken.italic = false;
                break;
              case 'U':
                currentToken.underline = true;
                break;
              case 'u':
                currentToken.underline = false;
                break;
              case 'S':
                currentToken.strikethrough = true;
                break;
              case 's':
                currentToken.strikethrough = false;
                break;
              case 'O':
                currentToken.overline = true;
                break;
              case 'o':
                currentToken.overline = false;
                break;
              default:
                break;
            }
          }
        }

        // Move past the closing ]:
        pos = endPos + 2;
      } else {
        currentToken.content += str[pos];
        ++pos;
      }
    }

    if (!currentToken.content.empty()) {
      int height;
      FontBook::measure(currentToken.fontName, currentToken.fontPoints, currentToken.content, &currentToken.width, &height);
      _tokens.push_back(currentToken);
    }
  }

  void FlatText::_updateContent()
  {
    if (getRenderer() == nullptr) {
      return;
    }

    // Clear existing textures
    for (auto& line : _lineTextures) {
      for (auto& displayTexture : line) {
        if (displayTexture.texture != nullptr) {
          SDL_DestroyTexture(displayTexture.texture);
        }
      }
    }
    _lineTextures.clear();

    // Run through the tokens createing textures
    // splitting up tokes as need for line wrapping
    std::vector<DisplayTextures> currentLine;
    int currentLineWidth = 0;

    for (const auto& token : _tokens) {
        std::string content = token.content;
        int tokenWidth = 0, tokenHeight = 0;

        const char* ptr = content.c_str();
        const char* endPtr = ptr + content.size();
        std::string chunk;
        
        while (ptr < endPtr) {
            Uint32 codepoint = SDL_StepUTF8(&ptr, NULL);
            char separatorBuff[5];
            SDL_zero(separatorBuff);
            SDL_UCS4ToUTF8(codepoint, separatorBuff);
            std::string utf8Character(separatorBuff);

            bool isSeparator = (codepoint == 0x000A || // New Line
                                codepoint == 0x000C || // Form Feed
                                codepoint == 0x0020 || // Space
                                codepoint == 0x1680 || // Ogham Space Mark
                                (codepoint >= 0x2000 && codepoint <= 0x200B) || // a space character
                                codepoint == 0x2028 || // Line Separator
                                codepoint == 0x2029 || // Paragraph Separator
                                codepoint == 0x205F || // Medium Mathematical Space
                                codepoint == 0x3000);  // Ideographic Space

            bool isLineOrParagraphSeparator = (codepoint == 0x2028 ||
                                               codepoint == 0x2029 ||
                                               codepoint == 0x000A);

            if (isSeparator) {
              if (!chunk.empty()) {
                // Process the chunk before the separator
                FontBook::measure(token.fontName, token.fontPoints, chunk, &tokenWidth, &tokenHeight);
                if (currentLineWidth + tokenWidth > getContentArea().w && !currentLine.empty()) {
                  _lineTextures.push_back(currentLine);
                  currentLine.clear();
                  currentLineWidth = 0;
                }

                // LOG(FLATTEXT, "%zu, [%s]", chunk.size(), chunk.c_str());
                auto surface = FontBook::render(token.fontName, token.fontPoints, chunk, token.textColor, token.bold, token.italic, token.underline, token.strikethrough, token.overline);
                if (surface) {
                  SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer().get(), surface.get());
                  if (texture) {
                    currentLine.push_back({texture, tokenWidth, tokenHeight});
                  } else {
                    ERROR(FLATTEXT, "Error creating texture: %s", SDL_GetError());
                  }
                } else {
                    ERROR(FLATTEXT, "Error creating surface: %s", SDL_GetError());
                }

                currentLineWidth += tokenWidth;
                chunk.clear();
              }

              if (isLineOrParagraphSeparator) {
                _lineTextures.push_back(currentLine);
                currentLine.clear();
                currentLineWidth = 0;
              } else {
                FontBook::measure(token.fontName, token.fontPoints, utf8Character, &tokenWidth, &tokenHeight);
                if (currentLineWidth + tokenWidth > getContentArea().w && !currentLine.empty()) {
                  _lineTextures.push_back(currentLine);
                  currentLine.clear();
                  currentLineWidth = 0;
                }

                auto surface = FontBook::render(token.fontName, token.fontPoints, utf8Character, token.textColor, token.bold, token.italic, token.underline, token.strikethrough, token.overline);
                if (surface) {
                  SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer().get(), surface.get());
                  if (texture) {
                    currentLine.push_back({texture, tokenWidth, tokenHeight});
                  } else {
                    ERROR(FLATTEXT, "Error creating texture: %s", SDL_GetError());
                  }
                } else {
                  ERROR(FLATTEXT, "Error creating surface: %s", SDL_GetError());
                }
                currentLineWidth += tokenWidth;
              }
            } else {
              chunk += utf8Character;
            }
        }

        // Process any remaining chunk
        if (!chunk.empty()) {
          FontBook::measure(token.fontName, token.fontPoints, chunk, &tokenWidth, &tokenHeight);
          if (currentLineWidth + tokenWidth > getContentArea().w && !currentLine.empty()) {
            _lineTextures.push_back(currentLine);
            currentLine.clear();
            currentLineWidth = 0;
          }

          // LOG(FLATTEXT, "%zu, [%s]", chunk.size(), chunk.c_str());
          auto surface = FontBook::render(token.fontName, token.fontPoints, chunk, token.textColor, token.bold, token.italic, token.underline, token.strikethrough, token.overline);
          if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer().get(), surface.get());
            if (texture) {
              currentLine.push_back({texture, tokenWidth, tokenHeight});
            } else {
              ERROR(FLATTEXT, "Error creating texture: %s", SDL_GetError());
            }
          } else {
            ERROR(FLATTEXT, "Error creating surface: %s", SDL_GetError());
          }

          currentLineWidth += tokenWidth;
      }
    }

    if (!currentLine.empty()) {
      _lineTextures.push_back(currentLine);
    }
  }
}