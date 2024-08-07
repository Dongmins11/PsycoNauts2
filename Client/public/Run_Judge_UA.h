#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CRun_Judge_UA : public CBTNode
{
private:
	explicit CRun_Judge_UA(class CJudge* _pJudge);
	virtual ~CRun_Judge_UA() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_float	m_fSpeed = 0.f;

public:
	static CRun_Judge_UA* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END