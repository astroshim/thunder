#ifndef __SINGLETON_H
#define __SINGLETON_H


template <typename TYPE>
class Singleton
{
protected:
	static TYPE *m_spInstance;
	
	static bool m_sbAlreadyCreate;
	
	static pthread_mutex_t m_sLock;

public:
	virtual ~Singleton()
	{
		clear();
	}

	static TYPE* instance()
	{
		if(m_spInstance == 0 && m_sbAlreadyCreate == false)
		{
			pthread_mutex_lock(&m_sLock);

			if(m_spInstance == 0)
			{
				m_spInstance = new TYPE;
				m_sbAlreadyCreate = true;
			}

			pthread_mutex_unlock(&m_sLock);
		}

		return m_spInstance;
	}

	static void clear()
	{
		if(m_spInstance != 0)
		{
			pthread_mutex_lock(&m_sLock);
			if(m_spInstance != 0)
			{
				TYPE *pInstance = m_spInstance;

				// �̸� 0���� �������� ������ delete�Ǿ� clear�Լ��� ȣ��� ��
				// �Ǵٽ� ���⸦ ���´�.(++Loozend 2004/09/09)
				m_spInstance = 0;
				delete pInstance;
			}

			pthread_mutex_unlock(&m_sLock);
		}
	}

protected:
	/// �⺻ �����ڴ� �ܺ� ������ ���� ���� protected�� �����Ѵ�.
	Singleton();

private:
	/// ���� �����ڴ� �ܺ� ������ ���� ���� private���� �����Ѵ�.
	Singleton(const Singleton<TYPE>&) {}
	
	/// ���� �����ڴ� �ܺ� ������ ���� ���� private���� �����Ѵ�.
	const Singleton<TYPE>& operator = (const Singleton<TYPE> &) 
	{
		return *this;
	}

};

template <typename TYPE>
TYPE* Singleton<TYPE>::m_spInstance=0;

template <typename TYPE>
bool Singleton<TYPE>::m_sbAlreadyCreate=false;

template <typename TYPE>
pthread_mutex_t Singleton<TYPE>::m_sLock = PTHREAD_MUTEX_INITIALIZER;

#endif

