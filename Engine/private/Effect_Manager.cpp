#include "..\public\Effect_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Transform.h"
#include "Model.h"
#include "Effect_Option.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	:m_pObject_Manager(CObject_Manager::Get_Instance()),
	m_pComponent_Manager(CComponent_Manager::Get_Instance())
{
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
}

HRESULT CEffect_Manager::Create_Prototype_Effect_Data(EFFECT_TYPE eType, _uint iPrototypeEffectSceneIndex, const _tchar* pEffectPath, const _tchar* pEffectTag)
{
	HANDLE hFile = CreateFile(pEffectPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;
	_ulong dwStrByte = 0;

	while (true)
	{
		if (TYPE_RECT == eType)
		{
			EFFECTRECTINFO*	tRectInfo = new EFFECTRECTINFO;

			/* Diffuse Num */
			ReadFile(hFile, &(tRectInfo->iDiffuseNum), sizeof(_uint), &dwByte, nullptr);

			/* Alpha Num */
			ReadFile(hFile, &(tRectInfo->iAlphaNum), sizeof(_uint), &dwByte, nullptr);

			/* 상세 데이터 값 */
			ReadFile(hFile, &(tRectInfo->vPos), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->vScale), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->vRot), sizeof(_float3), &dwByte, nullptr);

			/* 첫 월드 메트릭스 */
			ReadFile(hFile, &(tRectInfo->WorldMatrix), sizeof(_float4x4), &dwByte, nullptr);

			/* 쉐이더 패스 */
			ReadFile(hFile, &(tRectInfo->iPass), sizeof(_uint), &dwByte, nullptr);

			/* 색상 값 */
			ReadFile(hFile, &(tRectInfo->C_vColor), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->C_vEndColor), sizeof(_float3), &dwByte, nullptr);

			/* 체크 확인 */
			ReadFile(hFile, &(tRectInfo->PP_EffectChackX), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_EffectChackY), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_EffectChackZ), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);

			ReadFile(hFile, &(tRectInfo->PS_EffectChackX), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_EffectChackY), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_EffectChackZ), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);

			ReadFile(hFile, &(tRectInfo->PR_EffectChackX), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_EffectChackY), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_EffectChackZ), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);

			/* 할성화된 패턴 체크 */
			ReadFile(hFile, &(tRectInfo->PP_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FI_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_Chack), sizeof(_bool), &dwByte, nullptr);

			/* 빌보드 체크 */
			ReadFile(hFile, &(tRectInfo->I_isBillboardY), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->I_isBillboard), sizeof(_bool), &dwByte, nullptr);

			/* 패턴 */
			ReadFile(hFile, &(tRectInfo->PP_vEnd), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_vEnd), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_vEnd), sizeof(_float3), &dwByte, nullptr);

			/* 시간 */
			ReadFile(hFile, &(tRectInfo->CDT_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->CDT_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FI_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FI_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_fEndTime), sizeof(_float), &dwByte, nullptr);

			/* 반복 */
			ReadFile(hFile, &(tRectInfo->PP_Refeat), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_Refeat), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_Refeat), sizeof(_float), &dwByte, nullptr);

			/* 지속시간 */
			ReadFile(hFile, &(tRectInfo->FI_Degree), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_Degree), sizeof(_float), &dwByte, nullptr);

			/* 스프라이트 */
			ReadFile(hFile, &(tRectInfo->US_MaxNumX), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_MaxNumY), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_StarUVX), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_StarUVY), sizeof(_uint), &dwByte, nullptr);

			/* UV 애니매이션 */
			ReadFile(hFile, &(tRectInfo->UA_AniX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_AniY), sizeof(_float), &dwByte, nullptr);


			if (0 == dwByte)
			{
				Safe_Delete(tRectInfo);
				break;
			}

			if (FAILED(m_pObject_Manager->Create_EffectRectData(iPrototypeEffectSceneIndex, pEffectTag, tRectInfo)))
			{
				return E_FAIL;
			}
		}
		else if (TYPE_TOOL_RECT == eType)
		{
			EFFECTRECTINFO*	tRectInfo = new EFFECTRECTINFO;

			/* Diffuse Num */
			ReadFile(hFile, &(tRectInfo->iDiffuseNum), sizeof(_uint), &dwByte, nullptr);

			/* Alpha Num */
			ReadFile(hFile, &(tRectInfo->iAlphaNum), sizeof(_uint), &dwByte, nullptr);

			/* 상세 데이터 값 */
			ReadFile(hFile, &(tRectInfo->vPos), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->vScale), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->vRot), sizeof(_float3), &dwByte, nullptr);

			/* 첫 월드 메트릭스 */
			ReadFile(hFile, &(tRectInfo->WorldMatrix), sizeof(_float4x4), &dwByte, nullptr);

			/* 쉐이더 패스 */
			ReadFile(hFile, &(tRectInfo->iPass), sizeof(_uint), &dwByte, nullptr);

			/* 색상 값 */
			ReadFile(hFile, &(tRectInfo->C_vColor), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->C_vEndColor), sizeof(_float3), &dwByte, nullptr);

			/* 체크 확인 */
			ReadFile(hFile, &(tRectInfo->PP_EffectChackX), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_EffectChackY), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_EffectChackZ), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);

			ReadFile(hFile, &(tRectInfo->PS_EffectChackX), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_EffectChackY), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_EffectChackZ), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);

			ReadFile(hFile, &(tRectInfo->PR_EffectChackX), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_EffectChackY), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_EffectChackZ), sizeof(_bool) * EFFECTRECTINFO::OPTION_END, &dwByte, nullptr);

			/* 할성화된 패턴 체크 */
			ReadFile(hFile, &(tRectInfo->PP_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FI_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_Chack), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_Chack), sizeof(_bool), &dwByte, nullptr);

			/* 빌보드 체크 */
			ReadFile(hFile, &(tRectInfo->I_isBillboardY), sizeof(_bool), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->I_isBillboard), sizeof(_bool), &dwByte, nullptr);

			/* 패턴 */
			ReadFile(hFile, &(tRectInfo->PP_vEnd), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_vEnd), sizeof(_float3), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_vEnd), sizeof(_float3), &dwByte, nullptr);

			/* 시간 */
			ReadFile(hFile, &(tRectInfo->CDT_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->CDT_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PP_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FI_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FI_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_fEndTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_fStartTime), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_fEndTime), sizeof(_float), &dwByte, nullptr);

			/* 반복 */
			ReadFile(hFile, &(tRectInfo->PP_Refeat), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PS_Refeat), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->PR_Refeat), sizeof(_float), &dwByte, nullptr);

			/* 지속시간 */
			ReadFile(hFile, &(tRectInfo->FI_Degree), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->FO_Degree), sizeof(_float), &dwByte, nullptr);

			/* 스프라이트 */
			ReadFile(hFile, &(tRectInfo->US_MaxNumX), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_MaxNumY), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_StarUVX), sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->US_StarUVY), sizeof(_uint), &dwByte, nullptr);

			/* UV 애니매이션 */
			ReadFile(hFile, &(tRectInfo->UA_AniX), sizeof(_float), &dwByte, nullptr);
			ReadFile(hFile, &(tRectInfo->UA_AniY), sizeof(_float), &dwByte, nullptr);


			if (0 == dwByte)
			{
				Safe_Delete(tRectInfo);
				break;
			}

			_tchar* pTag = new _tchar[MAX_PATH];
			lstrcpy(pTag, pEffectTag);
			m_listEffectTag.emplace_back(pTag);

			if (FAILED(m_pObject_Manager->Create_EffectRectData(iPrototypeEffectSceneIndex, pTag, tRectInfo)))
			{
				return E_FAIL;
			}
		}
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Manager::Create_Prototype_Effect_Particle_Data(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iPrototypeEffectSceneIndex, const _tchar * pEffectPath, const _tchar * pEffectTag)
{
	HANDLE hFile = CreateFile(pEffectPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;
	_ulong dwStrByte = 0;

	while (true)
	{
		EFFECTPARTICLEINFO*	tParticleInfo = new EFFECTPARTICLEINFO;

		/* Alpha */
		ReadFile(hFile, &(tParticleInfo->isPoint), sizeof(_bool), &dwByte, nullptr);

		/* Alpha */
		ReadFile(hFile, &(tParticleInfo->isDistAlpha), sizeof(_bool), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->isRandomUV), sizeof(_bool), &dwByte, nullptr);

		/* Option */
		ReadFile(hFile, &(tParticleInfo->iPrototype), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->iOption), sizeof(_uint), &dwByte, nullptr);

		/* Pass */
		ReadFile(hFile, &(tParticleInfo->iPass), sizeof(_uint), &dwByte, nullptr);

		/* Tex */
		ReadFile(hFile, &(tParticleInfo->iTexNum), sizeof(_uint), &dwByte, nullptr);

		/* Option */
		ReadFile(hFile, &(tParticleInfo->iOptionX), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->iOptionY), sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->iOptionZ), sizeof(_uint), &dwByte, nullptr);

		/* LifeTime */
		ReadFile(hFile, &(tParticleInfo->fLifeTime), sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->fCreateTime), sizeof(_float), &dwByte, nullptr);

		/* UV */
		ReadFile(hFile, &(tParticleInfo->fUVDelay), sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->vMaxUV), sizeof(_float2), &dwByte, nullptr);

		/* Size */
		ReadFile(hFile, &(tParticleInfo->vSize), sizeof(_float2), &dwByte, nullptr);

		/* Color */
		ReadFile(hFile, &(tParticleInfo->vColor), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->vEndColor), sizeof(_float3), &dwByte, nullptr);

		/* Dir */
		ReadFile(hFile, &(tParticleInfo->vMinDir), sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &(tParticleInfo->vMaxDir), sizeof(_float3), &dwByte, nullptr);

		/* Pos */
		ReadFile(hFile, &(tParticleInfo->vMaxPos), sizeof(_float3), &dwByte, nullptr);


		if (0 == dwByte)
		{
			Safe_Delete(tParticleInfo);
			break;
		}

		if (FAILED(m_pObject_Manager->Create_EffectParticleData(iPrototypeEffectSceneIndex, pEffectTag, tParticleInfo)))
		{
			return E_FAIL;
		}

		_tchar* pTag = new _tchar[MAX_PATH];
		wstring wstPrototypeTag = TEXT("Prototype_Component_");
		wstPrototypeTag += pEffectTag;
		lstrcpy(pTag, wstPrototypeTag.c_str());
		m_listEffectTag.emplace_back(pTag);

		if (tParticleInfo->isPoint)
		{
			switch (tParticleInfo->iPrototype)
			{
			case 0:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_PointInstance::Create(pDevice, pDeviceContext, 50))))
					return E_FAIL;
				break;
			case 1:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_PointInstance::Create(pDevice, pDeviceContext, 100))))
					return E_FAIL;
				break;
			case 2:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_PointInstance::Create(pDevice, pDeviceContext, 300))))
					return E_FAIL;
				break;
			case 3:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_PointInstance::Create(pDevice, pDeviceContext, 400))))
					return E_FAIL;
				break;
			case 4:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_PointInstance::Create(pDevice, pDeviceContext, 500))))
					return E_FAIL;
				break;
			}
		}
		else
		{

			switch (tParticleInfo->iPrototype)
			{
			case 0:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_RectInstance::Create(pDevice, pDeviceContext, 10))))
					return E_FAIL;
					break;
			case 1:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_RectInstance::Create(pDevice, pDeviceContext, 20))))
					return E_FAIL;
				break;
			case 2:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_RectInstance::Create(pDevice, pDeviceContext, 30))))
					return E_FAIL;
				break;
			case 3:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_RectInstance::Create(pDevice, pDeviceContext, 40))))
					return E_FAIL;
				break;
			case 4:
				if (FAILED(m_pComponent_Manager->Add_Prototype(iPrototypeEffectSceneIndex, pTag, CVIBuffer_RectInstance::Create(pDevice, pDeviceContext, 50))))
					return E_FAIL;
				break;
			}
		}
	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Manager::Effect_Call_Inheritance(_uint iLayerSceneIndex, _bool isOnce, _bool isNonBillboard, class CTransform* pTransform, const _tchar * pLayerTag)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, TEXT("Com_Option"), i);

			iIndex++;
			if (nullptr == pOption)
				continue;

			if (nullptr == pOption->Get_ParentTransform())
				pOption->Set_ParentTransform(isOnce, isNonBillboard ,pTransform);
		}
	}

	return S_OK;
}

