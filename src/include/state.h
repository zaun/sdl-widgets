#ifndef SGI_STATE_H
#define SGI_STATE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <sstream>
#include <functional>
#include <map>
#include <vector>

namespace SGI {

  class State {
  public:
    using ValueType = std::variant<int, double, float, bool, std::string, std::shared_ptr<State>>;
    using StateMap = std::unordered_map<std::string, ValueType>;
    using ListenerCallback = std::function<void(const std::string&, bool)>;
    using ListenerMap = std::map<int, std::pair<ListenerCallback, bool>>;

    int addListener(const std::string& key, ListenerCallback callback, bool deep = false);

    void clear(const std::string& key);
    template<typename T> T get(const std::string& key, T defaultValue = T{}) const;

    void removeListener(int id);
    void set(const std::string& key, ValueType value);

  private:
    StateMap _data;
    std::unordered_map<std::string, ListenerMap> _listeners;
    int _nextListenerId = 1;

    template<typename T, typename U> static T _convertTo(U&& value);
    template<typename T> static T _fromString(const std::string& value);
    template<typename T> static std::string _toString(T value);

    static std::vector<std::string> _splitKey(const std::string& key);

    bool _contains(const std::string& key) const;
    template<typename T> T _convert(const ValueType& value) const;
    void _triggerListeners(const std::string& key, bool isFinal);
  };
}

#endif // SGI_STATE_H
