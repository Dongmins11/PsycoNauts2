#include "stdafx.h"
#include "..\public\SpinHole.h"
#include "GameInstance.h"

CSpinHole::CSpinHole(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CSpinHole::CSpinHole(const CSpinHole & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSpinHole::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpinHole::NativeConstruct(void * pArg, _uint iCurrentScene)
{
	if (FAILED(__super::NativeConstruct(pArg, iCurrentScene)))
		return E_FAIL;

	if (FAILED(SetUp_Component()))
		return E_FAIL;

	ZeroMemory(&m_tRectInfo, sizeof(EFFECTRECTINFO));
	memcpy(&m_tRectInfo, pArg, sizeof(EFFECTRECTINFO));

	m_pTransformCom->Set_WorldMatrix(m_tRectInfo.WorldMatrix);

	m_CDT_isPlay = true;
	return S_OK;
}

_int CSpinHole::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	TimeDelta *= m_pOptionCom->Get_EffectSpeed();

	Compute_ViewZ(m_pTransformCom);

	if (m_CDT_TimeAcc >= m_tRectInfo.CDT_fEndTime || !m_CDT_isPlay)
	{
		m_CDT_TimeAcc = 0.0;
		m_CDT_isPlay = true;
	}
	else
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		Effect_Time(TimeDelta);
		if (!m_isRender)
		{
			RELEASE_INSTANCE(CGameInstance);
			return 0;
		}

		if (m_CDT_Start)
		{
			m_US_iNowSprite = m_US_fCurIndexX + m_US_fCurIndexY;

			if (m_tRectInfo.UA_Chack)
			{
				m_UA_UVAccX += m_tRectInfo.UA_AniX * static_cast<_float>(TimeDelta);
				m_UA_UVAccY += m_tRectInfo.UA_AniY * static_cast<_float>(TimeDelta);
			}
			if (m_tRectInfo.US_Chack)
			{
				m_US_SpriteAcc += TimeDelta;
				if (m_pOptionCom->Get_UVSpriteTime() <= m_US_SpriteAcc)
				{
					m_US_fCurIndexX++;
					m_US_SpriteAcc = 0.0;
				}
			}

			if (FAILED(Pattern_Option(TimeDelta)))
				return -1;
		}
		if (m_tRectInfo.I_isBillboardY)
			m_pTransformCom->BillBoardY(pGameInstance->Get_CamPosition());

		if (m_tRectInfo.I_isBillboard)
			m_pTransformCom->BillBoard(pGameInstance->Get_CamPosition());

		RELEASE_INSTANCE(CGameInstance);
	}

	return 0;
}

_int CSpinHole::LateTick(_double TimeDelta)
{
	if (0 > (__super::LateTick(TimeDelta)))
		return -1;

	if (false == m_isRender)
		return 0;

	if (m_CDT_TimeAcc >= m_tRectInfo.CDT_fStartTime && m_CDT_Start)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_RECT_EFFECT, this);

	return 0;
}

HRESULT CSpinHole::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBuffer->Render(m_tRectInfo.iPass);

	return S_OK;
}

void CSpinHole::Effect_Time(_double TimeDetla)
{
	if (m_CDT_isPlay)
	{
		m_CDT_TimeAcc += TimeDetla;

		if (m_CDT_TimeAcc > (m_tRectInfo.CDT_fStartTime + m_tRectInfo.CDT_fEndTime))
		{
			m_CDT_isPlay = false;
		}
	}

	if (m_CDT_TimeAcc >= m_tRectInfo.CDT_fEndTime)
		m_CDT_Start = false;

	if (m_CDT_TimeAcc >= m_tRectInfo.CDT_fStartTime &&
		m_CDT_TimeAcc < m_tRectInfo.CDT_fEndTime)
		m_CDT_Start = true;

	if (m_CDT_Start)
	{
		if (m_tRectInfo.FO_Degree == 0.f)
			m_tRectInfo.FO_Degree = 0.5f;

		Effect_TimeAcc(&m_tRectInfo.PP_Chack, (m_tRectInfo.PP_fStartTime + m_tRectInfo.PP_fEndTime), &m_PP_TimeAcc, TimeDetla);
		Effect_TimeAcc(&m_tRectInfo.PS_Chack, (m_tRectInfo.PS_fStartTime + m_tRectInfo.PS_fEndTime), &m_PS_TimeAcc, TimeDetla);
		Effect_TimeAcc(&m_tRectInfo.PR_Chack, (m_tRectInfo.PR_fStartTime + m_tRectInfo.PR_fEndTime), &m_PR_TimeAcc, TimeDetla);
		Effect_TimeAcc(&m_tRectInfo.FI_Chack, (m_tRectInfo.FI_fStartTime + m_tRectInfo.FI_fEndTime), &m_FI_TimeAcc, TimeDetla);
		Effect_TimeAcc(&m_tRectInfo.FO_Chack, (m_tRectInfo.FO_fStartTime + m_tRectInfo.FO_fEndTime + m_tRectInfo.FO_Degree), &m_FO_TimeAcc, TimeDetla);
		Effect_TimeAcc(&m_tRectInfo.UA_Chack, (m_tRectInfo.UA_fStartTime + m_tRectInfo.UA_fEndTime), &m_UA_TimeAcc, TimeDetla);
	}

}

