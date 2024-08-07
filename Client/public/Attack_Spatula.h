 #pragma once
#include "BTNode.h"
#include "Spatula.h"

BEGIN(Client)
class CAttack_Spatula : public CBTNode
{
private:
	explicit CAttack_Spatula(class CSpatula* _pSpatula);
	virtual ~CAttack_Spatula() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

public:
	virtual void Reset_AttackState();

private:
	HRESULT Create_Effect(const _tchar* pEffectTag ,CModel* _pModel, CTransform* _pTransform);
	
private:
	CSpatula*	m_pSpatula = nullptr;
	_double		m_dDelayTime = 0.0;
	_uint		m_iAttackType = 0;
	_bool		m_IsDelayAttack = false;
	_bool		m_IsAttackLoop = false;
	_bool		m_IsAttack = false;
	_bool		m_IsRenderEffect[3] = { false , false, false };

public:
	static CAttack_Spatula* Create(CSpatula* _pSpatula);
	virtual void Free()override;
};

END
