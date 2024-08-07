#pragma once
#include "BTNode.h"
#include "GoatOtto.h"

BEGIN(Client)
class CDead_GoatO : public CBTNode
{
private:
	explicit CDead_GoatO(class CGoatOtto* _pGoat);
	virtual ~CDead_GoatO() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatOtto* m_pGoat = nullptr;
	_bool		m_IsReadyDead = false;
	_float		m_fScale = 1.75f;

public:
	static CDead_GoatO* Create(CGoatOtto* _pGoat);
	virtual void Free()override;
};

END
