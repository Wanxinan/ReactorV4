#include "Acceptor.hpp"
#include <sys/types.h>
#include <sys/socket.h>

namespace wd
{

Acceptor::Acceptor(unsigned short port, const string & ip)
: _addr(ip, port)
, _listensock()
{}

void Acceptor::ready()
{
	setReuseAddr(true);
	setReusePort(true);
	bind();
	listen();
}

void Acceptor::setReuseAddr(bool on)
{
	int one = on;
	if(setsockopt(_listensock.fd(), 
		SOL_SOCKET, 
		SO_REUSEADDR, 
		&one, 
		sizeof(one)) < 0) {
		perror("setsockopt");	
	}
}

void Acceptor::setReusePort(bool on)
{
	int one = on;
	if(setsockopt(_listensock.fd(), 
		SOL_SOCKET, 
		SO_REUSEPORT, 
		&one, 
		sizeof(one)) < 0) {
		perror("setsockopt");	
	}
}

void Acceptor::bind()
{
    //:: 必须加上，否则会报错
    //表示要调用的是Linux的bind函数
	int ret = ::bind(_listensock.fd(), 
		(struct sockaddr *)_addr.getInetAddressPtr(),
		sizeof(struct sockaddr));
	if(-1 == ret) {
		perror("bind");
	}
}

void Acceptor::listen()
{
	int ret = ::listen(_listensock.fd(), 128);
	if(-1 == ret) {
		perror("listen");
	}
}

int Acceptor::accept()
{
	int peerfd = ::accept(_listensock.fd(), NULL, NULL);
	if(peerfd == -1) {
		perror("accept");
	}
	return peerfd;
}

}//end of namespace wd
