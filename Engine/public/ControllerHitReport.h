#pragma once
#include "Base.h"
#include "PhysXScene_Manager.h"
BEGIN(Engine)

class CControllerHitReport : public PxUserControllerHitReport
{
public:
	explicit CControllerHitReport();
	virtual ~CControllerHitReport() = default;

public:
	HRESULT NativeConstruct();

	// PxUserControllerHitReport을(를) 통해 상속됨
	virtual void onShapeHit(const PxControllerShapeHit & hit) override;
	virtual void onControllerHit(const PxControllersHit & hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit & hit) override;

};
END
