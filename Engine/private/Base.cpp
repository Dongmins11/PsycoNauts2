#include "..\public\Base.h"

CBase::CBase()
{

}


/* ���ϰ� : ������Ű�� �� ������ ���� �����Ѵ�. */
unsigned long Engine::CBase::AddRef()
{	
	return ++m_dwRefCnt;
}

/* ���ϰ� : ���ҽ�Ű�� ������ ���� �����Ѵ�. */
unsigned long Engine::CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		/* �蠫����. */
		delete this;

		return 0;
	}
	else
		return m_dwRefCnt--;	
}
