#pragma once

#include "PxPhysicsAPI.h"
using namespace physx;

PxFilterFlags ContactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT
				| PxPairFlag::eNOTIFY_TOUCH_FOUND
				| PxPairFlag::eNOTIFY_TOUCH_LOST
				| PxPairFlag::eNOTIFY_CONTACT_POINTS;

			return PxFilterFlag::eCALLBACK;
		}

		if (PxFilterObjectIsKinematic(attributes0) || PxFilterObjectIsKinematic(attributes1))
		{
			pairFlags = PxPairFlag::eNOTIFY_TOUCH_FOUND
				| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
				| PxPairFlag::eNOTIFY_TOUCH_LOST
				| PxPairFlag::eDETECT_DISCRETE_CONTACT
				| PxPairFlag::eNOTIFY_CONTACT_POINTS;

			return PxFilterFlag::eSUPPRESS;
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		pairFlags = PxPairFlag::eSOLVE_CONTACT
			| PxPairFlag::eDETECT_DISCRETE_CONTACT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
			| PxPairFlag::eNOTIFY_TOUCH_LOST
			| PxPairFlag::eNOTIFY_CONTACT_POINTS;
		//}
		 
		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eSOLVE_CONTACT
		| PxPairFlag::eDETECT_DISCRETE_CONTACT;

	return PxFilterFlag::eDEFAULT;
}