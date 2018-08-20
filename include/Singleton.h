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


          // 미리 0으로 설정하지 않으면 delete되어 clear함수가 호출될 때
          // 또다시 여기를 들어온다.(++Loozend 2004/09/09)
          m_spInstance = 0;
          delete pInstance;
        }

        pthread_mutex_unlock(&m_sLock);
      }
    }

  protected:
    /// 기본 생성자는 외부 생성을 막기 위해 protected로 선언한다.
    Singleton();

  private:
    /// 복사 생성자는 외부 생성을 막기 위해 private으로 선언한다.
    Singleton(const Singleton<TYPE>&) {}

    /// 대입 연산자는 외부 생성을 막기 위해 private으로 선언한다.
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

