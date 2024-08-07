#pragma once
#include "BTNode.h"
#include "GoatOtto.h"

BEGIN(Client)
class CTaunt_GoatO : public CBTNode
{
private:
	explicit CTaunt_GoatO(class CGoatOtto* _pGoat);
	virtual ~CTaunt_GoatO() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CGoatOtto* m_pGoat = nullptr;

public:
	static CTaunt_GoatO* Create(CGoatOtto* _pGoat);
	virtual void Free()override;
};

END
