//---------------------------------------------------------------------------
/*
 * Name: $Id: sysThreading.cpp 273 2014-05-02 12:08:58Z roman $: 
 *
 *  
 * 
 * Small wrapper around OS dependent threading functionality. 
 * Mutex, Threads and Condition variables.
 *
 * ELSYS EXPRESSLY DISCLAIMS ALL WARRANITIES OR REPRESENTATIONS WITH RESPECT TO
 * THIS SOFTWARE AND DOCUMENTATION AND ANY SUPPORT OR MAINTENANCE SERVICES THAT
 * ELSYS MAY CHOOSE TO PROVIDE IN RELATION THERETO (INCLUDING, WITHOUT
 * LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE OR THAT THE SOFTWARE: WILL BE
 * ERROR-FREE, WILL OPERATE WITHOUT INTERUPTION, WILL NOT INFRINGE THE RIGHTS OF A
 * THIRD PARTY, OR WILL PRODUCE RESULTS IN CONNECTION WITH ITS USE). FURTHER,
 * ELSYS EXPRESSLY DISCLAIMS ALL EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * (C) Copyright 2005 - 2019 Elsys AG. All rights reserved.
*/

#include "sysThreading.h"
#include <assert.h>

#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <sys/timeb.h>
#endif

#ifdef _LINUX
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
//#include <asm-generic/errno-base.h>
//#include <asm-generic/errno.h>
#endif


#include <string>
#include <exception>
#include <algorithm>
using namespace std;

//---------------------------------------------------------------------------------

class SysThreadingException : public exception {
public:
	SysThreadingException(const string &msg) {
		m_message = msg;
	}
	virtual ~SysThreadingException()
	throw() {}
	virtual const char	*what() const
	throw() {
		return m_message.c_str();
	}

private:
	string				m_message;
};



#ifdef USE_WIN32_THREADS
string getSysError() {
	LPVOID MsgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR) &MsgBuf,
		0,
		NULL);
	string Message = static_cast<char *>(MsgBuf);
	LocalFree(MsgBuf);

	return Message;
}
#endif

#ifdef _LINUX
string getSysError()
{
	string errStr;
	errStr.resize(100);
	//strerror_r(errno, &errStr[0], 100);
	errStr = strerror(errno);
	return errStr;
}	
#endif

//=== Mutex =======================================================================



#ifdef USE_WIN32_THREADS

cMutex::cMutex(bool recursive) {
	InitializeCriticalSection(&Mutex);
	m_isRecursive = recursive;
	m_lockCount = 0;
}

cMutex::~cMutex() {
	DeleteCriticalSection(&Mutex);
}
	
void cMutex::lock() {
	EnterCriticalSection(&Mutex);

	// Prevent nested locks if the mutex is non-recursive.
	if (!m_isRecursive && m_lockCount > 0)	
		throw SysThreadingException("Non-recursive mutex is already locked by this thread.");

	m_lockCount++;
}


void cMutex::unlock() {
	// Since we have a lock counter, check for under-locking errors.
	if (m_lockCount == 0)
		throw SysThreadingException("Mutex is not locked.");

	m_lockCount--;
	LeaveCriticalSection(&Mutex);
}

#endif


#ifdef USE_PTHREADS


cMutex::cMutex(bool recursive)				
{
	pthread_mutexattr_t Attr;
	int Status = pthread_mutexattr_init(&Attr);
	if (Status != 0) // ENOMEM
		throw SysThreadingException("Could not initialize mutex attributes.");

	int AttrType = recursive ? PTHREAD_MUTEX_RECURSIVE : PTHREAD_MUTEX_ERRORCHECK;
	Status = pthread_mutexattr_settype(&Attr, AttrType);
	assert(Status == 0); //EINVAL

	Status = pthread_mutex_init(&m_mutex, &Attr);
	pthread_mutexattr_destroy(&Attr);

	if (Status != 0) // EAGAIN, ENOMEM, EPERM
		throw SysThreadingException("Could not initialize mutex [" + getSysError() + "]");
}


cMutex::~cMutex()	
{
	int Status = pthread_mutex_destroy(&m_mutex);
	if (Status == EBUSY)
		throw SysThreadingException("Cannot destroy locked mutex [" + getSysError() + "]");
}


