
#ifndef __SWATCH_SERVER_SERVER_HPP__
#define __SWATCH_SERVER_SERVER_HPP__


#include <string>
#include <vector>

#include <msgpack.hpp>
#include <zmq.hpp>


namespace swatch {

namespace action {
class Board;
}

namespace server {

class Server {
public:
  Server(action::Board& aBoard, uint16_t aPort);
  ~Server() {}

  void start();

private:

  msgpack::sbuffer handleRequest(const msgpack::object& aRequest);

  // Callbacks for specific requests
  static msgpack::sbuffer executeRunCommand(const std::vector<msgpack::object>& aRequest, action::Board& aBoard);
  static msgpack::sbuffer executeEngageFSM(const std::vector<msgpack::object>& aRequest, action::Board& aBoard);
  static msgpack::sbuffer executeResetFSM(const std::vector<msgpack::object>& aRequest, action::Board& aBoard);
  static msgpack::sbuffer executeDisengageFSM(const std::vector<msgpack::object>& aRequest, action::Board& aBoard);
  static msgpack::sbuffer executeRunTransition(const std::vector<msgpack::object>& aRequest, action::Board& aBoard);

  action::Board& mBoard;
  zmq::context_t mContext;
  zmq::socket_t mSocket;

  typedef std::function<msgpack::sbuffer (const std::vector<msgpack::object>&, action::Board&)> Callback;
  static const std::map<std::string, Callback> kCallbacks;
};

} // namespace server
} // namespace swatch

#endif
