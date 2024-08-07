#include "ControllerHitReport.h"
#include "PhysXScene_Manager.h"
#include "GameObject.h"
#include "CollisionObject.h"
CControllerHitReport::CControllerHitReport()
	: PxUserControllerHitReport()
{
}

HRESULT CControllerHitReport::NativeConstruct()
{
	return S_OK;
}

void CControllerHitReport::onShapeHit(const PxControllerShapeHit & hit)
{
	CGameObject* pColObject = static_cast<CGameObject*>(hit.actor->userData);
	CGameObject* pControllerObject = static_cast<CGameObject*>(hit.controller->getActor()->userData);
	if (!pColObject || !pControllerObject)
		return;

	CCollisionObject shapeCollision;
	shapeCollision.Set_GameObject(pControllerObject);
	shapeCollision.Set_HitPosition(FromPxextendedVec3(hit.worldPos));
	CCollisionObject controllerCollision;
	controllerCollision.Set_GameObject(pColObject);

	controllerCollision.Set_HitPosition(FromPxextendedVec3(hit.worldPos));

	pColObject->OnControllerCollisionHit(shapeCollision);
	pControllerObject->OnControllerCollisionHit(controllerCollision);

}

void CControllerHitReport::onControllerHit(const PxControllersHit & hit)
{
	OutputDebugString(TEXT("onControllerHit\n"));

}

void CControllerHitReport::onObstacleHit(const PxControllerObstacleHit & hit)
{
	OutputDebugString(TEXT("onObstacleHit\n"));

}
