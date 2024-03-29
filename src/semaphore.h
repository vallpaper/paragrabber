#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <mutex>
#include <condition_variable>


// Semaphore class
// counter holds how many threads can pass
class Semaphore {
public:
    Semaphore (int t_count = 0)
    {
        m_count = t_count;
    };

    // Update internal counter and notify one thread
    inline void notify()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_count++;
        m_cv.notify_one();
    }

    // If counter is grater that 0 thread will pass
    // If counter is 0 thread will wait (suspend)
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
