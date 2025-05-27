#include "Threadpool.hpp"

#include <unistd.h>
#include <iostream>

using std::cout;
using std::endl;

namespace wd
{

Threadpool::Threadpool(int n, int sz)
: _threadNum(n)
, _queSize(sz) 
, _isExit(false) 
, _threads()  
, _taskque(_queSize) 
{
    //提前开辟空间
    _threads.reserve(_threadNum);
}

void Threadpool::start()
{
    for(int i = 0; i < _threadNum; ++i) {
        //将线程池中的doTask方法打包成一个函数对象，注册给子线程对象
        //这里使用了std::bind的用法
        unique_ptr<Thread> up(new Thread(std::bind(&Threadpool::doTask, this)));
        _threads.push_back(std::move(up));
    }

    for(auto & thread: _threads) {
        thread->start();
    }
}


//充当的是生产者的角色
void Threadpool::addTask(Task && task)
{
    if(task)
        _taskque.push(std::move(task));
}


//每一个子线程都要做的事儿
void Threadpool::doTask()
{
    while(!_isExit) {
        //从任务队列中获取任务
        //using Task=function<void()>;
        Task  ftask = _taskque.pop();//当任务队列中没有任务时，子线程都在这里阻塞
        if(ftask) {
            ftask();//执行任务
        }
    }
}

//停止线程池的执行
void Threadpool::stop()
{
    //当所有的任务都执行完毕之后，再退出
    while(!_taskque.empty()) {
        //sleep(1);//如果任务队列中还有任务，需要等一等
        usleep(100);
    }


    //要让每一个子线程都正常回收
    if(!_isExit) {
        _isExit = true;

        //加上wakeup之后，防止因为任务的执行速度过快，在stop函数还没有
        //来得及将_isExit设置为true时，每一个子线程就阻塞在pop方法之上了
        //
        //调用了wakeup之后，会唤醒所有的等待的子线程
        _taskque.wakeup();

        for(auto & thread: _threads) {
            thread->join();
        }
    } 
}

}//end of namespace wd
