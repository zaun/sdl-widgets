
#include <SDL3/SDL.h>
#include <SDL3_net/SDL_net.h>
#include "debug.h"
#include "wsclient.h"

WSClient::WSClient(const std::string &hostname, int port)
  : _hostname(hostname), _port(port), _address(nullptr), _socket(nullptr), 
    _connected(false), _pollingActive(false) {
  if (!SDLNet_Init()) {
    LOG(WSCLIENT, "SDLNet_Init() failed: %s", SDL_GetError());
    exit(1);
  }

  if (atexit([](){
    SDLNet_Quit();
    LOG(APP, "SDLNet has been shutdown");
  }) != 0) {
    ERROR(APP, "atexit Error: Failed to register SDLNet_Quit");
    SDLNet_Quit();
    exit(1);
  }

  _address = SDLNet_ResolveHostname(hostname.c_str());
}

WSClient::~WSClient() {
  stopPolling();
  disconnect();
  SDLNet_Quit();
}

bool WSClient::connect() {
  if (SDLNet_WaitUntilResolved(_address, -1) == -1) {
    LOG(WSCLIENT, "Failed to resolve hostname: %s", SDL_GetError());
    return false;
  }
  _socket = SDLNet_CreateClient(_address, _port);
  if (!_socket) {
    LOG(WSCLIENT, "Failed to create client socket: %s", SDL_GetError());
    return false;
  }
  if (SDLNet_WaitUntilConnected(_socket, -1) < 0) {
    LOG(WSCLIENT, "Failed to connect: %s", SDL_GetError());
    SDLNet_DestroyStreamSocket(_socket);
    _socket = nullptr;
    return false;
  }
  _connected = true;

  // Handshake example for WebSocket protocol
  std::string handshake = "GET / HTTP/1.1\r\n"
              "Host: " + _hostname + ":" + std::to_string(_port) + "\r\n"
              "Upgrade: websocket\r\n"
              "Connection: Upgrade\r\n"
              "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
              "Sec-WebSocket-Version: 13\r\n\r\n";

  if (SDLNet_WriteToStreamSocket(_socket, handshake.c_str(), handshake.size()) < 0) {
    LOG(WSCLIENT, "Failed to send handshake: %s", SDL_GetError());
    return false;
  }

  char response[512];
  int response_length = SDLNet_ReadFromStreamSocket(_socket, response, sizeof(response) - 1);
  if (response_length <= 0) {
    LOG(WSCLIENT, "Failed to receive handshake response: %s", SDL_GetError());
    return false;
  }

  response[response_length] = '\0';
  LOG(WSCLIENT, "Received handshake response: %s", response);

  return true;
}

void WSClient::disconnect() {
  if (_connected && _socket) {
    SDLNet_DestroyStreamSocket(_socket);
    _socket = nullptr;
    _connected = false;
  }
}

bool WSClient::_sendWebSocketFrame(const std::string &message, bool binary) {
  return _sendWebSocketFrame(reinterpret_cast<const uint8_t *>(message.c_str()), message.size(), binary);
}

bool WSClient::_sendWebSocketFrame(const uint8_t *data, size_t size, bool binary) {
  if (!_connected || !_socket) {
    LOG(WSCLIENT, "Not connected to server");
    return false;
  }

  std::vector<uint8_t> frame;
  frame.push_back(binary ? 0x82 : 0x81); // FIN + opcode (text=0x1, binary=0x2)

  if (size <= 125) {
    frame.push_back(size & 0x7F); // Payload length (7-bit)
  } else if (size <= 65535) {
    frame.push_back(126); // Payload length indicator (16-bit)
    frame.push_back((size >> 8) & 0xFF); // Most significant byte
    frame.push_back(size & 0xFF);    // Least significant byte
  } else {
    frame.push_back(127); // Payload length indicator (64-bit)
    for (int i = 7; i >= 0; --i) {
      frame.push_back((size >> (i * 8)) & 0xFF); // 64-bit payload length
    }
  }

  frame.insert(frame.end(), data, data + size);

  if (SDLNet_WriteToStreamSocket(_socket, frame.data(), frame.size()) < 0) {
    LOG(WSCLIENT, "Failed to send WebSocket frame: %s", SDL_GetError());
    return false;
  }

  return true;
}

