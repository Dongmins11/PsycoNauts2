#pragma once
#include "BTNode.h"
#include "GoatOtto.h"

BEGIN(Client)
class CIdle_GoatO : public CBTNode
{
private:
	explicit CIdle_GoatO(class CGoatOtto* _pGoat);
	virtual ~CIdle_GoatO() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatOtto* m_pGoat = nullptr;

public:
	static CIdle_GoatO* Create(CGoatOtto* _pGoat);
	virtual void Free()override;
};

END