void cMutex::lock()
{
	int Status = pthread_mutex_lock(&m_mutex);
	if (Status == EDEADLK)
		throw SysThreadingException("Deadlock detected [" + getSysError() + "]");
	assert(Status == 0);
}


void cMutex::unlock()
{
	int Status = pthread_mutex_unlock(&m_mutex);
	if (Status == EPERM)
		throw SysThreadingException("The calling thread does not own the mutex lock [" + getSysError() + "]");
	assert(Status == 0);
}

#endif


//=== Threads ===================================================================

#ifdef USE_WIN32_THREADS


static DWORD s_tlsCondEventIx;
static bool s_tlsInitialized = false;
static cMutex s_tlsMutex;

void CheckTls() {
	// Check if the thread local storage is already initialized.

	// Quick check without blocking, returns most of the time
	if (s_tlsInitialized) return;

	// Serious test 
	s_tlsMutex.lock();
	if (s_tlsInitialized) {
		s_tlsMutex.unlock();
		return;
	}
	
	// Initialize the thread local storage.
	s_tlsCondEventIx = TlsAlloc();
	if (s_tlsCondEventIx == TLS_OUT_OF_INDEXES)
		throw SysThreadingException("Could not allocate thread local storage [" + getSysError() + "]");

	s_tlsInitialized = true;
	s_tlsMutex.unlock();
}



unsigned int __stdcall NonWaitableThread::StartNonWaitableThread(NonWaitableThread *thread) {
	try {
		thread->run();
	}
	catch(...) {
	}

	// The thread is not waitable. Deallocate its ressources now.
	thread->DeallocateResources();
	delete thread;

	return 0;
}


NonWaitableThread::NonWaitableThread() {
	m_threadHandle = 0;
}


void NonWaitableThread::DeallocateResources() {
	if (m_threadHandle == 0)
		return;

	// Delete the thread's condition event
	if (s_tlsInitialized) {
		HANDLE event = TlsGetValue(s_tlsCondEventIx);
		if (event != 0) {
			CloseHandle(event);
			event = 0;
			TlsSetValue(s_tlsCondEventIx, NULL);
		}
	}

	CloseHandle(m_threadHandle);
	m_threadHandle = 0;
}


void NonWaitableThread::start() {
	m_threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, (unsigned int(__stdcall*)(void*))StartNonWaitableThread, this, 0, &m_threadId));
	if (m_threadHandle == 0)
		throw SysThreadingException("Could not get a thread handle [" + getSysError() + "]");
}


bool NonWaitableThread::setPriority(Priority priority) {
	return SetThreadPriority(m_threadHandle, priority) != 0 ? true : false;
}


unsigned int NonWaitableThread::getThreadId() {
	return m_threadId;
}


unsigned int NonWaitableThread::getCurrentThreadId() {
	return GetCurrentThreadId();
}


unsigned int __stdcall WaitableThread::StartWaitableThread(WaitableThread *thread) {
	thread->m_running = true;

	try {
		thread->run();
	}
	catch(...) {
	}

	thread->m_running = false;
	thread->m_stopped = true;

	// This thread is waitable.
	// Deallocate its resources in the wait() function.
	return 0;
}


WaitableThread::WaitableThread()
	:NonWaitableThread() {
	m_running = false;
	m_stopped = true;
}


void WaitableThread::wait() {
	// Wait for the thread to terminate
	int err = WaitForSingleObject(m_threadHandle, INFINITE);
	if (err != 0)
		throw SysThreadingException("WaitForSingleObject returned an error [" + getSysError() + "]");

	// The thread has terminated, deallocate its resources
	DeallocateResources();
}


void WaitableThread::start() {
	m_stopped = false;
	m_threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, (unsigned int(__stdcall*)(void*))StartWaitableThread, this, 0, &m_threadId));
	if (m_threadHandle == 0)
		throw SysThreadingException("Could not get a thread handle [" + getSysError() + "]");
	
}




#endif


//---------------------------------------------------------------------------------


#ifdef USE_PTHREADS