void CSpinHole::Effect_TimeAcc(_bool* pPlay, _float fTotalTime, _double* pTargetTimeAcc, _double TimeDelta)
{
	if (*pPlay)
	{
		*pTargetTimeAcc += static_cast<_float>(TimeDelta);

		if (*pTargetTimeAcc > fTotalTime)
		{
			*pTargetTimeAcc = 0.0;
		}
	}
}

HRESULT CSpinHole::Pattern_Option(_double TimeDelta)
{
	_matrix LocalMatrix = XMLoadFloat4x4(&m_tRectInfo.WorldMatrix);

	if (m_tRectInfo.PS_Chack)
	{
		_float fStartTime = m_tRectInfo.PS_fStartTime;
		_float fEndTime = m_tRectInfo.PS_fEndTime;
		if (0 != m_tRectInfo.PS_Refeat)
		{
			fStartTime = m_tRectInfo.PS_fStartTime / m_tRectInfo.PS_Refeat;
			fEndTime = m_tRectInfo.PS_fEndTime / m_tRectInfo.PS_Refeat;
		}

		_vector vScale = XMVectorSet(
			Option_Type(m_tRectInfo.PS_EffectChackX, m_PS_TimeAcc, fStartTime, m_tRectInfo.vScale.x, m_tRectInfo.PS_vEnd.x, fEndTime, TimeDelta),
			Option_Type(m_tRectInfo.PS_EffectChackY, m_PS_TimeAcc, fStartTime, m_tRectInfo.vScale.y, m_tRectInfo.PS_vEnd.y, fEndTime, TimeDelta),
			Option_Type(m_tRectInfo.PS_EffectChackZ, m_PS_TimeAcc, fStartTime, m_tRectInfo.vScale.z, m_tRectInfo.PS_vEnd.z, fEndTime, TimeDelta),
			1.f
		);

		_vector vRight = LocalMatrix.r[0];
		_vector vUp = LocalMatrix.r[1];
		_vector vLook = LocalMatrix.r[2];

		vRight = XMVector3Normalize(vRight) * XMVectorGetX(vScale);
		vUp = XMVector3Normalize(vUp) * XMVectorGetY(vScale);
		vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

		if (m_pOptionCom->Get_SetScale())
		{
			LocalMatrix.r[0] = vRight * m_pOptionCom->Get_Scale().x;
			LocalMatrix.r[1] = vUp * m_pOptionCom->Get_Scale().y;
			LocalMatrix.r[2] = vLook * m_pOptionCom->Get_Scale().z;
		}
		else
		{
			LocalMatrix.r[0] = vRight;
			LocalMatrix.r[1] = vUp;
			LocalMatrix.r[2] = vLook;
		}
	}


	if (m_tRectInfo.PR_Chack)
	{
		_float fStartTime = m_tRectInfo.PR_fStartTime;
		_float fEndTime = m_tRectInfo.PR_fEndTime;
		if (0 != m_tRectInfo.PR_Refeat)
		{
			fStartTime = m_tRectInfo.PR_fStartTime / m_tRectInfo.PR_Refeat;
			fEndTime = m_tRectInfo.PR_fEndTime / m_tRectInfo.PR_Refeat;
		}

		_float3 vRot = _float3(
			XMConvertToRadians(Option_Type(m_tRectInfo.PR_EffectChackX, m_PR_TimeAcc, fStartTime, m_tRectInfo.vRot.x, m_tRectInfo.PR_vEnd.x, fEndTime, TimeDelta)),
			XMConvertToRadians(Option_Type(m_tRectInfo.PR_EffectChackY, m_PR_TimeAcc, fStartTime, m_tRectInfo.vRot.y, m_tRectInfo.PR_vEnd.y, fEndTime, TimeDelta)),
			XMConvertToRadians(Option_Type(m_tRectInfo.PR_EffectChackZ, m_PR_TimeAcc, fStartTime, m_tRectInfo.vRot.z, m_tRectInfo.PR_vEnd.z, fEndTime, TimeDelta))
		);

		_vector vScale = XMVectorSet(XMVectorGetX(XMVector3Length(LocalMatrix.r[0])),
			XMVectorGetY(XMVector3Length(LocalMatrix.r[1])),
			XMVectorGetZ(XMVector3Length(LocalMatrix.r[2]))
			, 0.f);

		_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(vScale);
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(vScale);
		_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(vScale);

		_matrix RotationMat = XMMatrixRotationRollPitchYaw(vRot.x, vRot.y, vRot.z);

		vRight = XMVector3TransformNormal(vRight, RotationMat);
		vUp = XMVector3TransformNormal(vUp, RotationMat);
		vLook = XMVector3TransformNormal(vLook, RotationMat);

		LocalMatrix.r[0] = vRight;
		LocalMatrix.r[1] = vUp;
		LocalMatrix.r[2] = vLook;
	}

	if (m_tRectInfo.PP_Chack)
	{
		_float fStartTime = m_tRectInfo.PP_fStartTime;
		_float fEndTime = m_tRectInfo.PP_fEndTime;
		if (0 != m_tRectInfo.PP_Refeat)
		{
			fStartTime = m_tRectInfo.PP_fStartTime / m_tRectInfo.PP_Refeat;
			fEndTime = m_tRectInfo.PP_fEndTime / m_tRectInfo.PP_Refeat;
		}

		_vector vPos = XMVectorSet(
			Option_Type(m_tRectInfo.PP_EffectChackX, m_PP_TimeAcc, fStartTime, m_tRectInfo.vPos.x, m_tRectInfo.PP_vEnd.x, fEndTime, TimeDelta),
			Option_Type(m_tRectInfo.PP_EffectChackY, m_PP_TimeAcc, fStartTime, m_tRectInfo.vPos.y, m_tRectInfo.PP_vEnd.y, fEndTime, TimeDelta),
			Option_Type(m_tRectInfo.PP_EffectChackZ, m_PP_TimeAcc, fStartTime, m_tRectInfo.vPos.z, m_tRectInfo.PP_vEnd.z, fEndTime, TimeDelta),
			1.f
		);

		XMStoreFloat3((_float3*)&LocalMatrix.r[3], vPos);
	}
	_float4x4 WorldMatrix;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (!m_isOnce)
	{
		if (m_pOptionCom->Get_NonBillboard())
		{
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentMatrix());
			m_ParentMat._41 += m_pOptionCom->Get_MovePos().x;
			m_ParentMat._42 += m_pOptionCom->Get_MovePos().y;
			m_ParentMat._43 += m_pOptionCom->Get_MovePos().z;
		}
		else if (m_pOptionCom->Get_WorldPosEnable())
		{
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_WorldPosMatrix());
		}
		else
		{
			XMStoreFloat4x4(&m_ParentMat, m_pOptionCom->Get_ParentBillboardMatrix(pGameInstance->Get_CamPosition()));
			m_ParentMat._41 += m_pOptionCom->Get_MovePos().x;
			m_ParentMat._42 += m_pOptionCom->Get_MovePos().y;
			m_ParentMat._43 += m_pOptionCom->Get_MovePos().z;
		}
	}

	if (m_pOptionCom->Get_NonRotion())
	{
		_float3 vPos = _float3(m_ParentMat._41, m_ParentMat._42, m_ParentMat._43);

		XMStoreFloat4x4(&m_ParentMat, XMMatrixIdentity());
		m_ParentMat._41 = vPos.x;
		m_ParentMat._42 = vPos.y;
		m_ParentMat._43 = vPos.z;
	}

	if (m_pOptionCom->Get_Once())
		m_isOnce = true;

	if (m_pOptionCom->Get_SetScale())
	{
		LocalMatrix.r[0] *= m_pOptionCom->Get_Scale().x;
		LocalMatrix.r[1] *= m_pOptionCom->Get_Scale().y;
		LocalMatrix.r[2] *= m_pOptionCom->Get_Scale().z;
	}

	RELEASE_INSTANCE(CGameInstance);
	XMStoreFloat4x4(&WorldMatrix, LocalMatrix * XMLoadFloat4x4(&m_ParentMat));
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

