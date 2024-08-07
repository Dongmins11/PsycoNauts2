#pragma once
#include "BTNode.h"
#include "GoatHollis.h"

BEGIN(Client)
class CSpawn_GoatH : public CBTNode
{
private:
	explicit CSpawn_GoatH(class CGoatHollis* _pGoat);
	virtual ~CSpawn_GoatH() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatHollis* m_pGoat = nullptr;
	_bool		m_IsSpawned = false;
	_bool		m_IsArrived = false;

public:
	static CSpawn_GoatH* Create(CGoatHollis* _pGoat);
	virtual void Free()override;
};

END
