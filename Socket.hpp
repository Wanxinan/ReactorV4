#ifndef __WD_SOCKET_H__
#define __WD_SOCKET_H__

#include "Noncopyable.hpp"

namespace wd
{

class Socket
: Noncopyable  //该类创建的对象不可以进行复制，表达对象语义
{
public:
	Socket();
	explicit
	Socket(int fd);

    //在类内部实现的函数都是inline函数
	int fd() const { return _fd;   }

	void shutdownWrite();//主动断开连接，关闭写端

	~Socket();

private:
	int _fd;
};

}//end of namespace wd


#endif
