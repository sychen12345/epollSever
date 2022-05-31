//
// Created by Lenovo on 2022/5/30.
//

#ifndef EPOLLTCP_CLOCK_H
#define EPOLLTCP_CLOCK_H
#include <pthread.h>

class cLock
{
public:
    cLock(pthread_mutex_t *pMutex)
    {
        m_pMutex = pMutex;
        pthread_mutex_lock(m_pMutex); //加锁互斥量
    }
    ~cLock()
    {
        pthread_mutex_unlock(m_pMutex); //解锁互斥量
    }

private:
    pthread_mutex_t *m_pMutex;
};


#endif // EPOLLTCP_CLOCK_H
