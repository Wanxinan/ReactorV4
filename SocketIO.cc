#include "SocketIO.hpp"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace wd
{

SocketIO::SocketIO(int fd)
: _fd(fd)
{}

//确定读取len个字节的数据
int SocketIO::readn(char * buff, int len)
{
	int left = len;
	char * p = buff;
    //提问：left的值有可能小于0吗？不可能小于0，退出循环时，left等于0
	while(left > 0) {
		int ret = ::recv(_fd, p, left, 0);
		if(ret == -1 && errno == EINTR)
			continue;
		else if(ret == -1) {
			perror("read");
			return len - left;
		}
		else if(ret == 0) {//连接断开
			return len - left;
		} else {
			left -= ret;
			p += ret;
		}
	}
	return len - left;
}

//每一次读取一行数据, 每一行的长度不好界定，是以'\n'作为结尾
//char buff[1024] = {0}; // 下标为1023的值存放'\0'
int SocketIO::readline(char * buff, int maxlen)
{
	int left = maxlen - 1;
	char * p = buff; 
	int ret;
	int total = 0;
	while(left > 0) {
		ret = recvPeek(p, left);		
		//查找 '\n'
		for(int idx = 0; idx != ret; ++idx) {
			if(p[idx] == '\n') {
				int sz = idx + 1;
				readn(p, sz);//从内核缓冲区中移走
				total += sz;
				p += sz;
				*p = '\0';
				return total;
			}
		}
		//如果没有发现 '\n'
		readn(p, ret);
		left -= ret;
		p += ret;
		total += ret;
	}
	*p = '\0';// 最终没有发现'\n'
	return total;
}

int SocketIO::recvPeek(char * buff, int len) const
{
	int ret;
	do {
		ret = ::recv(_fd, buff, len, MSG_PEEK);
	} while(ret == -1 && errno == EINTR);
	return ret;
}

	
int SocketIO::writen(const char * buff, int len)
{
	int left = len;
	const char * p = buff;
	while(left > 0) {
		int ret = ::send(_fd, p, left, 0);
		if(ret == -1 && errno == EINTR)
			continue;
		else if(ret == -1) {
			perror("write");
			return len - left;
		} else {
			left -= ret;
			p += ret;
		}
	}
	return len - left;
}

}//end of namespace wd
