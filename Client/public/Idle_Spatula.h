#pragma once
#include "BTNode.h"
#include "Spatula.h"

BEGIN(Client)
class CIdle_Spatula : public CBTNode
{
private:
	explicit CIdle_Spatula(class CSpatula* _pSpatula);
	virtual ~CIdle_Spatula() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CSpatula*	m_pSpatula = nullptr;

public:
	static CIdle_Spatula* Create(CSpatula* _pSpatula);
	virtual void Free()override;
};

END