HRESULT CEffect_Manager::Effect_Call_WorldPos(_uint iLayerSceneIndex, const _tchar * pLayerTag, _vector vPos, _bool isWorldPosEnable)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, TEXT("Com_Option"), i);

			iIndex++;
			if (nullptr == pOption)
				continue;

			if (!pOption->Get_SetPos())
			{
				pOption->Set_SetPos();
				pOption->Set_MovePos(vPos);
				pOption->Set_WorldPosEnable(isWorldPosEnable);
			}
		}
	}

	return S_OK;
}

HRESULT CEffect_Manager::Effect_ParentBoneMatrix(_uint iLayerSceneIndex, const _tchar* pLayerTag, CTransform* pTransform, CModel * pModel, const char* pBoneName)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, TEXT("Com_Option"), i);

			iIndex++;
			if (nullptr == pOption)
				continue;

			if (nullptr == pOption->Get_ParentModel())
				pOption->Set_ParentModel(pModel);

			if (nullptr == pOption->Get_ParentTransform())
				pOption->Set_ParentTransform(true, true, pTransform);

			if (nullptr == pOption->Get_CombinedMatrix())
				pOption->Setting_BoneMatrix(pBoneName);
		}
	}

	return S_OK;
}

void CEffect_Manager::Setting_Effect_Speed(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _double EffectSpeed)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pEffectOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, pComTag, iIndex);

			iIndex++;
			if (nullptr == pEffectOption)
				continue;

			pEffectOption->Set_EffectSpeed(EffectSpeed);
		}
	}
}

