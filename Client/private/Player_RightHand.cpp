#include "stdafx.h"
#include "..\public\Player_RightHand.h"
#include "GameInstance.h"
#include "Right_Hand_Anim.h"
#include "CollisionObject.h"
#include "Enemy.h"
CPlayer_RightHand::CPlayer_RightHand(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer_RightHand::CPlayer_RightHand(const CPlayer_RightHand & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer_RightHand::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_RightHand::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg, SCENE_STATIC)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	if (FAILED(SetUp_MyBoneMatrices("Rt_Middle1")))
		return E_FAIL;

	m_isRender = true;

	m_pModelCom->Set_Animation(0);

	m_Right_HandAnim = CRight_Hand_Anim::Create(this, m_pTransformCom, m_pModelCom, m_pSphereCom,m_pColliderCom, m_iCurrentScene);
	if (nullptr == m_Right_HandAnim)
	{
		MSGBOX("Failed to Create RightHand_Anim");
		return E_FAIL;
	}

	return S_OK;
}

_int CPlayer_RightHand::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;
	
	if (false == m_bFirstBoneCheck)
	{
		if (FAILED(SetUp_TargetBoneMatrices("DeltaTrans")))
			return E_FAIL;

		m_bFirstBoneCheck = true;
	}

	if (!m_isRender)
		return 0;

	m_Right_HandAnim->Update(TimeDelta);

	return _int();
}

_int CPlayer_RightHand::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (!m_isRender)
		return 0;

	if (true == m_Right_HandAnim->Get_RenderCheck())
	{
		if (false == m_Right_HandAnim->Get_BoneCaculationCheck())
			Bone_MatrixCalculation();
		if (true == m_Right_HandAnim->Get_ThrowBoneCaculationCheck())
			ThrowBone_MatrixCalculation();

		m_pColliderCom->Add_Actor();
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODEL_EFFECT, this);
	}
	else
	{
		m_pColliderCom->Remove_Actor();
	}

	if (m_isInteraction)
	{
		m_isInteraction = false;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Add_ObjectToEffect(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"),
			pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_RightHandHitEffect"),
			SCENE_STATIC, TEXT("Effect_HitEffect3"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		if (FAILED(pGameInstance->Effect_Call_Inheritance(pGameInstance->Get_CurrentSceneIndex(), true, false, m_pTransformCom, TEXT("Layer_RightHandHitEffect"))))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		_vector vPos = XMLoadFloat3(&m_vTargetEffectPos) - m_pTransformCom->Get_Position();
		pGameInstance->Setting_Effect_MovePos(pGameInstance->Get_CurrentSceneIndex(), TEXT("Layer_RightHandHitEffect"), TEXT("Com_Option"), vPos);

		RELEASE_INSTANCE(CGameInstance);
	}
	//My_Bone_MatrixCalculation();

	return _int();
}

HRESULT CPlayer_RightHand::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(6)))
		return E_FAIL;


	return S_OK;
}

void CPlayer_RightHand::Set_ChangeAnimCheck(_bool bChangeCheck)
{
	if (nullptr == m_Right_HandAnim)
		return;
	m_Right_HandAnim->Set_ChangeCheck(bChangeCheck);
}

void CPlayer_RightHand::Set_HandState(_uint iHandState)
{
	if (nullptr == m_Right_HandAnim)
		return;
	m_Right_HandAnim->Set_HandState(iHandState);
}

_uint CPlayer_RightHand::Get_HandState() const
{
	if (nullptr == m_Right_HandAnim)
		return -1;

	return m_Right_HandAnim->Get_Right_HandState();
}

_bool CPlayer_RightHand::Get_DishGrabCheck() const
{
	if (nullptr == m_Right_HandAnim)
		return false;

	return m_Right_HandAnim->Get_Dish_Grab();
}

_bool CPlayer_RightHand::Get_DishThrowCheck() const
{
	if (nullptr == m_Right_HandAnim)
		return false;

	return m_Right_HandAnim->Get_Dish_Throw();
}

const _float4x4 * CPlayer_RightHand::Get_BoneMatrix() const
{
	if (nullptr == m_pBoneMatrix)
		return nullptr;

	return m_pBoneMatrix;
}

void CPlayer_RightHand::Set_PullingSuccessCheck(_bool bPullingCheck)
{
	if (nullptr == m_Right_HandAnim)
		return;

	m_Right_HandAnim->Set_PullingSuccessCheck(bPullingCheck);
}


