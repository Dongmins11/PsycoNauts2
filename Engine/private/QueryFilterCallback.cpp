#include "QueryFilterCallback.h"

CQueryFilterCallback::CQueryFilterCallback()
{
}

CQueryFilterCallback::CQueryFilterCallback(const PxU32 layerMask)
{
	m_layerMask = layerMask;
}

CQueryFilterCallback::CQueryFilterCallback(const PxU32 layerMask, const _bool _isOnce)
{
	m_layerMask = layerMask;
	m_isOnce = _isOnce;
}

PxQueryHitType::Enum CQueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	PxFilterData filterData0 = shape->getQueryFilterData();
	if (filterData0.word0 & m_layerMask)
	{
		if (m_isOnce)
			return PxQueryHitType::eBLOCK;
		else
			return PxQueryHitType::eTOUCH;
	}

	else
		return PxQueryHitType::eNONE;
}

PxQueryHitType::Enum CQueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
{
	int ia = 0;
	return PxQueryHitType::Enum();
}
