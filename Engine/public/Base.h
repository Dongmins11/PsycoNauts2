#pragma once


#include "Engine_Defines.h"

/* ��� Ŭ�������� �θ�Ŭ���� */
/* ��� Ŭ�����鿡�� �������α���� ���. */
/* ���������� �����Ѵ�. */
BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	CBase();
	virtual ~CBase() = default;
public:
	unsigned long AddRef(); /* ���۷���ī��p�� ������Ű��. */
	unsigned long Release(); /* ���۷���ī�줿��Ʈ�� �����ҽ�Ų��.(!= 0) or �����Ѵ�.(RefCnt == 0) */
private:
	/* ���� ������ ������ �����Ѵ�. */
	/* ���� �����ϰ����ߴ� �������������� ��Ƴ��� �ൿ(����x)*/
	/* ��Ƴ����ּҸ� �ٸ� ��ü���� �������شٶ�� ���·� �ൿ(����o) */
	unsigned long		m_dwRefCnt = 0;

public:
	virtual void Free() = 0;
};

END