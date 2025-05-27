#ifndef __WD_TCPCONNECTION_H__
#define __WD_TCPCONNECTION_H__

#include "Noncopyable.hpp"
#include "Socket.hpp"
#include "InetAddress.hpp"
#include "SocketIO.hpp"

#include <memory>
#include <string>
#include <functional>
using std::string;
using std::shared_ptr;

namespace wd
{

class EventLoop;

class TcpConnection;
using TcpConnectionPtr=shared_ptr<TcpConnection>;
using TcpConnectionCallback=std::function<void(TcpConnectionPtr)>;

class TcpConnection
: Noncopyable
, public std::enable_shared_from_this<TcpConnection> //继承子该辅助类之后，就拥有了shared_from_this函数
{
public:
	TcpConnection(int fd, EventLoop * p);
	~TcpConnection();

	string receive();
	void send(const string & msg);
    void sendInLoop(const string & msg);

	string toString() const;//获取五元组信息
	void shutdown();

    void setAllCallbacks(const TcpConnectionCallback & cb1,
                         const TcpConnectionCallback & cb2,
                         const TcpConnectionCallback & cb3)
    {
        _onConnection = cb1;
        _onMessage = cb2;
        _onClose = cb3;
    }
#if 0
    //在TcpConnection的函数对象注册过程中，其参数不能使用移动语义
    //如果使用，会造成的问题是：只有第一个建立好的连接会拥有3个函数对象
    //其他的连接都无法正常注册函数对象，因为EventLoop对象中的3个函数对象
    //已经转交给了第一个TcpConnection对象，转移之后，EventLoop对象中的3个
    //函数对象就是空的了
    void setAllCallbacks(TcpConnectionCallback && cb1,
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {
        _onConnection = std::move(cb1);
        _onMessage = std::move(cb2);
        _onClose = std::move(cb3);
    }
#endif

    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();
    bool isClosed() const;

private:
	InetAddress getLocalAddr(int fd);
	InetAddress getPeerAddr(int fd);
private:
	Socket _sock;
	SocketIO _socketIo;
	InetAddress _localAddr;
	InetAddress _peerAddr;
	bool _isShutdwonWrite;//是否要主动关闭连接
    EventLoop * _ploop;
                          //
    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;

};

}//end of namespace wd

#endif
