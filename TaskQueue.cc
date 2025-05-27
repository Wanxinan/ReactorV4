#include "TaskQueue.hpp"
#include "MutexLock.hpp"

#include <iostream>

namespace wd
{


TaskQueue::TaskQueue(size_t sz)
: _queSize(sz)
, _mutex()
, _notFull(_mutex)
, _notEmpty(_mutex)
, _flag(true)
{
}

bool TaskQueue::empty() const
{
    return _que.size() == 0; 
}

bool TaskQueue::full() const
{
    return _que.size() == _queSize;
}

//push函数运行在生产者线程
void TaskQueue::push(ElemType e)
{
    //autolock是一个局部对象，当push函数
    //执行结束时，一定会销毁，在销毁时就会解锁
    MutexLockGuard autolock(_mutex);
    //使用while是为了防止异常（虚假）唤醒
    while(full()) {
        _notFull.wait();
    }

    _que.push(e);//往队列中存放数据
    //通知消费者线程取数据
    _notEmpty.notify();
}


//pop函数运行在消费者线程
ElemType TaskQueue::pop()
{
    MutexLockGuard autolock(_mutex);
    while(_flag && empty()) {
        _notEmpty.wait();
    }

    if(_flag) {
        ElemType tmp = _que.front();//从队头取数据
        _que.pop();

        //通知生产者线程放数据
        _notFull.notify();
        return tmp;
    } else {
        return nullptr;
    }
}

void TaskQueue::wakeup()
{
    _flag = false;
    _notEmpty.notifyAll();
}

}//end of namespace wd
