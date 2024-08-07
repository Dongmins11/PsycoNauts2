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

	// PxUserControllerHitReport��(��) ���� ��ӵ�
	virtual void onShapeHit(const PxControllerShapeHit & hit) override;
	virtual void onControllerHit(const PxControllersHit & hit) override;
	virtual void onObstacleHit(const PxControllerObstacleHit & hit) override;

};
END
