#pragma once
#include "BTNode.h"
#include "GoatFord.h"

BEGIN(Client)
class CSpawn_GoatF : public CBTNode
{
private:
	explicit CSpawn_GoatF(class CGoatFord* _pGoat);
	virtual ~CSpawn_GoatF() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatFord* m_pGoat = nullptr;
	_bool		m_IsSpawned = false;
	_bool		m_IsArrived = false;

public:
	static CSpawn_GoatF* Create(CGoatFord* _pGoat);
	virtual void Free()override;
};

END
