#ifndef __Threadpool_H__
#define __Threadpool_H__

#include "Thread.hpp"
#include "TaskQueue.hpp"
#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

namespace wd
{

class Threadpool
{
    friend class WorkerThread;
public:
    Threadpool(int n, int sz);
    void start();
    void stop();
    void addTask(Task && task);
private:
    void doTask();

private:
    int _threadNum;
    int _queSize;
    bool _isExit;
    vector<unique_ptr<Thread>> _threads;
    TaskQueue _taskque;
};

}//end of namespace wd


#endif

