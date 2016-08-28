#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore (int t_count = 0)
    {
        m_count = t_count;
    };

    inline void notify()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_count++;
        m_cv.notify_one();
    }

    inline void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [&]{ return m_count > 0; });
        m_count--;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    int m_count;
};

#endif /* end of include guard: __SEMAPHORE_H__ */
