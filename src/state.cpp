#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <sstream>
#include <functional>
#include <map>
#include <vector>
#include "state.h"

namespace SGI {

  int State::addListener(const std::string& key, ListenerCallback callback, bool deep) {
    int id = _nextListenerId++;
    _listeners[key].emplace(id, std::make_pair(callback, deep));
    return id;
  }

  void State::clear(const std::string& key) {
    auto keys = _splitKey(key);
    State* current = this;

    for (size_t i = 0; i < keys.size() - 1; ++i) {
      const std::string& subkey = keys[i];
      if (!current->_contains(subkey)) {
        return; // If the key doesn't exist, nothing to clear
      }
      current = std::get<std::shared_ptr<State>>(current->_data[subkey]).get();
    }
    current->_data.erase(keys.back());
    _triggerListeners(key, true);
  }

  template<typename T> T State::get(const std::string& key, T defaultValue) const {
    auto keys = _splitKey(key);
    const State* current = this;

    for (size_t i = 0; i < keys.size() - 1; ++i) {
      const std::string& subkey = keys[i];
      if (!current->_contains(subkey)) {
        return defaultValue;
      }
      current = std::get<std::shared_ptr<State>>(current->_data.at(subkey)).get();
    }

    if (current->_contains(keys.back())) {
      return _convert<T>(current->_data.at(keys.back()));
    } else {
      return defaultValue;
    }
  }

  void State::removeListener(int id) {
    for (auto& [key, listenerMap] : _listeners) {
      listenerMap.erase(id);
      if (listenerMap.empty()) {
        _listeners.erase(key);
      }
    }
  }

  void State::set(const std::string& key, ValueType value) {
    auto keys = _splitKey(key);
    State* current = this;
    
    for (size_t i = 0; i < keys.size() - 1; ++i) {
      const std::string& subkey = keys[i];
      if (!current->_contains(subkey)) {
        current->set(subkey, std::make_shared<State>());
      }
      current = std::get<std::shared_ptr<State>>(current->_data[subkey]).get();
    }
    current->_data[keys.back()] = value;
    _triggerListeners(key, false);
  }

  bool State::_contains(const std::string& key) const {
    return _data.find(key) != _data.end();
  }

  template<typename T> T State::_convert(const ValueType& value) const {
    return std::visit([](auto&& arg) -> T {
      return _convertTo<T>(arg);
    }, value);
  }

  template<typename T, typename U> T State::_convertTo(U&& value) {
    if constexpr (std::is_same_v<T, U>) {
      return value;
    } else if constexpr (std::is_same_v<T, std::string>) {
      return _toString(value);
    } else if constexpr (std::is_same_v<U, std::string> && (std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, float> || std::is_same_v<T, bool>)) {
      return _fromString<T>(value);
    } else {
      throw std::bad_variant_access();
    }
  }

  template<typename T> T _fromString(const std::string& value) {
    if constexpr (std::is_same_v<T, bool>) {
      return (value == "True" || value == "1");
    } else {
      std::istringstream iss(value);
      T result;
      iss >> result;
      if (iss.fail()) {
        throw std::runtime_error("Conversion failed");
      }
      return result;
    }
  }

  template<typename T> std::string _toString(T value) {
    if constexpr (std::is_same_v<T, std::string>) {
      return value;
    } else if constexpr (std::is_same_v<T, bool>) {
      return value ? "True" : "False";
    } else {
      return std::to_string(value);
    }
  }

  std::vector<std::string> State::_splitKey(const std::string& key) {
    std::vector<std::string> keys;
    std::string token;
    std::istringstream tokenStream(key);
    while (std::getline(tokenStream, token, '.')) {
      keys.push_back(token);
    }
    return keys;
  }

  void State::_triggerListeners(const std::string& key, bool isFinal) {
    auto keys = _splitKey(key);
    std::string currentKey;

    for (size_t i = 0; i < keys.size(); ++i) {
      currentKey += (i > 0 ? "." : "") + keys[i];

      if (_listeners.count(currentKey)) {
        for (const auto& [id, listenerPair] : _listeners[currentKey]) {
          const auto& [callback, deep] = listenerPair;
          if (deep || i == keys.size() - 1) {
            callback(key, isFinal);
          }
        }
      }
    }
  }
}
