#pragma once

#include "Base.h"
#include "PhysXScene_Manager.h"

BEGIN(Engine)

class CControllerBehaviorCallback : public PxControllerBehaviorCallback
{
public:
	explicit CControllerBehaviorCallback();
	virtual ~CControllerBehaviorCallback() = default;

public:
	HRESULT NativeConstruct();

public:
	// PxControllerBehaviorCallback을(를) 통해 상속됨
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) override;
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) override;
};

END