#pragma once
#include "BTNode.h"
#include "GoatFord.h"

BEGIN(Client)
class CTaunt_GoatF : public CBTNode
{
private:
	explicit CTaunt_GoatF(class CGoatFord* _pGoat);
	virtual ~CTaunt_GoatF() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatFord* m_pGoat = nullptr;

public:
	static CTaunt_GoatF* Create(CGoatFord* _pGoat);
	virtual void Free()override;
};

END
