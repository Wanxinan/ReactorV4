#ifndef __Thread_H__
#define __Thread_H__

#include "Noncopyable.hpp"


#include <pthread.h>

#include <functional>

namespace wd
{

//std::function是函数的容器
using ThreadCallback = std::function<void()>;

class Thread : Noncopyable
{
public:
    Thread(ThreadCallback && cb);
    void start();
    void join();

private:
    //该函数必须要设置为静态的， 因为非静态成员函数都有一个隐含的this指针
    //这样就不满足pthread_create函数对于线程入口函数的要求了
    //因此要将该函数设置为static的，静态成员函数没有this指针
    static void * start_routine(void*);//子线程的入口函数


private:
    pthread_t _pthid;
    bool _isRunning;
    ThreadCallback _cb;//函数对象保存要执行的函数
};


}//end of namespace wd


#endif

