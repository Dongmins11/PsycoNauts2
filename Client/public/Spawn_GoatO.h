#pragma once
#include "BTNode.h"
#include "GoatOtto.h"

BEGIN(Client)
class CSpawn_GoatO : public CBTNode
{
private:
	explicit CSpawn_GoatO(class CGoatOtto* _pGoat);
	virtual ~CSpawn_GoatO() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatOtto*	m_pGoat = nullptr;
	_bool		m_IsSpawned = false;
	_bool		m_IsArrived = false;

public:
	static CSpawn_GoatO* Create(CGoatOtto* _pGoat);
	virtual void Free()override;
};

END