void CPlayer_RightHand::Bone_MatrixCalculation()
{
	if (nullptr != m_pCombinedMatrix || nullptr != m_pTargetWorldMatrix)
	{
		_float4x4		TransformationMatrix;
		_matrix			RootTransformationMatrix;
		_vector			RootScale;
		_vector			RootRight, RootUp, RootLook;

		_matrix TargetTransformationMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedMatrix) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(m_pTargetWorldMatrix);
		RootTransformationMatrix = XMLoadFloat4x4(&m_MyWorldMatrix) * TargetTransformationMatrix;

		RootScale = XMVectorSet(XMVectorGetX(XMVector3Length(RootTransformationMatrix.r[0])), XMVectorGetY(XMVector3Length(RootTransformationMatrix.r[1])), XMVectorGetZ(XMVector3Length(RootTransformationMatrix.r[2])), 1.f);

		RootRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(RootScale);
		RootUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(RootScale);
		RootLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(RootScale);

		RootTransformationMatrix.r[0] = RootRight;
		RootTransformationMatrix.r[1] = RootUp;
		RootTransformationMatrix.r[2] = RootLook;

		XMStoreFloat4x4(&TransformationMatrix, RootTransformationMatrix);

		m_pTransformCom->Set_WorldMatrix(TransformationMatrix);

		m_pTransformCom->Set_MyLook(XMLoadFloat4((_float4*)&m_pTargetWorldMatrix->m[2]));

		_matrix ColiderMatrix = XMLoadFloat4x4(&m_MyOffsetMatrix) * XMLoadFloat4x4(m_pMyCombinedMatrix) * XMLoadFloat4x4(&m_MyPivotMatrix) * XMLoadFloat4x4(&m_pTransformCom->Get_WorldFloat4x4());
		XMStoreFloat4x4(&m_ColiderMatirx, ColiderMatrix);
		m_pBoneMatrix = &m_ColiderMatirx;

		m_pColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pBoneMatrix));
		m_pColliderCom->Update_Transform(m_pBoneMatrix);
	}

}

void CPlayer_RightHand::ThrowBone_MatrixCalculation()
{
	if (nullptr != m_pCombinedMatrix || nullptr != m_pTargetWorldMatrix)
	{
		_float4x4		TransformationMatrix;
		_matrix			RootTransformationMatrix;
		_vector			RootScale;
		_vector			RootRight, RootUp, RootLook;

		_matrix TargetTransformationMatrix = XMLoadFloat4x4(&m_OffsetMatrix) * XMLoadFloat4x4(m_pCombinedMatrix) * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(m_pTargetWorldMatrix);
		RootTransformationMatrix = XMLoadFloat4x4(&m_MyWorldMatrix) * TargetTransformationMatrix;

		RootScale = XMVectorSet(XMVectorGetX(XMVector3Length(RootTransformationMatrix.r[0])), XMVectorGetY(XMVector3Length(RootTransformationMatrix.r[1])), XMVectorGetZ(XMVector3Length(RootTransformationMatrix.r[2])), 1.f);

		RootRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(RootScale);
		RootUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(RootScale);
		RootLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(RootScale);

		RootTransformationMatrix.r[0] = RootRight;
		RootTransformationMatrix.r[1] = RootUp;
		RootTransformationMatrix.r[2] = RootLook;
		RootTransformationMatrix.r[3] += ((XMVector3Normalize(RootRight) * 1.5f) + (XMVector3Normalize(RootLook) * 1.5f));

		XMStoreFloat4x4(&TransformationMatrix, RootTransformationMatrix);

		m_pTransformCom->Set_WorldMatrix(TransformationMatrix);

		m_pTransformCom->Set_MyLook(XMLoadFloat4((_float4*)&m_pTargetWorldMatrix->m[2]));

		_matrix ColiderMatrix = XMLoadFloat4x4(&m_MyOffsetMatrix) * XMLoadFloat4x4(m_pMyCombinedMatrix) * XMLoadFloat4x4(&m_MyPivotMatrix) * XMLoadFloat4x4(&m_pTransformCom->Get_WorldFloat4x4());
		XMStoreFloat4x4(&m_ColiderMatirx, ColiderMatrix);
		m_pBoneMatrix = &m_ColiderMatirx;

		m_pColliderCom->Update_TransformPhysX(XMLoadFloat4x4(m_pBoneMatrix));
		m_pColliderCom->Update_Transform(m_pBoneMatrix);
	}
}



