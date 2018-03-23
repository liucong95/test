#include "zmqmsg.h"
#include "log.h"
#include "gsmsgdef.h"

const char LOG_NAME[] = "ZmqMsg";

namespace ZmqMsg {

using namespace GsMsg;

void SendTo(zmq::socket_t & rSocket, MsgPtr pMsg)
{
	BOOST_ASSERT(pMsg);
	BOOST_ASSERT(pMsg->eType > MT_ILLEGAL);
	BOOST_ASSERT(pMsg->eType < MT_MAX);

	// We can not send a object directly, nor can we send a void*.
	// Must send a pointer of fixed type, so the receiver can delete correctly.
	MsgPtr * pPtr = new MsgPtr;  // pPtr will be deleted in RecvFrom().
	*pPtr = pMsg;
	try
	{
		const size_t kSize = sizeof(pPtr);
		size_t nSend = rSocket.send(&pPtr, kSize, ZMQ_DONTWAIT);
		if (nSend != kSize)
		{
			delete pPtr;

			// nSend is 0 if queue is full.
			if (0 == nSend)
				LOG_WARN("Msg lost.");
			else 
				LOG_FATAL(Fmt("Send msg %1% bytes, but only sent %2%.") % kSize % nSend);
		}
		return;  // pPtr will be deleted in RecvFrom().
	}
	catch (const zmq::error_t & err)
	{
		LOG_WARN(Fmt("send() failed. (%d)%s")
			% err.num() % err.what());
	}
	delete pPtr;  // delete on error return;
}

// return NULL if no msg.
MsgPtr RecvFrom(zmq::socket_t & rSocket)
{
	// The received msg is a pointer.
	const size_t kBufLen = 64;
	BOOST_STATIC_ASSERT_MSG(sizeof(MsgPtr *) < kBufLen, "kBufLen must be large enough.");
	char aRecvBuf[kBufLen];

	try
	{
		size_t nRecv = rSocket.recv(aRecvBuf, kBufLen, ZMQ_DONTWAIT);
		if (0 == nRecv)
			return MsgPtr();
		BOOST_ASSERT(sizeof(MsgPtr *) == nRecv);
	}
	catch (const zmq::error_t & err)
	{
		LOG_DEBUG(Fmt("recv() failed. (%d)%s") % err.num() % err.what());
		return MsgPtr();
	}

	MsgPtr * pPtr = *reinterpret_cast<MsgPtr**>(aRecvBuf);
	MsgPtr pMsg = *pPtr;
	delete pPtr;
	BOOST_ASSERT(pMsg);
	return pMsg;
}

}  // namespace ZmqMsg
