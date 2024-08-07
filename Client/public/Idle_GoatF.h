#pragma once
#include "BTNode.h"
#include "GoatFord.h"

BEGIN(Client)
class CIdle_GoatF : public CBTNode
{
private:
	explicit CIdle_GoatF(class CGoatFord* _pGoat);
	virtual ~CIdle_GoatF() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatFord* m_pGoat = nullptr;

public:
	static CIdle_GoatF* Create(CGoatFord* _pGoat);
	virtual void Free()override;
};

END