_float CSpinHole::Option_Type(_bool* isOptionType, _double TimeAcc, _float fStartTime, _float fStartValue, _float fEndValue, _float fEndTime, _double TimeDelta)
{
	_float fResult = 0.0;

	_bool isChackOption[OPTION_END] = { isOptionType[OPTION_LINEAR], isOptionType[OPTION_EASE_IN_QUAD], isOptionType[OPTION_EASE_OUT_QUAD],isOptionType[OPTION_EASE_IN_OUT] };
	if (isChackOption[OPTION_LINEAR])
	{
		//Float값이라 TimeDelta에 곱해줘서 디렉선에 적용
		fResult = Linear((static_cast<_float>(TimeAcc) - fStartTime), fStartValue, fEndValue, fEndTime);
	}
	else if (isChackOption[OPTION_EASE_IN_QUAD])
	{
		fResult = Ease_In_Quad((static_cast<_float>(TimeAcc) - fStartTime), fStartValue, fEndValue, fEndTime);
	}
	else if (isChackOption[OPTION_EASE_OUT_QUAD])
	{
		fResult = Ease_Out_Quad((static_cast<_float>(TimeAcc) - fStartTime), fStartValue, fEndValue, fEndTime);
	}
	else if (isChackOption[OPTION_EASE_IN_OUT])
	{
		fResult = Ease_In_Out((static_cast<_float>(TimeAcc) - fStartTime), fStartValue, fEndValue, fEndTime);
	}
	return fResult;

}

