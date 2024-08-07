#pragma once
#include "BTNode.h"
#include "Beater.h"

BEGIN(Client)
class CTaunt_Beater : public CBTNode
{
private:
	explicit CTaunt_Beater(class CBeater* _pBeater);
	virtual ~CTaunt_Beater() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBeater*	m_pBeater = nullptr;
	_double		m_dDelayTime = 0.0;

public:
	static CTaunt_Beater* Create(CBeater* _pBeater);
	virtual void Free()override;
};

END
