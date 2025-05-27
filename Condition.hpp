#ifndef __Condition_H__
#define __Condition_H__

#include "Noncopyable.hpp"
#include <pthread.h>
//#include "MutexLock.hpp"  //该头文件可以放到实现文件中去, 不需要放在这里

namespace wd
{

class MutexLock;//类的前向声明， 减少头文件的依赖
class Condition : Noncopyable
{
public:
    Condition(MutexLock & m);
    ~Condition();

    void wait();
    void notify();
    void notifyAll();

private:
    pthread_cond_t _cond;
    MutexLock & _mutex;
};

}//end of namespace wd


#endif

