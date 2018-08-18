#ifndef __NONCOPYABLE_H
#define __NONCOPYABLE_H

//////////////////////////////////////////////////////////////////////////////
/// \class NonCopyable
/// \brief ���� �����ڳ� ���� �����ڰ� ���Ǹ� ����� Ŭ�������� �� Ŭ������ 
/// private���� ����ϸ�, ������ �� ������ �߻��ϸ鼭 ���� �����ڳ� ���� 
/// �����ڸ� ����� �� ���ٰ� �˷��ش�.
//////////////////////////////////////////////////////////////////////////////

class NonCopyable
{
protected:
	NonCopyable() {}
	~NonCopyable() {}


private:
	NonCopyable(const NonCopyable&);
	const NonCopyable& operator = (const NonCopyable&);
};

#ifdef _MSC_VER
	// ���� �����ڸ� �������� ���߽��ϴ�.
	#pragma warning(disable:4511)

	// �����Ϸ����� �־��� Ŭ������ ���� ���� �����ڸ� ������ �� �����ϴ�.
	#pragma warning(disable:4512)
#endif

/// ������ NonCopyable�� �ٷ� ��ӹ��� ���ϴ� Ŭ��������, 
/// ���� ������ �� �����ڸ� ���� ���� ��ũ��
#define DECLARE_NONCOPYABLE(NAME) private: \
	NAME(const NAME&) {} \
	NAME& operator = (const NAME&) { return *this; }

#endif //__NONCOPYABLE_H

