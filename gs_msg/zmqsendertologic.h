#pragma once

#include "gsmsgptr.h"

namespace ZmqSenderToLogic
{
	void Send(GsMsg::MsgPtr pMsg);
	bool InitInThread();  // Should init in this thread.
	void ResetSocket();  // Optional reset socket explicitly.
}  // namespace ZmqSenderToLogic