_float CSpinHole::Linear(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return fEndValue * fCurTime / fEndTime + fStartValue;
}

_float CSpinHole::Ease_In_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime;

	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return fEndValue * fCurTime * fCurTime + fStartValue;
}

_float CSpinHole::Ease_Out_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime;

	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return -fEndValue * fCurTime * (fCurTime - 2) + fStartValue;
}

_float CSpinHole::Ease_In_Out(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime / 2;
	if (0 == fEndTime)
		return fEndValue + fStartValue;

	if (fCurTime < 1)
		return fEndValue / 2 * fCurTime * fCurTime + fStartValue;
	fCurTime--;
	return -fEndValue / 2 * (fCurTime * (fCurTime - 2) - 1) + fStartValue;
}

_float CSpinHole::Ease_Out_Bounce(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	_float n1 = 7.5625f;
	_float d1 = 2.75f;

	_float fDuration = fCurTime / fEndTime;

	if (fDuration < 1.f / d1) {
		return fEndValue  * n1 * fDuration * fDuration + fStartValue;
	}
	else if (fDuration < 2.f / d1) {
		return fEndValue  * n1 * (fDuration - 1.5f / d1) * fDuration + 0.75f + fStartValue;
	}
	else if (fDuration < 2.5f / d1) {
		return fEndValue  * n1 * (fDuration - 2.25f / d1) * fDuration + 0.9375f + fStartValue;
	}
	else {
		return fEndValue  * n1 * (fDuration - 2.625f / d1) * fDuration + 0.984375f + fStartValue;
	}
}

