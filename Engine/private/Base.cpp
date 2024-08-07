#include "..\public\Base.h"

CBase::CBase()
{

}


/* 리턴값 : 증가시키고 난 이후의 값을 리턴한다. */
unsigned long Engine::CBase::AddRef()
{	
	return ++m_dwRefCnt;
}

/* 리턴값 : 감소시키기 이전에 값을 리턴한다. */
unsigned long Engine::CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();

		/* 삭젷나다. */
		delete this;

		return 0;
	}
	else
		return m_dwRefCnt--;	
}