void* NonWaitableThread::StartNonWaitableThread(NonWaitableThread* thread)
{
	// Make non-waitable
	pthread_detach(pthread_self());

	try {
		thread->run();
	}
	catch(...) {
	}

	// The thread is not waitable. Deallocate its ressources now.
	delete thread;

	pthread_exit(NULL);
	return NULL;
}


NonWaitableThread::NonWaitableThread(){
	m_thread = 0;
}


void NonWaitableThread::start()
{
	int err = pthread_create(&m_thread, NULL, (void*(*)(void*))StartNonWaitableThread, this);

	if (err != 0)
		throw SysThreadingException("pthread_create returned an error [" + getSysError() + "]");
}


bool NonWaitableThread::setPriority(Priority priority)
{
  return false;
}


unsigned int NonWaitableThread::getThreadId()
{
#ifdef _LINUX
	return (unsigned int)(m_thread);
#else
	return (unsigned int)(m_thread.p);
#endif
}


unsigned int NonWaitableThread::getCurrentThreadId()
{
#ifdef _LINUX
	return (unsigned int)(pthread_self());
#else
	return (unsigned int)(pthread_self().p);
#endif
}


void* WaitableThread::StartWaitableThread(WaitableThread* thread)
{
	thread->m_running = true;

	try {
		thread->run();
	}
	catch(...) {
	}

	thread->m_running = false;
 	thread->m_stopped = true;

	// This thread is waitable.
	// Deallocate its resources in the wait() function.
	pthread_exit(NULL);
	return NULL;
}


WaitableThread::WaitableThread()
:NonWaitableThread()
{
	m_running = false;
  m_stopped = true;
}


void WaitableThread::start()
{
	 m_stopped = false;
	m_pthreadJoined	= false;
	int err = pthread_create(&m_thread, NULL, (void*(*)(void*))StartWaitableThread, this);

	if (err != 0)
		throw SysThreadingException("pthread_create returned an error [" + getSysError() + "]");
 
}



void WaitableThread::wait()
{
	
	if(m_pthreadJoined || m_thread == 0){ 
		return;	
	}
	// Wait for the thread to terminate
	int err = pthread_join(m_thread, NULL);
 	m_pthreadJoined	= true;
	if (err != 0)
        throw SysThreadingException("pthread_join failed [" + getSysError() + "]");
}



#endif


void NonWaitableThread::sleep(unsigned long milliSeconds) {
#ifdef WIN32
	Sleep(milliSeconds);
#endif

#ifdef _LINUX
		usleep(1000 * milliSeconds);
	#endif
}




//=== Conditions ==================================================================


#ifdef USE_WIN32_THREADS


// This implementation is based on the code found in GLIB, gthread-win32.c

cCondition::cCondition() {
	InitializeCriticalSection(&m_lock);
}



cCondition::~cCondition() {
	DeleteCriticalSection(&m_lock);
}



void cCondition::signal() {
	EnterCriticalSection(&m_lock);

	if (m_array.size() > 0) {
		SetEvent(m_array[0]);
		m_array.erase(m_array.begin());
	}

	LeaveCriticalSection(&m_lock);
}



void cCondition::broadcast() {
	EnterCriticalSection(&m_lock);

	for (unsigned int i = 0; i < m_array.size(); i++)
		SetEvent(m_array[i]);

	m_array.clear();

	LeaveCriticalSection(&m_lock);
}



void cCondition::wait(cMutex &externalMutex) {
	timedwait(externalMutex, INFINITE);
}



