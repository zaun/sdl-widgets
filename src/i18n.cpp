#include <locale>
#include <string>
#include <SDL3/SDL.h>
#include "i18n.h"

namespace SGI {

  std::string I18N::_currentLang = "";
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>> I18N::_translations;

  std::unordered_map<std::string, std::string> I18N::_currencySymbols = {
    {"af", "R"},
    {"bg", "лв"},
    {"cs", "Kč"},
    {"de", "€"},
    {"el", "€"},
    {"en", "$"},
    {"es", "€"},
    {"eu", "€"},
    {"fi", "€"},
    {"fr", "€"},
    {"he", "₪"},
    {"hu", "Ft"},
    {"it", "€"},
    {"jp", "¥"},
    {"ko", "₩"},
    {"nl", "€"},
    {"pt", "€"},
    {"ru", "₽"},
    {"sk", "€"},
    {"zh", "¥"}
  };

  std::unordered_map<std::string, std::string> I18N::_currencyNames = {
    {"af", "South African Rand"},
    {"bg", "Bulgarian Lev"},
    {"cs", "Czech Koruna"},
    {"de", "Euro"},
    {"el", "Euro"},
    {"en", "US Dollar"},
    {"es", "Euro"},
    {"eu", "Euro"},
    {"fi", "Euro"},
    {"fr", "Euro"},
    {"he", "Israeli New Shekel"},
    {"hu", "Hungarian Forint"},
    {"it", "Euro"},
    {"jp", "Japanese Yen"},
    {"ko", "South Korean Won"},
    {"nl", "Euro"},
    {"pt", "Euro"},
    {"ru", "Russian Ruble"},
    {"sk", "Euro"},
    {"zh", "Chinese Yuan Renminbi"}
  };

  void I18N::init()
  {
    _currentLang = _getLanguage(std::locale("").name());

    add("Button", "af", "Knoppie");
    add("Button", "bg", "Бутон");
    add("Button", "cs", "Tlačítko");
    add("Button", "de", "Taste");
    add("Button", "el", "Κουμπί");
    add("Button", "en", "Button");
    add("Button", "es", "Botón");
    add("Button", "eu", "Botoia");
    add("Button", "fi", "Painike");
    add("Button", "fr", "Bouton");
    add("Button", "he", "כפתור");
    add("Button", "hu", "Gomb");
    add("Button", "it", "Pulsante");
    add("Button", "jp", "ボタン");
    add("Button", "ko", "버튼");
    add("Button", "nl", "Knop");
    add("Button", "pt", "Botão");
    add("Button", "ru", "Кнопка");
    add("Button", "sk", "Tlačidlo");
    add("Button", "zh", "按钮");
  }

  void I18N::add(const std::string& key, const std::string& locale, const std::string& text)
  {
    _translations[key][_getLanguage(locale)] = text;
  }

  std::string I18N::t(const std::string& key)
  {
    if (_translations.count(key) && _translations[key].count(_currentLang)) {
      return _translations[key][_currentLang];
    }
    return key;
  }

  std::string I18N::t(const std::string& key, const std::string& locale)
  {
    std::string lang = _getLanguage(locale);
    if (_translations.count(key) && _translations[key].count(lang)) {
      return _translations[key][lang];
    }
    return key;
  }

  std::string I18N::getCurrencySymbol(const std::string& locale)
  {
    std::string lang = _getLanguage(locale);
    auto it = _currencySymbols.find(lang);
    if (it != _currencySymbols.end()) {
      return it->second;
    }
    return "";
  }

  std::string I18N::getCurrencySymbol()
  {
    return getCurrencySymbol(_currentLang);
  }

  std::string I18N::getCurrencyName(const std::string& locale)
  {
    std::string lang = _getLanguage(locale);
    auto it = _currencyNames.find(lang);
    if (it != _currencyNames.end()) {
      return it->second;
    }
    return "";
  }

  std::string I18N::getCurrencyName()
  {
    return getCurrencyName(_currentLang);
  }

  void I18N::setLanguage(const std::string& locale)
  {
    _currentLang = _getLanguage(locale);
  }

  std::string I18N::_getLanguage(std::string locale)
  {
    std::size_t pos = locale.find('_');
    if (pos != std::string::npos) {
      return locale.substr(0, pos);
    }

    pos = locale.find('-');
    if (pos != std::string::npos) {
      return locale.substr(0, pos);
    }

    return locale;
  }

}
