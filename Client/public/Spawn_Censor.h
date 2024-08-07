#pragma once

#include "BTNode.h"
#include "Censor.h"

BEGIN(Client)
class CSpawn_Censor : public CBTNode
{
private:
	explicit CSpawn_Censor(class CCensor* _pCensor);
	virtual ~CSpawn_Censor() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor* m_pCensor = nullptr;
	_bool m_IsSpawned = false;

public:
	static CSpawn_Censor* Create(CCensor* _pCensor);
	virtual void Free()override;
};

END