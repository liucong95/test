#include "zmqsendertologic.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>
#include "log.h"
#include "gsmsgdef.h"
#include "zmq.hpp"
#include "zmqcontext.h"
#include "zmqinprocaddress.h"
#include "zmqmsg.h"

const char LOG_NAME[] = "ZmqSenderToLogic";

namespace {

typedef boost::thread_specific_ptr<zmq::socket_t> ZmqSocketTsp;

ZmqSocketTsp & GetZmqSocketTsp()
{
	static ZmqSocketTsp s_pSocket;  // thread specific zmq socket
	return s_pSocket;
}

// will throw zmq::error_t
void InitSocket()
{
	ZmqSocketTsp & pSocket = GetZmqSocketTsp();
	BOOST_ASSERT_MSG(NULL == pSocket.get(), "Thread specific socket already inited!");

	pSocket.reset(new zmq::socket_t(ZmqContext::Get(), ZMQ_PUSH));

	int nLingerMs = 0;  // milliseconds
	pSocket->setsockopt(ZMQ_LINGER, &nLingerMs, sizeof(nLingerMs));
	int nSndHwm = 1000 * 1000 * 1000;  // Very large!
	pSocket->setsockopt(ZMQ_SNDHWM, &nSndHwm, sizeof(nSndHwm));
	pSocket->connect(kLogicFrontend);  // throws if frontend not ready.
}

}  // namespace

namespace ZmqSenderToLogic {

void Send(GsMsg::MsgPtr pMsg)
{
	// Each thread has its own socket.
	ZmqSocketTsp & pSocket = GetZmqSocketTsp();
	BOOST_ASSERT(pSocket.get());  // Should init before Send()
	ZmqMsg::SendTo(*pSocket, pMsg);
}

bool InitInThread()
{
	try
	{
		InitSocket();
	}
	catch(const zmq::error_t & err)
	{
		LOG_ERROR(Fmt("Failed to init zmq socket. (%d)%s")
			% err.num() % err.what());
		return false;
	}
	return true;
}

// Thread specific socket pointer will be reset on thread end.
// But you can explicitly reset it before thread end.
void ResetSocket()
{
	ZmqSocketTsp & pSocket = GetZmqSocketTsp();
	pSocket.reset();
}

}  // namespace ZmqSenderToLogic
