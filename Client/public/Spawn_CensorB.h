#pragma once

#include "BTNode.h"
#include "Censor_Burly.h"

BEGIN(Client)
class CSpawn_CensorB : public CBTNode
{
private:
	explicit CSpawn_CensorB(class CCensor_Burly* _pCensor);
	virtual ~CSpawn_CensorB() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CCensor_Burly* m_pCensor = nullptr;
	_bool m_IsSpawned = false;

public:
	static CSpawn_CensorB* Create(CCensor_Burly* _pCensor);
	virtual void Free()override;
};

END