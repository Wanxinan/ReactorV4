#include "Socket.hpp"

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>


namespace wd
{

Socket::Socket()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_fd == -1) {
		perror("socket");
	}
}

Socket::Socket(int fd)
: _fd(fd)
{}

//int Socket::fd() {	return _fd;	}

void Socket::shutdownWrite()
{
	::shutdown(_fd, SHUT_WR);	
}

Socket::~Socket()
{
	::close(_fd);
}


}//end of namespace wd