bool WSClient::_receiveWebSocketFrame(std::string &message, bool &isBinary) {
  if (!_connected || !_socket) {
    LOG(WSCLIENT, "Not connected to server");
    return false;
  }

  uint8_t header[2];
  if (SDLNet_ReadFromStreamSocket(_socket, header, 2) != 2) {
    LOG(WSCLIENT, "Failed to read WebSocket frame header: %s", SDL_GetError());
    return false;
  }

  uint8_t opcode = header[0] & 0x0F;
  isBinary = (opcode == 0x2); // Check if it's a binary frame

  // Handle Ping
  if (opcode == 0x9) { // Ping frame
    LOG(WSCLIENT, "Received Ping frame");
    _sendPong(std::vector<uint8_t>(header + 1, header + 2)); // Echo the payload back
    return true;
  }

  if (opcode == 0x8) { // Close frame
    LOG(WSCLIENT, "Received close frame");
    disconnect();
    return false;
  }

  size_t payload_length = header[1] & 0x7F;
  if (payload_length == 126) {
    uint8_t extended_payload[2];
    if (SDLNet_ReadFromStreamSocket(_socket, extended_payload, 2) != 2) {
      LOG(WSCLIENT, "Failed to read extended payload length: %s", SDL_GetError());
      return false;
    }
    payload_length = (extended_payload[0] << 8) | extended_payload[1];
  } else if (payload_length == 127) {
    uint8_t extended_payload[8];
    if (SDLNet_ReadFromStreamSocket(_socket, extended_payload, 8) != 8) {
      LOG(WSCLIENT, "Failed to read extended payload length: %s", SDL_GetError());
      return false;
    }
    payload_length = 0;
    for (int i = 0; i < 8; ++i) {
      payload_length = (payload_length << 8) | extended_payload[i];
    }
  }

  std::vector<uint8_t> payload(payload_length);
  if (SDLNet_ReadFromStreamSocket(_socket, payload.data(), payload_length) != payload_length) {
    LOG(WSCLIENT, "Failed to read WebSocket frame payload: %s", SDL_GetError());
    return false;
  }

  message.assign(payload.begin(), payload.end());
  return true;
}

void WSClient::_sendPong(const std::vector<uint8_t> &payload) {
  _sendWebSocketFrame(payload.data(), payload.size(), false); // Send Pong frame
}

bool WSClient::sendMessage(const std::string &cmd, const json &data) {
  if (!_connected || !_socket) {
    LOG(WSCLIENT, "Not connected to server");
    return false;
  }

  json message = {
    {"cmd", cmd},
    {"data", data}
  };

  std::string msg_str = message.dump();
  return _sendWebSocketFrame(msg_str);
}

bool WSClient::sendBinaryMessage(const std::vector<uint8_t> &data) {
  return _sendWebSocketFrame(data.data(), data.size(), true);
}

void WSClient::startPolling() {
  _pollingActive = true;
  _pollingThread = std::thread(&WSClient::_pollingFunction, this);
}

void WSClient::stopPolling() {
  _pollingActive = false;
  if (_pollingThread.joinable()) {
    _pollingThread.join();
  }
}

void WSClient::_pollingFunction() {
  while (_pollingActive) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust polling interval as needed
    std::string message;
    bool isBinary;
    if (_receiveWebSocketFrame(message, isBinary)) {
      std::lock_guard<std::mutex> lock(_queueMutex);
      _messageQueue.emplace(message, isBinary);
      _cv.notify_one();
    }
  }
}

void WSClient::_processMessages() {
  std::unique_lock<std::mutex> lock(_queueMutex);
  _cv.wait(lock, [this] { return !_messageQueue.empty(); });

  while (!_messageQueue.empty()) {
    auto [message, isBinary] = _messageQueue.front();
    _messageQueue.pop();
    lock.unlock();

    if (isBinary) {
      std::vector<uint8_t> binaryData(message.begin(), message.end());
      for (const auto& listener : _binaryListeners) {
        listener(binaryData);
      }
    } else {
      try {
        json jsonResponse = json::parse(message);
        std::string cmd = jsonResponse["cmd"];
        if (_listeners.find(cmd) != _listeners.end()) {
          // Call the listener on the main thread
          LOG(WSCLIENT, "Dispatching command: %s", cmd.c_str());
          LOG(WSCLIENT, "Data: %s", jsonResponse["data"].dump().c_str());
          SDL_AddTimer(0, [](void *userData, SDL_TimerID timerID, Uint32 interval)->Uint32 {
            auto *data = static_cast<std::tuple<std::unordered_map<std::string, Listener>, std::string, json*>*>(userData);

            std::unordered_map<std::string, Listener> listeners = std::get<0>(*data);
            std::string command = std::get<1>(*data);
            json* jsonData = std::get<2>(*data);

            auto it = listeners.find(command);
            if (it != listeners.end()) {
              it->second(*jsonData);
            }

            delete jsonData;
            delete data;
            return 0;
          }, new std::tuple<std::unordered_map<std::string, Listener>, std::string, json*>(_listeners, cmd, new json(jsonResponse["data"])));
        }
      } catch (const std::exception &e) {
        LOG(WSCLIENT, "Failed to parse JSON message: %s", e.what());
      }
    }

    lock.lock();
  }
}

void WSClient::addListener(const std::string &cmd, Listener listener) {
  _listeners[cmd] = listener;
}

void WSClient::removeListener(const std::string &cmd) {
  _listeners.erase(cmd);
}

void WSClient::addBinaryListener(BinaryListener listener) {
  _binaryListeners.push_back(listener); // Add the binary listener to the vector
}

void WSClient::removeBinaryListener(BinaryListener listener) {
  _binaryListeners.erase(std::remove_if(_binaryListeners.begin(), _binaryListeners.end(),
    [&listener](const BinaryListener& l) { return l.target<BinaryListener>() == listener.target<BinaryListener>(); }), _binaryListeners.end());
}
