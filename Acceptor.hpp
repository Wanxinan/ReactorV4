#ifndef __WD_ACCEPTOR_H__
#define __WD_ACCEPTOR_H__

#include "InetAddress.hpp"
#include "Socket.hpp"

namespace wd
{

class Acceptor
{
public:
	Acceptor(unsigned short port, const string & ip = "0.0.0.0");
	void ready();

    int fd() const {    return _listensock.fd();    }

	int accept();
private:
	void setReuseAddr(bool on);
	void setReusePort(bool on);
	void bind();
	void listen();
private:
	InetAddress _addr;
	Socket _listensock;
};

}//end of namespace wd

#endif
