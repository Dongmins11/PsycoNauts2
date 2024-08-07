#pragma once
#include "Base.h"
#include "PhysXScene_Manager.h"
#include "PxSimulationEventCallback.h"
#include "GameObject.h"
#include "CollisionObject.h"

BEGIN(Engine)
class CGameObject;

class CSimulationEventCallBack : public PxSimulationEventCallback
{
public:
	typedef struct tagCollisionDesc
	{
		CCollisionObject pSrcCollision;
		CCollisionObject pDestCollision;
		PxPairFlag::Enum eFlag;
	}COLLISIONDESC;

public:
	explicit CSimulationEventCallBack();
	virtual ~CSimulationEventCallBack() = default;

	// PxSimulationEventCallback을(를) 통해 상속됨
	virtual void onConstraintBreak(PxConstraintInfo * constraints, PxU32 count) override;
	virtual void onWake(PxActor ** actors, PxU32 count) override;
	virtual void onSleep(PxActor ** actors, PxU32 count) override;
	virtual void onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs) override;
	virtual void onTrigger(PxTriggerPair * pairs, PxU32 count) override;
	virtual void onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count) override;

	HRESULT UpdateCollsionResult();
	HRESULT UpdateTriggerResult();
	HRESULT NotReadyToCollision();
private:
	list<COLLISIONDESC> m_listCollision;
	list<COLLISIONDESC> m_listTrigger;
};

END