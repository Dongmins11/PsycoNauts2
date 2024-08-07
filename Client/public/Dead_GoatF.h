#pragma once
#include "BTNode.h"
#include "GoatFord.h"

BEGIN(Client)
class CDead_GoatF : public CBTNode
{
private:
	explicit CDead_GoatF(class CGoatFord* _pGoat);
	virtual ~CDead_GoatF() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatFord* m_pGoat = nullptr;
	_bool		m_IsReadyDead = false;
	_bool		m_IsDigestion = false;
	_bool		m_IsIdle = false;
	_float		m_fScale = 1.75f;

public:
	static CDead_GoatF* Create(CGoatFord* _pGoat);
	virtual void Free()override;
};

END
