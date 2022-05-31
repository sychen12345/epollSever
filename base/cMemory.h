//
// Created by Lenovo on 2022/5/30.
//

#ifndef EPOLLTCP_CMEMORY_H
#define EPOLLTCP_CMEMORY_H


class cMemory
{
private:
    cMemory() {} // 构造函数，因为要做成单例类，所以是私有的构造函数

public:
    ~cMemory(){};

private:
    static cMemory *mInstance;

public:
    static cMemory *getInstance() // 单例
    {
        if (mInstance == nullptr)
        {
            // 锁
            if (mInstance == nullptr)
            {
                mInstance = new cMemory();
                // 第一次调用不应该放在线程中，应该放在主进程中，以免和其他线程调用冲突从而导致同时执行两次new

                static cGarhuishou cl;
            }
            // 放锁
        }
        return mInstance;
    }

    class cGarhuishou
    {
    public:
        ~cGarhuishou()
        {
            if (cMemory::mInstance)
            {
                delete cMemory::mInstance; // 这个释放是整个系统退出的时候，系统来调用释放内存的哦
                cMemory::mInstance = nullptr;
            }
        }
    };
    //-------

public:
    void *allocMemory(int memCount, bool ifmemset);

    void freeMemory(void *point);
};


#endif // EPOLLTCP_CMEMORY_H
