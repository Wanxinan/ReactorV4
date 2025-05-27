#ifndef __Noncopyable_H__
#define __Noncopyable_H__

namespace wd
{
class Noncopyable
{
public:
    Noncopyable(){}
    ~Noncopyable(){}

    //禁止复制
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable & operator=(const Noncopyable &) = delete;

};

}//end of namespace wd



#endif

