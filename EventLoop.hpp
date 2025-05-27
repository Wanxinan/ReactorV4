#ifndef __EventLoop_H__
#define __EventLoop_H__

#include "TcpConnection.hpp"
#include "MutexLock.hpp"
#include <sys/epoll.h>

#include <map>
#include <vector>
using std::vector;
using std::map;

namespace wd
{

using Functor = std::function<void()>;
class Acceptor;
class EventLoop
{
public:
    EventLoop(Acceptor & a);
    ~EventLoop();

    void loop();
    void unloop() { _isLooping = false;}
    void runInLoop(Functor && cb);

    void setAllCallbacks(TcpConnectionCallback && cb1,
                         TcpConnectionCallback && cb2,
                         TcpConnectionCallback && cb3)
    {
        _onConnection = std::move(cb1);
        _onMessage = std::move(cb2);
        _onClose = std::move(cb3);
    }

private:
    int createEpollFd();
    void addEpollReadEvent(int fd);
    void delEpollReadEvent(int fd);

    void waitEpollFd();//循环执行体
    void handleNewConnection();
    void handleMessage(int);

    int createEventFd();
    void handleRead();
    void wakeup();
    void doPendingFunctors();

private:
    int _epfd;
    int _eventfd;
    bool _isLooping;
    Acceptor & _acceptor;
    vector<struct epoll_event> _eventArr;

    map<int, TcpConnectionPtr> _conns;
    vector<Functor> _pendingFunctors;
    MutexLock _mutex;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

}// end of namespace wd


#endif

