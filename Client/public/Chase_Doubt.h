#pragma once

#include "BTNode.h"
#include "Doubt.h"

BEGIN(Client)
class CChase_Doubt : public CBTNode
{
private:
	explicit CChase_Doubt(class CDoubt* _pDoubt);
	virtual ~CChase_Doubt() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CDoubt* m_pDoubt = nullptr;

public:
	static CChase_Doubt* Create(CDoubt* _pDoubt);
	virtual void Free()override;
};

END