void CEffect_Manager::Setting_Effect_UVSprite(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar* pComTag, _double SprtieTime)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pEffectOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, pComTag, iIndex);

			iIndex++;
			if (nullptr == pEffectOption)
				continue;

			pEffectOption->Set_UVSpriteTime(SprtieTime);
		}
	}
}

void CEffect_Manager::Setting_Effect_MovePos(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _vector vMovePos)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pEffectOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, pComTag, iIndex);

			iIndex++;
			if (nullptr == pEffectOption)
				continue;

			pEffectOption->Set_MovePos(vMovePos);
			
		}
	}
}

void CEffect_Manager::Setting_Effect_TransformPos(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _uint iType, _float fScale)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pEffectOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, pComTag, iIndex);

			iIndex++;
			if (nullptr == pEffectOption)
				continue;

			pEffectOption->Set_SetTransformPos();
			pEffectOption->Set_TransformPosScale(fScale);
			pEffectOption->Set_TransformPosType(iType);

		}
	}
}

void CEffect_Manager::Setting_Effect_Scale(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _vector vScale)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pEffectOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, pComTag, iIndex);

			iIndex++;
			if (nullptr == pEffectOption)
				continue;

			if(!pEffectOption->Get_SetScale())
				pEffectOption->Set_Scale(vScale);

			pEffectOption->Set_SetScale();
		}
	}
}

void CEffect_Manager::Setting_Effect_NonRotation(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _bool isNonRotation)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pEffectOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, pComTag, iIndex);

			iIndex++;
			if (nullptr == pEffectOption)
				continue;

			pEffectOption->Set_NonRotation(isNonRotation);
		}
	}
}

void CEffect_Manager::Setting_Effect_LocalBillboard(_uint iLayerSceneIndex, const _tchar * pLayerTag, const _tchar * pComTag, _bool isLocalBillboard)
{
	_uint iSize = m_pObject_Manager->Get_LayerSize(iLayerSceneIndex, pLayerTag);
	if (0 < iSize)
	{
		_uint iIndex = 0;
		for (_uint i = 0; i < iSize; ++i)
		{
			CEffect_Option* pEffectOption = (CEffect_Option*)m_pObject_Manager->Get_Components_Effect(iLayerSceneIndex, pLayerTag, pComTag, iIndex);

			iIndex++;
			if (nullptr == pEffectOption)
				continue;

			pEffectOption->Set_LocalBillboard();
		}
	}
}

void CEffect_Manager::Free()
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);

	for (auto& iter : m_listEffectTag)
		Safe_Delete(iter);
	m_listEffectTag.clear();
}