HRESULT CSpinHole::SetUp_Component()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer*/
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_EffectRect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	/* Com_Transform */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Diffuse */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Diffuse"), TEXT("Com_Diffuse"), (CComponent**)&m_pDiffuseMapCom)))
		return E_FAIL;

	/* Com_Alpha */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_Texture_Effect_Alpha"), TEXT("Com_Alpha"), (CComponent**)&m_pAlphaMapCom)))
		return E_FAIL;

	/* Com_Option */
	if (FAILED(__super::SetUp_Component(SCENE_STATIC, TEXT("Prototype_Component_EffectOption"), TEXT("Com_Option"), (CComponent**)&m_pOptionCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpinHole::SetUp_ConstantTable()
{
	if (nullptr == m_pVIBuffer ||
		nullptr == m_pAlphaMapCom ||
		nullptr == m_pDiffuseMapCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4	WorldMatrix, ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrixTranspose());
	XMStoreFloat4x4(&ViewMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ)));

	m_pVIBuffer->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));

	m_pDiffuseMapCom->SetUp_OnShader("g_DiffuseTexture", m_pVIBuffer, m_tRectInfo.iDiffuseNum);


	m_pAlphaMapCom->SetUp_OnShader("g_AlphaMapTexture", m_pVIBuffer, m_tRectInfo.iAlphaNum);

	m_pVIBuffer->Set_RawValue("g_vColor", &m_tRectInfo.C_vColor, sizeof(_float3));
	m_pVIBuffer->Set_RawValue("g_vEndColor", &m_tRectInfo.C_vEndColor, sizeof(_float3));
	m_pVIBuffer->Set_RawValue("g_isFadeIn", &m_tRectInfo.FI_Chack, sizeof(_bool));
	m_pVIBuffer->Set_RawValue("g_isFadeOut", &m_tRectInfo.FO_Chack, sizeof(_bool));
	m_pVIBuffer->Set_RawValue("g_isUVSprite", &m_tRectInfo.US_Chack, sizeof(_bool));
	m_pVIBuffer->Set_RawValue("g_isUVAnimation", &m_tRectInfo.UA_Chack, sizeof(_bool));

	if (m_tRectInfo.FI_Chack)
	{
		_float	FI_fStartAcc = static_cast<_float>(m_FI_TimeAcc) - m_tRectInfo.FI_fStartTime;
		_float	FI_fFadeInDelay = m_tRectInfo.FI_fEndTime - m_tRectInfo.FI_fStartTime;
		_float	FI_fFadeInPower = FI_fStartAcc / FI_fFadeInDelay;

		m_pVIBuffer->Set_RawValue("g_fFadeInPower", &FI_fFadeInPower, sizeof(_float));
	}

	if (m_tRectInfo.FO_Chack && !m_tRectInfo.FI_Chack)
	{
		_float	FO_fStartAcc = static_cast<_float>(m_FO_TimeAcc) - m_tRectInfo.FO_fStartTime;
		_float	FO_fFadeInDelay = m_tRectInfo.FO_fEndTime - m_tRectInfo.FO_fStartTime;

		_float	FO_fFadeInPower = 1.f - (FO_fStartAcc / FO_fFadeInDelay);
		if (0.f >= FO_fFadeInPower)
			FO_fFadeInPower = 0.f;

		m_pVIBuffer->Set_RawValue("g_fFadeOutPower", &FO_fFadeInPower, sizeof(_float));
	}

	if (m_tRectInfo.UA_Chack)
	{
		m_pVIBuffer->Set_RawValue("g_fUVAnimX", &m_UA_UVAccX, sizeof(_float));
		m_pVIBuffer->Set_RawValue("g_fUVAnimY", &m_UA_UVAccY, sizeof(_float));
	}
	else
	{
		m_UA_UVAccX = 0.f;
		m_UA_UVAccY = 0.f;
	}

	if (m_tRectInfo.US_Chack)
	{
		if (m_US_fCurIndexX >= m_tRectInfo.US_MaxNumX)
		{
			m_US_fCurIndexX = 0;
			m_US_fCurIndexY++;
			if (m_US_fCurIndexY >= m_tRectInfo.US_MaxNumY)
			{
				m_US_fCurIndexX = 0;
				m_US_fCurIndexY = 0;
			}
		}

		m_pVIBuffer->Set_RawValue("g_fImgMaxCountX", &m_tRectInfo.US_MaxNumX, sizeof(_int));
		m_pVIBuffer->Set_RawValue("g_fImgMaxCountY", &m_tRectInfo.US_MaxNumY, sizeof(_int));
		m_pVIBuffer->Set_RawValue("g_fCurIndexX", &m_US_fCurIndexX, sizeof(_int));
		m_pVIBuffer->Set_RawValue("g_fCurIndexY", &m_US_fCurIndexY, sizeof(_int));
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CSpinHole * CSpinHole::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSpinHole*	pInstance = new CSpinHole(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created : CSpinHole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSpinHole::Clone(void * pArg, _uint iCurrentScene)
{
	CSpinHole*	pInstance = new CSpinHole(*this);

	if (FAILED(pInstance->NativeConstruct(pArg, iCurrentScene)))
	{
		MSGBOX("Failed to Created : CSpinHole");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpinHole::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pAlphaMapCom);
	Safe_Release(m_pDiffuseMapCom);
	Safe_Release(m_pOptionCom);
}
