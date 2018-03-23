#pragma once

#include "gsmsgptr.h"
#include "zmq.hpp"

namespace ZmqMsg {

void SendTo(zmq::socket_t & rSocket, GsMsg::MsgPtr pMsg);
GsMsg::MsgPtr RecvFrom(zmq::socket_t & rSocket);

}  // namespace ZmqMsg
