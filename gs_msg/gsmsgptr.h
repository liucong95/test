// Gs 内部消息指针

#pragma once

#include <boost/shared_ptr.hpp>

namespace GsMsg {

struct Msg;
typedef boost::shared_ptr<Msg> MsgPtr;

}  // namespace GsMsg

