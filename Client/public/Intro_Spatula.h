#pragma once
#include "BTNode.h"
#include "Spatula.h"

BEGIN(Client)
class CIntro_Spatula : public CBTNode
{
private:
	explicit CIntro_Spatula(class CSpatula* _pSpatula);
	virtual ~CIntro_Spatula() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CSpatula*	 m_pSpatula = nullptr;
	_bool		m_IsSpawned = false;

public:
	static CIntro_Spatula* Create(CSpatula* _pSpatula);
	virtual void Free()override;
};

END
