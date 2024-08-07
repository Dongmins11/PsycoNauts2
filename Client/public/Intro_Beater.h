#pragma once
#include "BTNode.h"
#include "Beater.h"

BEGIN(Client)
class CIntro_Beater : public CBTNode
{
private:
	explicit CIntro_Beater(class CBeater* _pBeater);
	virtual ~CIntro_Beater() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CBeater* m_pBeater = nullptr;
	_bool		m_IsSpawned = false;
	_bool		m_IsRenderEffect = false;
	_bool		m_IsDestroyEffect = false;
public:
	static CIntro_Beater* Create(CBeater* _pBeater);
	virtual void Free()override;
};

END
