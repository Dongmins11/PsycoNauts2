#pragma once

#include "Base.h"

/* Ÿ�̸Ӹ� ������ �����Ͽ� �����Ѵ�.  */

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)
public:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;	

public:
	_bool	Get_Stiffness() { return m_isStiffness; }

public:
	void	Set_Stiffness(_bool isStiffness) { m_isStiffness = isStiffness; }

public:
	HRESULT Add_Timer(const _tchar* pTimerTag);
	_double Compute_TimeDelta(const _tchar* pTimerTag);

private: /* ������ Ÿ�̸Ӹ� �����ϱ����� �����̳�. */
	unordered_map<const _tchar*, class CTimer*>			m_Timers;
	typedef unordered_map<const _tchar*, class CTimer*>	TIMERS;

private: /* ȿ���� Ÿ�̸� */
	_bool	m_isStiffness = false;

private: /* ȿ���� Ÿ�̸� �Լ� */
	

private:
	class CTimer* Find_Timer(const _tchar* pTimerTag);

public:
	virtual void Free() override;
};

END