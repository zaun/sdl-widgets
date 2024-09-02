#ifndef SGI_I18N_H
#define SGI_I18N_H

#include <string>
#include <unordered_map>

namespace SGI {

  class I18N {
    public:
      static void init();

      static void add(const std::string& key, const std::string& locale, const std::string& text);
      static std::string t(const std::string& key);
      static std::string t(const std::string& key, const std::string& locale);

      static std::string getCurrencySymbol(const std::string& locale);
      static std::string getCurrencySymbol();
      static std::string getCurrencyName(const std::string& locale);
      static std::string getCurrencyName();

      static void setLanguage(const std::string& locale);

    private:
      static std::string _currentLang;
      static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _translations;

      static std::unordered_map<std::string, std::string> _currencySymbols;
      static std::unordered_map<std::string, std::string> _currencyNames;

      static std::string _getLanguage(std::string locale);
  };

}

#endif // SGI_I18N_H