#pragma once

#include "Base.h"
#include "PhysXScene_Manager.h"


BEGIN(Engine)

class CQueryFilterCallback : public PxQueryFilterCallback
{
public:
	explicit CQueryFilterCallback();
	explicit CQueryFilterCallback(const PxU32 layerMask);
	explicit CQueryFilterCallback(const PxU32 layerMask, const _bool _isOnce);
	virtual ~CQueryFilterCallback() = default;
	
	// PxQueryFilterCallback을(를) 통해 상속됨
	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override;

private:
	PxU32 m_layerMask = 0;
	_bool m_isOnce = false;
};

END