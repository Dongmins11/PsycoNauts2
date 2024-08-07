#pragma once
#include "BTNode.h"
#include "Spatula.h"

BEGIN(Client)
class CTaunt_Spatula : public CBTNode
{
private:
	explicit CTaunt_Spatula(class CSpatula* _pSpatula);
	virtual ~CTaunt_Spatula() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CSpatula*	m_pSpatula = nullptr;

public:
	static CTaunt_Spatula* Create(CSpatula* _pSpatula);
	virtual void Free()override;
};

END
