#pragma once

/* Ÿ�ӵ�Ÿ�� �����ش�. */

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;
public:
	HRESULT NativeConstruct();
	_double Compute_TimeDelta();	
private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;
	double				m_TimeDelta;
public:
	static CTimer* Create();
	virtual void Free() override;
};

END