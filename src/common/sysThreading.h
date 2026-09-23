#ifndef COMMON_SYSTHREADING_H
#define COMMON_SYSTHREADING_H
//---------------------------------------------------------------------------------

// To force using of PThreads in Win32 uncomment the following line
// #define USE_PTHREADS

//---------------------------------------------------------------------------------

#ifdef WIN32
#ifndef USE_PTHREADS
#define USE_WIN32_THREADS
#endif
#endif

#ifdef _LINUX
#define USE_PTHREADS
#endif

//---------------------------------------------------------------------------------

#ifdef WIN32
#include <common/clean_windows.h>
#endif

#ifdef USE_PTHREADS
#include <pthread.h>
#endif

#include <atomic>
#include <string>
#include <vector>

std::string getSysError();

#if defined(TSAN_ENABLED) && (defined(__GNUC__) || defined(__clang__))
// Disable thread sanitizer warnings for a specific block of code
#define DISABLE_TSAN __attribute__((no_sanitize("thread")))
#else
#define DISABLE_TSAN
#endif

//=== Mutex =======================================================================

/*
Usage:

static cMutex mutex();

void AMethod()
{
    mutex.lock();
    .. do something critical ..
    mutex.unlock();
}
*/

// Mutex class for mutual exclusion of threads in critical sections.
class cMutex {
   public:
    // Initialize the mutex. recursive = true allows nested lock() / unlock() calls.
    cMutex(bool recursive = false);
    cMutex(const cMutex&)            = delete;
    cMutex& operator=(const cMutex&) = delete;

    ~cMutex();

    // Lock the mutex. A thread that already owns the mutex can lock it again without blocking
    // if the mutex was created recursive. If it was created non-recursive, it cannot be locked
    // twice by the same thread.
    void lock();

    // Unlock the mutex. A thread must call unlock() once for each call to lock() (and successful tryLock()).
    void unlock();

   private:
#ifdef USE_WIN32_THREADS
    CRITICAL_SECTION Mutex;
    bool m_isRecursive;
    std::atomic<int> m_lockCount;
    friend class cCondition;
#endif
#ifdef USE_PTHREADS
    pthread_mutex_t m_mutex;
    friend class cCondition;
#endif
};

//=== Threads =====================================================================

/*
Usage:

class MyThread : public NonWaitableThread
{
public:
    MyThread(int* dataArray)
    : NonWaitableThread()
    {
        m_dataArray = dataArray;
    }

    ~MyThread()
    {
        delete m_dataArray;
    }

protected:
    // .. additional member fields
    int* m_dataArray;

    virtual void run()
    {
        // .. the thread code ..
        int* p = m_dataArray;
        while (*p != 0) {
            doSomething(*p);
            sleep(1000);
            p++;
        }
    }
};
..
..
{
    data = new ...;
    MyThread* myThread = new MyThread(data);
    myThread->start();
    ...
    // DO NOT call delete myThread;
    // This will be called automatically when the
    // thread has finished.
}

*/

// This kind of thread is 'non-waitable' which means the thread runs until
// completion and then releases all its resources and calls the destructor
// of this object automatically. It is not possible for another thread to
// wait for the completion of this thread.
class NonWaitableThread {
   public:
    // Create a thread object. The thread is not started automatically.
    // You call the start() method to actually start the thread's execution.
    NonWaitableThread();

    NonWaitableThread(const NonWaitableThread&)            = delete;
    NonWaitableThread& operator=(const NonWaitableThread&) = delete;

    // *** BE CAREFUL not to call the destructor for non-waitable threads! The destructor will be called
    // automatically when the non-waitable thread therminates. Do not call delete yourself, and do not
    // define waitable non-waitable thread objects as stack variables or static variables!
    // The only exception when you can call delete is when the non-waitable thread was created but not
    // started with start().
    virtual ~NonWaitableThread() {};

    // Create the internal thread handle and start the thread.
    virtual void start();

    // Suspend execution for the given number of milliseconds.
    static void sleep(unsigned long milliSeconds);

    // Return the thread ID of the of the calling thread.
    static unsigned int getCurrentThreadId();

   protected:
    // Return the thread ID.
    unsigned int getThreadId();

    // Implement this method in subclasses to define the thread's activity.
    // The thread terminates its execution by returning from this method.
    // The run() method can access all member fields.
    virtual void run() = 0;

   protected:
#ifdef USE_WIN32_THREADS
    HANDLE m_threadHandle;
    unsigned int m_threadId;
    void DeallocateResources();
#endif
#ifdef USE_PTHREADS
    pthread_t m_thread;
#endif

    enum Priority {
#ifdef USE_WIN32_THREADS
        priorityIdle         = THREAD_PRIORITY_IDLE,           // -15
        priorityLowest       = THREAD_PRIORITY_LOWEST,         // -2
        priorityBelowNormal  = THREAD_PRIORITY_BELOW_NORMAL,   // -1
        priorityNormal       = THREAD_PRIORITY_NORMAL,         // 0
        priorityAboveNormal  = THREAD_PRIORITY_ABOVE_NORMAL,   // +1
        priorityHighest      = THREAD_PRIORITY_HIGHEST,        // +2
        priorityTimeCritical = THREAD_PRIORITY_TIME_CRITICAL,  // +15
#else
        priorityIdle,
        priorityLowest,
        priorityBelowNormal,
        priorityNormal,
        priorityAboveNormal,
        priorityHighest,
        priorityTimeCritical,
#endif
    };

    // Sets the priority value for the current thread
    bool setPriority(Priority priority);

