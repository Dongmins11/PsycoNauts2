#pragma once

#include "BTNode.h"
#include "Doubt.h"

BEGIN(Client)
class CSpawn_Doubt : public CBTNode
{
private:
	explicit CSpawn_Doubt(class CDoubt* _pDoubt);
	virtual ~CSpawn_Doubt() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CDoubt* m_pDoubt = nullptr;
	_bool m_IsSpawned = false;
	_double m_dAccTime = 0.0;

public:
	static CSpawn_Doubt* Create(CDoubt* _pDoubt);
	virtual void Free()override;
};

END