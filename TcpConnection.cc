#include "TcpConnection.hpp"
#include "InetAddress.hpp"
#include "EventLoop.hpp"

#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <sstream>

namespace wd
{
TcpConnection::TcpConnection(int fd, EventLoop * p)
: _sock(fd)
, _socketIo(fd)
, _localAddr(getLocalAddr(fd))
, _peerAddr(getPeerAddr(fd))
, _isShutdwonWrite(false)
, _ploop(p)
{
}

TcpConnection::~TcpConnection()
{
	if(!_isShutdwonWrite) {
		shutdown();
	}
}

string TcpConnection::receive()
{
	char buff[65536] = {0};
    //这里使用的是readline,所以消息的边界为'\n'
	_socketIo.readline(buff, sizeof(buff));
	return string(buff);
}
	
//send函数的执行是在IO线程中进行的（Epoll的事件循环中调用）
void TcpConnection::send(const string & msg)
{
	_socketIo.writen(msg.c_str(), msg.size());
}

//sendInLoop函数的执行是在计算线程中进行的
void TcpConnection::sendInLoop(const string & msg)
{
    if(_ploop) {
        _ploop->runInLoop(std::bind(&TcpConnection::send, this, msg));
    }
}

void TcpConnection::shutdown()
{
	if(!_isShutdwonWrite) {
		_isShutdwonWrite = true;
		_sock.shutdownWrite();
	}
}

void TcpConnection::handleConnectionCallback()
{
    if(_onConnection) {
        //this -> shared_ptr
        //_onConnection(shared_ptr<this>());
        _onConnection(shared_from_this());
    }
}

void TcpConnection::handleMessageCallback()
{
    if(_onMessage) {
        _onMessage(shared_from_this());
    }
}

void TcpConnection::handleCloseCallback()
{
    if(_onClose) {
        _onClose(shared_from_this());
    }
}

string TcpConnection::toString() const
{
	std::ostringstream oss;
	oss << "tcp " << _localAddr.ip() << ":" << _localAddr.port() << " --> "
		<< _peerAddr.ip() << ":" << _peerAddr.port();
	return oss.str();
}


bool TcpConnection::isClosed() const
{
    char buff[20] = {0};
    return _socketIo.recvPeek(buff, sizeof(buff)) == 0;
}


InetAddress TcpConnection::getLocalAddr(int fd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr);
	if(getsockname(_sock.fd(), (struct sockaddr*)&addr, &len) == -1) {
		perror("getsockname");
	}
	return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr(int fd)
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(struct sockaddr);
	if(getpeername(_sock.fd(), (struct sockaddr*)&addr, &len) == -1) {
		perror("getpeername");
	}
	return InetAddress(addr);
}

}//end of namespace wd
