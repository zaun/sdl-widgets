#ifndef SGI_WSCLIENT_H
#define SGI_WSCLIENT_H

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class WSClient {
public:
  using Listener = std::function<void(const json&)>;
  using BinaryListener = std::function<void(const std::vector<uint8_t>&)>;

  WSClient(const std::string &hostname, int port);
  ~WSClient();

  bool connect();
  void disconnect();
  bool sendMessage(const std::string &cmd, const json &data);
  bool sendBinaryMessage(const std::vector<uint8_t> &data);
  void startPolling();
  void stopPolling();
  void addListener(const std::string &cmd, Listener listener);
  void removeListener(const std::string &cmd);
  void addBinaryListener(BinaryListener listener);
  void removeBinaryListener(BinaryListener listener);

private:
  std::string _hostname;
  int _port;
  SDLNet_Address *_address;
  SDLNet_StreamSocket *_socket;
  bool _connected;
  std::thread _pollingThread;
  std::mutex _queueMutex;
  std::condition_variable _cv;
  std::queue<std::pair<std::string, bool>> _messageQueue;
  std::unordered_map<std::string, Listener> _listeners;
  std::vector<BinaryListener> _binaryListeners;
  bool _pollingActive;

  bool _sendWebSocketFrame(const std::string &message, bool binary = false);
  bool _sendWebSocketFrame(const uint8_t *data, size_t size, bool binary = false);
  bool _receiveWebSocketFrame(std::string &message, bool &isBinary);
  void _handlePing(const std::vector<uint8_t> &payload);
  void _sendPong(const std::vector<uint8_t> &payload);
  void _pollingFunction();
  void _processMessages();
};

#endif // SGI_WSCLIENT_H