   private:
#ifdef USE_WIN32_THREADS
    static unsigned int __stdcall StartNonWaitableThread(NonWaitableThread* thread);
#endif
#ifdef USE_PTHREADS
    static void* StartNonWaitableThread(NonWaitableThread* thread);
#endif
};

/*
Usage:

class MyThread : public WaitableThread
{
public:
    MyThread(int* dataArray)
    : WaitableThread()
    {
        m_dataArray = dataArray;
    }

protected:
    // .. additional member fields
    int* m_dataArray;

    virtual void run()
    {
        // .. the thread code ..
        int* p = m_dataArray;
        while (*p != 0) {
            doSomething(*p);
            sleep(1000);
            p++;
        }
    }
};
...
...
{
    data = new ...;
    ...
    MyThread* myThread = new MyThread(data);
    myThread->start();
    ...
    myThread->wait();
    delete myThread;
    ...
    delete data;
}

*/

// This kind of thread is 'waitable' which means that there is the intention
// of having another thread to wait for the completion if this thread. The
// other thread calls wait() and is blocked until this thread finishes. Then
// the thread's resources are released, but this wrapper object is not deleted
// automatically.
class WaitableThread : public NonWaitableThread {
   public:
    // Create a thread object. The thread is not started automatically.
    // You call the start() method to actually start the thread's execution.
    WaitableThread();

    WaitableThread(const WaitableThread&)            = delete;
    WaitableThread& operator=(const WaitableThread&) = delete;

    // Do not to call the destructor before the thread has terminated and
    // is joined by calling the wait() function from another thread.
    // *** WAITABLE THREADS MUST be joined by another thread with the wait() function!
    virtual ~WaitableThread() {};

    // Create the internal thread handle and start the thread.
    virtual void start();

    // Another thread can call this method to wait for the end of the thread.
    void wait();

    // false before start() is called, true() as long as the thread is running,
    // false when the thread is terminated. USE ONLY for waitable threads and
    // do not call after wait().
    bool running() { return m_running; }
    bool stopped() { return m_stopped; }

    // Return the thread ID.
    unsigned int getThreadId() { return NonWaitableThread::getThreadId(); }

   private:
    std::atomic<bool> m_running;
    std::atomic<bool> m_stopped;
    std::atomic<bool> m_pthreadJoined;

   private:
#ifdef USE_WIN32_THREADS
    static unsigned int __stdcall StartWaitableThread(WaitableThread* thread);
#endif
#ifdef USE_PTHREADS
    static void* StartWaitableThread(WaitableThread* thread);
#endif
};

//=== Conditions ==================================================================

/*
Usage example:


static bool count = 0;
cMutex mutex;
cCondition cond;

Thread A:
{
    while(true) {
        mutex.lock();
        while (count == 0)
            cond.wait();
        count--;
        mutex.unlock();
    }
}

Thread B:
{
    while(true) {
        mutex.lock();
        count++;
        cond.signal();
        mutex.unlock();
        cTrhead::sleep(100);
    }
}
*/

class cCondition {
   public:
    cCondition();

    cCondition(const cCondition&)            = delete;
    cCondition& operator=(const cCondition&) = delete;

    ~cCondition();

    // This function waits for the condition to be notified. When called, it atomically
    // (1) releases the associated externalMutex (which the caller must hold
    // while evaluating the condition expression) and
    // (2) goes to sleep awaiting a subsequent notification from another thread (via
    // the signal() or broadcast() methods).
    // The externalMutex will be locked when wait() returns.
    void wait(cMutex& externalMutex);

    // This method is a time-based variant of wait(). It waits up to milliSeconds amount
    // of time for the condition to be notified.
    // Return value:
    //   true  - The condition was notified within the given time.
    //   false - The time elapsed before the condition was notified (timeout).
    // Even in the case of timeouts, the externalMutex will be locked when
    // timedwait() returns.
    bool timedwait(cMutex& externalMutex, unsigned long milliSeconds);

    // This method notifies one thread waiting on the condition.
    void signal();

    // This method notifies ALL threads that are currently waiting on the condition.
    void broadcast();

   public:  // Backward compatibility for existing TPCX driver code
    bool wait(cMutex& externalMutex, unsigned long TimeOut) { return timedwait(externalMutex, TimeOut); }
    void wakeOne() { signal(); }
    void wakeAll() { broadcast(); }

   private:
#ifdef USE_WIN32_THREADS
    CRITICAL_SECTION m_lock;
    std::vector<HANDLE> m_array{};  // Each thread has its own event for signalling. This is a list of waiting threads.
#endif
#ifdef USE_PTHREADS
    pthread_cond_t m_condition;
#endif
};

//=== ScopedLock ==================================================================

/*
Usage:

void AMethod()
{
    cScopedLock lock(theMutex);

    .. do something critical ..
    if (...) return;
    .. do something critical ..
    if (...) throw ...
    .. do something critical ..
}
*/

class cScopedLock {
   public:
    cScopedLock(cMutex& aMutex, bool DoLock = true) : Mutex(aMutex) {
        LockCnt = 0;
        if (DoLock) lock();
    }

    cScopedLock(const cScopedLock&)            = delete;
    cScopedLock& operator=(const cScopedLock&) = delete;

    ~cScopedLock() {
        if (LockCnt > 0) unlock();
    }

    void lock() {
        Mutex.lock();
        LockCnt++;
    }

    void unlock() {
        LockCnt--;
        Mutex.unlock();
    }

   private:
    cMutex& Mutex;
    unsigned LockCnt;
};

//---------------------------------------------------------------------------------
#endif  // COMMON_SYSTHREADING_H
