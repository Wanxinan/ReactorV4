
#include "Thread.hpp" // 1. 放的自定义头文件

#include <stdio.h>    // 2. C的头文件
#include <string.h>

//#include <iostream> // 3. C++头文件

//#include <log4cpp/Category.hh>  //4. 第三方库头文件


namespace wd
{

Thread::Thread(ThreadCallback && cb)
: _pthid(0)
, _isRunning(false)
, _cb(std::move(cb)) //cb虽然是一个右值引用，但在这里是一个左值
{                    //为了表达移动语义，需要强制转换为一个右值
                     //因此使用了std::move函数
}

void Thread::start()
{
    //第四个参数直接传递Thread对象本身过去，交给线程的入口函数的参数
    int ret = pthread_create(&_pthid, nullptr, start_routine, this);
    if(ret !=0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(ret));
        return;
    }
    _isRunning = true;
}

//参数arg是通过pthread_create的第四个参数传递过来的
void * Thread::start_routine(void* arg)
{
    //子线程要运行一个任务
    //而纯虚函数run方法是我们抽象出来的任务
    //所以这里需要调用run方法, 需要一个Thread的指针
    //因此需要通过线程的参数传递过来
    Thread * pthread = static_cast<Thread*>(arg);
    if(pthread)
        pthread->_cb();

    return nullptr;
}

void Thread::join()
{
    if(_isRunning) {
        pthread_join(_pthid, nullptr);
        _isRunning = false;
    }
}


}//end of namespace wd

