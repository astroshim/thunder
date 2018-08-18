#ifndef  __THREADMANAGER_H
#define  __THREADMANAGER_H

#include <list>
#include <pthread.h>

//#include "./Thread.h"

class Thread;

class ThreadManager : public NonCopyable
{
public:
	static	ThreadManager*	GetInstance();
	static	void			ReleaseInstance();

	bool Spawn(Thread *pThread);

	/* 
	*	��ϵ� thread�� �۾� �Ϸ�� thread�� �������ִ� ��ƾ�� ���� �Ѵ�. 
	**/

private:
	ThreadManager();
	virtual ~ThreadManager();

	static	ThreadManager*		m_SelfInstance;
	std::list	< Thread* >	m_lstThreadHandle;

};
#endif

