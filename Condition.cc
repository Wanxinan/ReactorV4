#include "Condition.hpp"
#include "MutexLock.hpp"  //在实现文件中才使用MutexLock的头文件
#include <iostream>
#include <pthread.h>


namespace wd
{

Condition::Condition(MutexLock & m)
: _mutex(m)
{
    pthread_cond_init(&_cond, nullptr);
}

Condition::~Condition()
{
    pthread_cond_destroy(&_cond);
}

void Condition::wait()
{
    pthread_cond_wait(&_cond, _mutex.getMutexPtr());
}

void Condition::notify()
{
    pthread_cond_signal(&_cond);
}

void Condition::notifyAll()
{
    pthread_cond_broadcast(&_cond);
}
}//end of namespace wd
