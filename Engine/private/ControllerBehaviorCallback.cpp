#include "ControllerBehaviorCallback.h"

CControllerBehaviorCallback::CControllerBehaviorCallback()
	: PxControllerBehaviorCallback()
{
}

HRESULT CControllerBehaviorCallback::NativeConstruct()
{
	return S_OK;
}

PxControllerBehaviorFlags CControllerBehaviorCallback::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CControllerBehaviorCallback::getBehaviorFlags(const PxController& controller)
{
	return PxControllerBehaviorFlags();
}

PxControllerBehaviorFlags CControllerBehaviorCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
	return PxControllerBehaviorFlags();
}
