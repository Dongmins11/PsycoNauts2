#pragma once

#include "BTNode.h"
#include "BadIdea.h"

BEGIN(Client)
class CSpawn_BadIdea : public CBTNode
{
private:
	explicit CSpawn_BadIdea(class CBadIdea* _pBadIdea);
	virtual ~CSpawn_BadIdea() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBadIdea* m_pBadIdea = nullptr;
	_bool m_IsSpawned = false;
	_bool m_IsLand = false;

public:
	static CSpawn_BadIdea* Create(CBadIdea* _pCensor);
	virtual void Free()override;
};

END