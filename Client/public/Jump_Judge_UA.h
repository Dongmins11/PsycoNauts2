#pragma once

#include "BTNode.h"
#include "Judge.h"

BEGIN(Client)
class CJump_Judge_UA : public CBTNode
{
private:
	explicit CJump_Judge_UA(class CJudge* _pJudge);
	virtual ~CJump_Judge_UA() = default;

public:
	virtual _bool Invoke(_double TimeDelta, CModel* _pModel, CTransform* _pTransform, CCharacterController* _pController)override;

private:
	CJudge* m_pJudge = nullptr;
	_bool m_IsSpawned = false;

public:
	static CJump_Judge_UA* Create(CJudge* _pJudge);
	virtual void Free()override;
};

END