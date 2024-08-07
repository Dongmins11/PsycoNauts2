#pragma once


#include "Engine_Defines.h"

/* 모든 클래스들의 부모클래스 */
/* 모든 클래스들에게 공통적인기능을 상속. */
/* 참조갯수를 관리한다. */
BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	CBase();
	virtual ~CBase() = default;
public:
	unsigned long AddRef(); /* 레퍼런스카우늩를 증가시키낟. */
	unsigned long Release(); /* 레퍼런스카우ㅏㄴ트르 ㄹ감소시킨다.(!= 0) or 삭제한다.(RefCnt == 0) */
private:
	/* 현재 참조된 갯수를 보관한다. */
	/* 원래 보관하고자했던 포인터형변수에 담아놓는 행동(참조x)*/
	/* 담아놨던주소를 다른 객체에게 저장해준다라는 형태로 행동(참조o) */
	unsigned long		m_dwRefCnt = 0;

public:
	virtual void Free() = 0;
};

END