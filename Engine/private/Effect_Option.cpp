#include "..\public\Effect_Option.h"
#include "Transform.h"
#include "Model.h"

CEffect_Option::CEffect_Option(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CEffect_Option::CEffect_Option(const CEffect_Option & rhs)
	: CComponent(rhs)
{
}

_matrix CEffect_Option::Get_ParentMatrix()
{
	return m_pParentTransform->Get_WorldMatrix();
}

_matrix CEffect_Option::Get_NormalParentMatrix()
{
	_matrix ParentMat = m_pParentTransform->Get_WorldMatrix();
	ParentMat.r[0] = XMVector4Normalize(ParentMat.r[0]);
	ParentMat.r[1] = XMVector4Normalize(ParentMat.r[1]);
	ParentMat.r[2] = XMVector4Normalize(ParentMat.r[2]);

	return ParentMat;
}

_matrix CEffect_Option::Get_ParentBillboardMatrix(_vector vCamPos)
{
	return m_pParentTransform->Get_BillBoardMatrix(vCamPos);
}

_matrix CEffect_Option::Get_WorldPosMatrix()
{
	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[3] = XMLoadFloat4(&_float4(m_vMovePos, 1.f));

 	return WorldMatrix;
}

_matrix CEffect_Option::Get_ParentBoneMatrix()
{
	if (nullptr != m_pCombinedMatrix ||
		nullptr != m_pTargetWorldMatrix)
	{
		_matrix		TransformMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedMatrix) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(m_pTargetWorldMatrix);
		return TransformMatrix;
	}
	return XMMatrixIdentity();
}

void CEffect_Option::Set_ParentTransform(_bool isOnce, _bool isNonBillboard, CTransform * pTransfrom)
{
	m_isOnce = isOnce;
	m_isNonBillboard = isNonBillboard;

	m_pParentTransform = pTransfrom;
	Safe_AddRef(pTransfrom);
}

void CEffect_Option::Set_ParentModel(CModel * pModel)
{
	m_pParentModel = pModel;
	Safe_AddRef(pModel);
}

HRESULT CEffect_Option::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Option::NativeConstruct(void * pArg)
{
	return S_OK;
}

void CEffect_Option::Setting_BoneMatrix(const char * pBoneName)
{
	m_OffsetMatrix = m_pParentModel->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix = m_pParentModel->Get_PivotMatrix();
	m_pCombinedMatrix = m_pParentModel->Get_CombinedMatrixPtr(pBoneName);
	m_pTargetWorldMatrix = m_pParentTransform->Get_WorldMatrixPtr();
}

CEffect_Option * CEffect_Option::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffect_Option*	pInstance = new CEffect_Option(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CEffect_Option");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CEffect_Option::Clone(void * pArg)
{
	CEffect_Option*	pInstance = new CEffect_Option(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CEffect_Option");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Option::Free()
{
	__super::Free();

	if (nullptr != m_pParentTransform)
		Safe_Release(m_pParentTransform);

	if (nullptr != m_pParentModel)
		Safe_Release(m_pParentModel);
}
