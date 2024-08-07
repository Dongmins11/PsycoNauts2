#pragma once

#include "Base.h"

/* 타이머를 여러개 생성하여 보관한다.  */

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

private: /* 생성한 타이머를 보관하기위한 컨테이너. */
	unordered_map<const _tchar*, class CTimer*>			m_Timers;
	typedef unordered_map<const _tchar*, class CTimer*>	TIMERS;

private: /* 효과용 타이머 */
	_bool	m_isStiffness = false;

private: /* 효과용 타이머 함수 */
	

private:
	class CTimer* Find_Timer(const _tchar* pTimerTag);

public:
	virtual void Free() override;
};

END