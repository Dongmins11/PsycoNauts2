#pragma once

#include "BTNode.h"
#include "Doubt.h"

BEGIN(Client)
class CDead_Doubt : public CBTNode
{
private:
	explicit CDead_Doubt(class CDoubt* _pDoubt);
	virtual ~CDead_Doubt() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CDoubt* m_pDoubt = nullptr;
	_bool m_IsReadyDead = false;

public:
	static CDead_Doubt* Create(CDoubt* _pDoubt);
	virtual void Free()override;
};

END