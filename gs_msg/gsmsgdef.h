// Gs 内部消息，用于LogicThread 和其它线程,之间。
#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>

#include "th_bufferptr.h"
#include "ksdef.h"  // for UINT64
#include "zmqcltaddress.h"

namespace GsMsg {

enum MsgType
{
	MT_ILLEGAL = 0,

	MT_FWD_LOGINTD_TO_LOGIC,		// LoginThread -> Logic FwdMsg
	MT_FWD_LOGIC_TO_LOGINTD,		// Logic -> LoginThread FwdMsg

	// SecTick,
	MT_CTRL_SEC_TICK,  // SecTickCtrlMsg

	MT_MAX
};

struct Msg
{
	MsgType eType;
};

// Forward msg from client to GS , or vice versa.
struct FwdMsg : Msg
{
	TH_BufferPtr pMsg;  // shared_ptr
};
typedef boost::shared_ptr<FwdMsg> FwdMsgPtr;


// Second tick, MT_CTRL_SEC_TICK, App->Logic(PingGses)->Gs
struct SecTickCtrlMsg : Msg
{
	UINT64 qwNowNs;  // for debug
};
typedef boost::shared_ptr<SecTickCtrlMsg> SecTickCtrlMsgPtr;




}  // namespace GsMsg