bool cCondition::timedwait(cMutex &externalMutex, unsigned long milliSeconds) {
	// Get local storage for each thread's event, if not already allocated.
	CheckTls();

	// Get the thread's condition event, or create one if it does not have one yet.
	HANDLE event = TlsGetValue(s_tlsCondEventIx);

	if (event == 0) {
		event = CreateEvent(0, FALSE, FALSE, NULL);
		if (event == 0)
			throw SysThreadingException("Could not create event handle [" + getSysError() + "]");

		TlsSetValue(s_tlsCondEventIx, event);
	}

	EnterCriticalSection(&m_lock);

	// The event must not be signaled. Check this.
	assert(WaitForSingleObject(event, 0) == WAIT_TIMEOUT);

	// Add the event to the list of waiters.
	m_array.push_back(event);
	LeaveCriticalSection(&m_lock);
	
	// Unlock the external mutex and wait for the event.
	assert(externalMutex.m_lockCount == 1);
	externalMutex.unlock();

	DWORD retval = WaitForSingleObject(event, milliSeconds);
	if (retval == WAIT_FAILED)
		throw SysThreadingException("WaitForSingleObject returned an error [" + getSysError() + "]");

	// Acquire the external mutex again.
	externalMutex.lock();

	if (retval == WAIT_TIMEOUT) {
		// Handle timeouts: Remove from the list of 
		// waiters, and check for late signals.

		EnterCriticalSection(&m_lock);

		// Remove from the list.
		m_array.erase(remove(m_array.begin(), m_array.end(), event), m_array.end());

		// In the meantime we could have been signaled, so we must again
		// wait for the signal, this time with no timeout, to reset
		// it. retval is set again to honour the late arrival of the
		// signal 
		retval = WaitForSingleObject(event, 0);
		if (retval == WAIT_FAILED)
			throw SysThreadingException("WaitForSingleObject returned an error [" + getSysError() + "]");

		LeaveCriticalSection(&m_lock);
	}

	/*
		// Now event must not be inside the array, check this 
		EnterCriticalSection(&m_lock);
		vector<HANDLE>::iterator pos = find(m_array.begin(), m_array.end(), event);
		assert(pos == m_array.end());
		LeaveCriticalSection(&m_lock);
	*/

	return retval != WAIT_TIMEOUT;
}


#endif




#ifdef USE_PTHREADS


cCondition::cCondition()
{
	int Status = pthread_cond_init(&m_condition, 0);
	if (Status != 0) // EAGAIN, ENOMEM, EBUSY
		throw SysThreadingException("pthread_cond_init returned an error [" + getSysError() + "]");
}


cCondition::~cCondition()
{
	int Status = pthread_cond_destroy(&m_condition);
	if (Status == EBUSY)
		throw SysThreadingException("The condition variable condition is referenced by another thread [" + getSysError() + "]");
}



void cCondition::wait(cMutex& externalMutex)
{
	int err = pthread_cond_wait(&m_condition, &externalMutex.m_mutex);
	if (err != 0)
		throw SysThreadingException("pthread_cond_wait returned an error [" + getSysError() + "]");
}



bool cCondition::timedwait(cMutex& externalMutex, unsigned long milliSeconds)
{
	// Calculate the absolute time needed for pthread_cond_timedwait

	// Get current system time
	#ifdef WIN32
		struct timespec abstime;
		struct _timeb currSysTime;
		_ftime(&currSysTime);
		abstime.tv_sec = currSysTime.time;
		abstime.tv_nsec = currSysTime.millitm * 1000000;
	#endif

	#ifdef _LINUX
		struct timespec abstime;
		struct timeval AbsTime;
		gettimeofday(&AbsTime, 0);
		abstime.tv_sec = AbsTime.tv_sec;
		abstime.tv_nsec = AbsTime.tv_usec * 1000;
	#endif

	// Add the timeout
	abstime.tv_nsec += (milliSeconds % 1000) * 1000000;
	abstime.tv_sec += milliSeconds / 1000;
	if (abstime.tv_nsec >= 1000000000) {
		abstime.tv_nsec -= 1000000000;
		abstime.tv_sec++;
	}

	// Timed wait
	int err = pthread_cond_timedwait(&m_condition, &externalMutex.m_mutex, &abstime);
	if (err == 0) return true;
	if (err == ETIMEDOUT) return false;

	throw SysThreadingException("pthread_cond_timedwait returned an error [" + getSysError() + "]");
}



void cCondition::signal()
{
	int Status = pthread_cond_signal(&m_condition);
	if (Status != 0)
		throw SysThreadingException("pthread_cond_signal returned an error [" + getSysError() + "]");
}



void cCondition::broadcast()
{
	int Status = pthread_cond_broadcast(&m_condition);
	if (Status != 0)
		throw SysThreadingException("pthread_cond_broadcast returned an error [" + getSysError() + "]");
}



#endif


//---------------------------------------------------------------------------------
