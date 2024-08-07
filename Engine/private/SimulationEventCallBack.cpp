#include "..\public\SimulationEventCallBack.h"
#include "GameObject.h"


CSimulationEventCallBack::CSimulationEventCallBack()
{
}


void CSimulationEventCallBack::onConstraintBreak(PxConstraintInfo * constraints, PxU32 count)
{
}

void CSimulationEventCallBack::onWake(PxActor ** actors, PxU32 count)
{
}

void CSimulationEventCallBack::onSleep(PxActor ** actors, PxU32 count)
{
}

void CSimulationEventCallBack::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{
	for (_uint i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];
 
		if (cp.flags & (PxContactPairFlag::eREMOVED_SHAPE_0 | PxContactPairFlag::eREMOVED_SHAPE_1))
		{
			continue;
		}

		CGameObject* pSrcObject = static_cast<CGameObject*>(pairHeader.actors[0]->userData);
		CGameObject* pDestObject= static_cast<CGameObject*>(pairHeader.actors[1]->userData);

		if (!pSrcObject || !pDestObject || pDestObject->Get_IsDestroy() || pSrcObject->Get_IsDestroy())
			continue;

		COLLISIONDESC tCollisionDesc;
		tCollisionDesc.pSrcCollision.Set_GameObject(pSrcObject);
		tCollisionDesc.pDestCollision.Set_GameObject(pDestObject);

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			tCollisionDesc.eFlag = PxPairFlag::eNOTIFY_TOUCH_FOUND;
			
		}
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			tCollisionDesc.eFlag = PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
		}
		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			tCollisionDesc.eFlag = PxPairFlag::eNOTIFY_TOUCH_LOST;
		}

		m_listCollision.emplace_back(tCollisionDesc);
	}
}

void CSimulationEventCallBack::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
 	for (_uint i = 0; i < count; i++)
	{
		const PxTriggerPair& tp = pairs[i];

		// 도형이 삭제되었을 때 쌍 무시 
		if (tp.flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
		{
			continue;
		}

		CGameObject* pSrcObject = static_cast<CGameObject*>(tp.triggerActor->userData);
		CGameObject* pDestObject = static_cast<CGameObject*>(tp.otherActor->userData);

		if (pSrcObject->Get_IsDestroy() || pDestObject->Get_IsDestroy())
			continue;


		
		COLLISIONDESC tCollisionDesc;
		tCollisionDesc.pSrcCollision.Set_GameObject(pSrcObject);
		tCollisionDesc.pDestCollision.Set_GameObject(pDestObject);


		if (tp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			tCollisionDesc.eFlag = PxPairFlag::eNOTIFY_TOUCH_FOUND;
		}

		if (tp.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			//_bool isFind = false;
			//for (auto& list : m_listTrigger)
			//{
			//	if (list.eFlag == PxPairFlag::eNOTIFY_TOUCH_PERSISTS && list.pDestCollision.Get_Object() == pDestObject)
			//	{
			//		list.eFlag = PxPairFlag::eNOTIFY_TOUCH_LOST;
			//		isFind = true;
			//		break;
			//	}
			//}
			//if (isFind)
			//	continue;

			tCollisionDesc.eFlag = PxPairFlag::eNOTIFY_TOUCH_LOST;
		}
		
		m_listTrigger.emplace_back(tCollisionDesc);
	}
}

void CSimulationEventCallBack::onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count)
{
	
}

HRESULT CSimulationEventCallBack::UpdateCollsionResult()
{
	if (m_listCollision.size() <= 0)
		return S_OK;

	for (auto& tCollisionDesc : m_listCollision)
	{
		switch (tCollisionDesc.eFlag)
		{
		case PxPairFlag::eNOTIFY_TOUCH_FOUND:
			tCollisionDesc.pSrcCollision.CollisionEnter(tCollisionDesc.pDestCollision);
			tCollisionDesc.pDestCollision.CollisionEnter(tCollisionDesc.pSrcCollision);
			break;
		case PxPairFlag::eNOTIFY_TOUCH_PERSISTS:
			tCollisionDesc.pSrcCollision.CollisionStay(tCollisionDesc.pDestCollision);
			tCollisionDesc.pDestCollision.CollisionStay(tCollisionDesc.pSrcCollision);
			break;
		case PxPairFlag::eNOTIFY_TOUCH_LOST:
			tCollisionDesc.pSrcCollision.CollisionExit(tCollisionDesc.pDestCollision);
			tCollisionDesc.pDestCollision.CollisionExit(tCollisionDesc.pSrcCollision);
			break;
		}
	}

	m_listCollision.clear();
	return S_OK;
}

HRESULT CSimulationEventCallBack::UpdateTriggerResult()
{

	if (m_listTrigger.size() <= 0)
		return S_OK;


	for (auto& tCollisionDesc : m_listTrigger)
	{
		if (!tCollisionDesc.pSrcCollision.Get_Object() || !tCollisionDesc.pDestCollision.Get_Object())
		{
			continue;
		}
		switch (tCollisionDesc.eFlag)
		{
		case PxPairFlag::eNOTIFY_TOUCH_FOUND:
			tCollisionDesc.pSrcCollision.TriggerEnter(tCollisionDesc.pDestCollision);
			tCollisionDesc.pDestCollision.TriggerEnter(tCollisionDesc.pSrcCollision);
			//tCollisionDesc.eFlag = PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
			break;
		case PxPairFlag::eNOTIFY_TOUCH_LOST:
			tCollisionDesc.pSrcCollision.TriggerExit(tCollisionDesc.pDestCollision);
			tCollisionDesc.pDestCollision.TriggerExit(tCollisionDesc.pSrcCollision);
			break;
		}
	}

	//auto        iter = m_listTrigger.begin();
	//
	//for (; iter != m_listTrigger.end();)
	//{
	//	if ((*iter).eFlag == PxPairFlag::eNOTIFY_TOUCH_LOST)
	//	{
	//		iter = m_listTrigger.erase(iter);
	//	}
	//	else
	//		++iter;
	//}

	m_listTrigger.clear();

	return S_OK;
}

HRESULT CSimulationEventCallBack::NotReadyToCollision()
{
	m_listCollision.clear();
	m_listTrigger.clear();

	return S_OK;
}

