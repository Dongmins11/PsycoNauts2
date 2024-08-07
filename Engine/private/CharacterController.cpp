#include "..\public\CharacterController.h"
#include "Transform.h"
#include "GameObject.h"

CCharacterController::CCharacterController(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
	:CComponent(_pDevice, _pDeviceContext)
{
}

CCharacterController::CCharacterController(const CCharacterController & _rhs)
	:CComponent(_rhs)
{
}

HRESULT CCharacterController::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CCharacterController::NativeConstruct(void * _pArg)
{
	if (nullptr != _pArg)
	{
		m_tDesc = *(DESC*)_pArg;
		m_pTranform = (CTransform*)(m_tDesc.pGameObject->Get_Components(TEXT("Com_Transform")));
		//m_pTranform = m_tDesc.pTransform;
		//Safe_AddRef(m_pTranform);
	}

	if (FAILED(__super::NativeConstruct(_pArg)))
		return E_FAIL;

	if (FAILED(Create_Controller()))
		return E_FAIL;

	return S_OK;
}

void CCharacterController::Remove_CharacterController()
{
	m_pPxController->release();
	
}

const CCharacterController::DESC & CCharacterController::Get_Desc() const
{
	return m_tDesc;
}

const _vector CCharacterController::Get_CenterPos() const
{
	return _vector();
}

const _vector CCharacterController::Get_TopPos() const
{
	return _vector();
}

const _vector CCharacterController::Get_FootPos() const
{
	PxExtendedVec3 pxPos =m_pPxController->getFootPosition();

	return XMVectorSet(pxPos.x,pxPos.y, pxPos.z, 0.f);
}

void CCharacterController::SetMaterial(PxMaterial * _pMaterial)
{
	m_pMaterial = _pMaterial;
}

void CCharacterController::SetPxController(PxController * _pPxController)
{
	m_pPxController = _pPxController;
}

void CCharacterController::SetShapes(vector<PxShape*>& _vecShapes)
{
	m_vecShapes = _vecShapes;
}

void CCharacterController::SetShaperLayer(LayerType eType)
{
	CPhysXScene_Manager* pPhysXMgr = CPhysXScene_Manager::Get_Instance();
	if (!pPhysXMgr)
		return;

	for (auto& pShape : m_vecShapes)
		pPhysXMgr->Set_ShapeLayer(pShape, (_uint)eType);

}

void CCharacterController::SetHeight(_float Height)
{
	PxReal pxHeight = Height;
	m_pPxController->resize(pxHeight);
}

const _bool CCharacterController::IsDown()
{
	return m_curFlag & PxControllerCollisionFlag::eCOLLISION_DOWN;
}

const _bool CCharacterController::IsUp()
{
	return m_curFlag & PxControllerCollisionFlag::eCOLLISION_UP;
}

const _bool CCharacterController::IsSides()
{
	return m_curFlag & PxControllerCollisionFlag::eCOLLISION_SIDES;
}

void CCharacterController::Update_Transform()
{
	if (nullptr == m_pPxController)
		return;

	PxExtendedVec3 pxPosition = m_pPxController->getFootPosition();


	_float4 vPosition = { (_float)pxPosition.x, (_float)pxPosition.y, (_float)pxPosition.z, 1.f };
	m_pTranform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));

}

void CCharacterController::Move(const _double & _dTimeDelta, const _float3 _vDisp)
{
	if (nullptr == m_pPxController)
		return;

	PxVec3 pxvVelocity = ToPxVec3(_vDisp);
	if (!pxvVelocity.isFinite())
	{
		m_vVelocity = { 0.f, 0.f, 0.f };
		pxvVelocity = { 0.f, 0.f, 0.f };
	}
	
	m_preFlag = m_curFlag;
	m_curFlag = m_pPxController->move(pxvVelocity, 0.f, (_float)_dTimeDelta, NULL);
	
}

void CCharacterController::Set_Position(const _float3 _vPos)
{
	m_pPxController->setPosition(PxExtendedVec3(_vPos.x, _vPos.y, _vPos.z));
}

void CCharacterController::Set_FootPosition(const _float3 _vPos)
{
	m_pPxController->setFootPosition(PxExtendedVec3(_vPos.x, _vPos.y, _vPos.z));
}

HRESULT CCharacterController::Create_Controller()
{
	CPhysXScene_Manager* pPhysX = CPhysXScene_Manager::Get_Instance();
	if (FAILED(pPhysX->Create_CharacterController(this)))
		return E_FAIL;

	return S_OK;
}

CTransform * CCharacterController::Get_Transform() const
{
	CTransform* pTransform = (CTransform*)(m_tDesc.pGameObject->Get_Components(TEXT("Com_Transform")));
	return pTransform;
}

CCharacterController * CCharacterController::Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	CCharacterController* pInstance = new CCharacterController(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed To Create CCharacterController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCharacterController::Clone(void * _pArg)
{
	CCharacterController* pInstance = new CCharacterController(*this);
	if (FAILED(pInstance->NativeConstruct(_pArg)))
	{
		MSGBOX("Failed To Create CCharacterController");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCharacterController::Free()
{
	__super::Free();

	//Safe_Release(m_pTranform);

}