void CPlayer_RightHand::OnTriggerEnter(CCollisionObject & collision)
{
	if (0 == m_Right_HandAnim->Get_Right_HandState() || 1 == m_Right_HandAnim->Get_Right_HandState())
	{
		if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_ATTAKABLE)
		{
			OutputDebugStringW(TEXT("Col-Trigger-Static충돌:HandRight\n"));

			if (collision.Get_ObjectTransform())
			{
				((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->SetGravity(true);
				((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->Add_Force(m_pTransformCom->Get_State(CTransform::STATE_LOOK)*10.f, PxForceMode::eVELOCITY_CHANGE);
				((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->SetRestitution(.5f);
				((CPhysXCollider*)collision.Get_Object()->Get_Components(TEXT("Com_Collider")))->Add_Force((m_pTransformCom->Get_State(CTransform::STATE_LOOK) + m_pTransformCom->Get_State(CTransform::STATE_UP))*3.f, PxForceMode::eIMPULSE);
				collision.Get_Object()->Set_IsDestroy(true);

			}
			OutputDebugString(TEXT("Hit PickObject!!\n"));
		}

		if (collision.Get_ObjectLayerType() == LayerType::LAYERTYPE_MONSTER)
		{
			OutputDebugStringW(TEXT("Col-Trigger-Static충돌:HandRight\n"));

			if (collision.Get_ObjectTransform())
			{
				if (!m_isInteraction)
				{
					m_isInteraction = true;
					_float fRandRight = GET_RANDOMFLOAT(-0.5f, 0.5f);
					_float fRandUp = GET_RANDOMFLOAT(0.5f, 1.5f);
					XMStoreFloat3(&m_vTargetEffectPos, ((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_Position() + 
						((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_State(CTransform::STATE_UP) * fRandUp +
						((CTransform*)collision.Get_Object()->Get_Components(TEXT("Com_Transform")))->Get_State(CTransform::STATE_RIGHT) * fRandRight);
				}
			}
			OutputDebugString(TEXT("Hit Monster!!\n"));
		}

	}
}


HRESULT CPlayer_RightHand::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Model_Player_RightHand"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 7.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	
	/* Com_Collider */
	CPhysXCollider::DESC tColliderDesc;

	tColliderDesc.eType = CPhysXScene_Manager::TYPE_DYNAMIC;
	tColliderDesc.isVisualization = true;
	tColliderDesc.isTrigger = true;
	tColliderDesc.pGameObject = this;
	tColliderDesc.isSceneQuery = true;

	CSphereCollider::DESC tSphereColliderDesc;
	tSphereColliderDesc.tColliderDesc = tColliderDesc;
	tSphereColliderDesc.vScale = {0.4f,0.4f,0.4f}; //MULTI_FLOAT3(vColSize, 2.0);

	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Collider_PxSphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tSphereColliderDesc)))
		return E_FAIL;
	m_eLayerType = LayerType::LAYERTYPE_PLAYER_SKILL;
	
	//_matrix smatPviot = XMMatrixIdentity();
	//m_pColliderCom->SetPivotMatrix(smatPviot);

	m_pColliderCom->Set_ShapeLayer(m_eLayerType);

	m_pColliderCom->Update_TransformPhysX();
	m_pColliderCom->Update_Transform();


	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_RightHand::SetUp_TargetBoneMatrices(const char * pBoneName)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*	pTargetTransform = (CTransform*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Transform"), 0);
	if (nullptr == pTargetTransform)
		return E_FAIL;

	CModel*		pModel = (CModel*)pGameInstance->Get_Components(m_iCurrentScene, TEXT("Layer_Player"), TEXT("Com_Model"), 0);
	if (nullptr == pModel)
		return E_FAIL;

	m_OffsetMatrix = pModel->Get_OffsetMatrix(pBoneName);
	m_PivotMatrix = pModel->Get_PivotMatrix();
	m_pCombinedMatrix = pModel->Get_CombinedMatrixPtr(pBoneName);
	m_pTargetWorldMatrix = pTargetTransform->Get_WorldMatrixPtr();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer_RightHand::SetUp_MyBoneMatrices(const char * pBoneName)
{
	if (nullptr == m_pModelCom)
	{
		MSGBOX("Failed to CPlayer_RightHand : Funtion SetUp_MyBoneMatrices");
		return E_FAIL;
	}

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	m_MyOffsetMatrix = m_pModelCom->Get_OffsetMatrix(pBoneName);
	m_MyPivotMatrix = m_pModelCom->Get_PivotMatrix();
	m_pMyCombinedMatrix = m_pModelCom->Get_CombinedMatrixPtr(pBoneName);
	m_MyWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer_RightHand::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 TransPoseWorldMat, ViewTransPoseMat, ProjectionTransPoseMat;

	XMStoreFloat4x4(&TransPoseWorldMat, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjectionTransPoseMat, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	if (FAILED(m_pModelCom->Set_RawValue("g_WorldMatrix", &TransPoseWorldMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ViewMatrix", &ViewTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Set_RawValue("g_ProjMatrix", &ProjectionTransPoseMat, sizeof(_float4x4))))
		return E_FAIL;

	m_pModelCom->Set_TextureValue("g_AlphaMapTexture", m_pTextureCom->Get_SRV(6));

	_float fTimeAcc = (_float)m_Right_HandAnim->Get_TimeAcc();
	m_pModelCom->Set_RawValue("g_fTimeAcc", &fTimeAcc, sizeof(_float));

	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}

CPlayer_RightHand * CPlayer_RightHand::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer_RightHand*	pInstance = new CPlayer_RightHand(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CPlayer_RightHand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_RightHand::Clone(void * pArg, _uint iSceneIndex)
{
	CPlayer_RightHand*	pInstance = new CPlayer_RightHand(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created : CPlayer_RightHand");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_RightHand::Free()
{
	__super::Free();

	Safe_Release(m_Right_HandAnim);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pTextureCom);
}
