#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CDead_Judge : public CBTNode
{
private:
	explicit CDead_Judge(class CJudge* _pJudge);
	virtual ~CDead_Judge() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_bool m_IsReadyDead = false;

public:
	static CDead_Judge* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END