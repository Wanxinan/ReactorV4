#include "EventLoop.hpp"
#include "Acceptor.hpp"
#include "TcpConnection.hpp"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>



namespace wd
{

EventLoop::EventLoop(Acceptor & a)
: _epfd(createEpollFd())
, _eventfd(createEventFd())
, _acceptor(a)
, _isLooping(false)
, _eventArr(100)
{
    addEpollReadEvent(_acceptor.fd());
    addEpollReadEvent(_eventfd);
}

EventLoop::~EventLoop()
{
    close(_epfd);
    close(_eventfd);
}


void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping) {
        waitEpollFd();
    }
}

//runInLoop方法是在计算线程中调用
void EventLoop::runInLoop(Functor && cb)
{
    _mutex.lock();
    //将函数对象填充到vector数据结构中
    _pendingFunctors.push_back(cb);
    _mutex.unlock();

    wakeup();//通知IO线程
}

void EventLoop::waitEpollFd()
{
    int nready = epoll_wait(_epfd, _eventArr.data(), _eventArr.size(), -1);
    if(nready == -1 && errno == EINTR) {
        //是软中断的错误
        return;
    } else if(nready == -1) {
        perror("epoll_wait");
        return ;
    } else if(nready == 0) {
        printf("epoll timeout.\n");
    } else {
        //nready > 0
        for(int i = 0; i < nready; ++i) {
            int fd = _eventArr[i].data.fd;
            if(fd == _acceptor.fd()) {
                //新连接的处理
                handleNewConnection();
            } else if(fd == _eventfd)  {
                //对_eventfd进行处理
                handleRead();
                doPendingFunctors();//进行消息的发送
            } else {
                //已经建立好的连接
                handleMessage(fd);
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int connfd = _acceptor.accept();
    //connfd添加到epoll的红黑树
    addEpollReadEvent(connfd);

    TcpConnectionPtr sp(new TcpConnection(connfd, this));
    _conns.insert(std::make_pair(connfd, sp));
    
    sp->setAllCallbacks(_onConnection, 
                        _onMessage, 
                        _onClose);

    //当连接建立的时候，要调用函数对象
    sp->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    //消息到达时
    //先通过fd 找到TcpConnection对象
    auto iter = _conns.find(fd);
    if(iter != _conns.end()) {
        //先判断连接是否断开
        ////iter->second就是TcpConnectionPtr
        bool isClosed = iter->second->isClosed();
        if(isClosed) {
            //连接断开
            iter->second->handleCloseCallback();
            //从红黑树上要删除掉
            delEpollReadEvent(fd);
            _conns.erase(fd);//从map删除掉记录
        } else {
            //消息处理
            ////如果该操作的时间过长，就会影响实时性
            //这里不宜处理时间过长的服务
            iter->second->handleMessageCallback();
        }
    }
}

void EventLoop::doPendingFunctors()
{
    printf("doPendingFunctors()\n");
    //_pendingFunctors是临界资源，应该尽快将其释放，交给计算线程
    //来进行填充
    vector<Functor> tmp;//
    _mutex.lock();
    tmp.swap(_pendingFunctors);//时间复杂度为O(1)
    _mutex.unlock();

    //执行消息的发送, 此时IO线程不会再耽误_pendingFunctors的使用
    for(auto & f : tmp) {
        f();
    }
}

int EventLoop::createEpollFd()
{
    int fd = epoll_create1(0);
    if(fd < 0) {
        perror("epoll_create1");
    }
    return fd;
}



void EventLoop::addEpollReadEvent(int fd)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;//监听读事件
    event.data.fd = fd;
    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret < 0) {
        perror("epoll_ctl");
    }
}

void EventLoop::delEpollReadEvent(int fd)
{
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &event);
    if(ret < 0) {
        perror("epoll_ctl");
    }
}


void EventLoop::handleRead()
{
    uint64_t howmany = 0;
    int ret = read(_eventfd, &howmany, sizeof(howmany));
    if(ret != sizeof(howmany)) {
        perror("read");
    }
}

void EventLoop::wakeup()//用来做通知操作
{
    //只要往内核计数器上加1，就能改变其值，从而达到通知的效果
    uint64_t one = 1;
    int ret = write(_eventfd, &one, sizeof(one));
    if(ret != sizeof(one)) {
        perror("write");
    }
}
int EventLoop::createEventFd()
{
    int fd = eventfd(0, 0);
    if(fd < 0) {
        perror("eventfd");
    }
    return fd;
}

}//end of namespace wd
