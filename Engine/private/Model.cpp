#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "AnimMeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "AnimBone.h"
#include "pathcch.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_pEffect(rhs.m_pEffect)
	, m_Passes(rhs.m_Passes)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_Materials(rhs.m_Materials)
	, m_HierarchyNodes(rhs.m_HierarchyNodes)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimation(rhs.m_iCurrentAnimation)
	, m_eMeshType(rhs.m_eMeshType)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
{
	for (auto& pMeshContainer_Prototype : rhs.m_MeshContainers)
	{
		m_MeshContainers.push_back((CVIBuffer*)pMeshContainer_Prototype->Clone(nullptr));
	}
	Safe_AddRef(m_pEffect);

	strcpy_s(m_szFilePathSpec, rhs.m_szFilePathSpec);

	for (auto& pPass : m_Passes)
	{
		Safe_AddRef(pPass.pInputLayout);
		Safe_AddRef(pPass.pPass);
	}
}

_float4x4 CModel::Get_OffsetMatrix(const char * pBoneName)
{
	_float4x4			OffsetMatrix;

	CHierarchyNode*		pNode = Find_HierarchyNodes(pBoneName);

	if (nullptr == pNode)
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

	else
		XMStoreFloat4x4(&OffsetMatrix, pNode->Get_OffsetMatrix());

	return OffsetMatrix;
}

const _float4x4 * CModel::Get_CombinedMatrixPtr(const char * pBoneName)
{
	_float4x4			CombinedMatrix;

	CHierarchyNode*		pNode = Find_HierarchyNodes(pBoneName);
	if (nullptr == pNode)
		return nullptr;

	return pNode->Get_CombinedTransformationMatrixPtr();
}

_vector CModel::Get_MiddlePos()
{
	return (XMLoadFloat3(&m_vMin) + XMLoadFloat3(&m_vMax) / 2.f);

}

HRESULT CModel::Set_Animation(_uint iAnimationIndex)
{
	m_iCurrentAnimation = iAnimationIndex;

	return S_OK;
}

HRESULT CModel::Set_RootAnimation(_uint iAnimationIndex)
{
	m_iRootCurAnim = iAnimationIndex;

	return S_OK;
}

HRESULT CModel::Set_UpperAnimation(_uint iAnimationIndex)
{
	m_iUpperCurAnim = iAnimationIndex;

	return S_OK;
}

HRESULT CModel::Set_BottomAnimation(_uint iAnimationIndex)
{
	m_iBottomCurAnim = iAnimationIndex;

	return S_OK;
}

_uint CModel::Get_AnimationCurrentKeyFrame() const
{
	return m_Animations[m_iCurrentAnimation]->Get_FrameCurrentIndex();
}

void CModel::BoneScailing(char * BoneName, _float _fScale)
{
	CHierarchyNode* pNode = Find_HierarchyNodes(BoneName);

	pNode->Scaling(_fScale);
}

void CModel::Set_NextAnimation()
{
	_uint iNextAnimIndex = ++m_iCurrentAnimation;
	if (m_iNumAnimations <= iNextAnimIndex)
		m_iCurrentAnimation = 0;
	else
	{
		m_iCurrentAnimation = iNextAnimIndex;
	}
}

HRESULT CModel::NativeConstruct_Prototype(char* pModelFilePath, char* pFilePathSpec, _fmatrix PivotMatrix)
{
	char		szFilePath[MAX_PATH] = "";
	char		szPath[MAX_PATH] = "";
	char		szFileName[MAX_PATH] = "";
	char		szEXT[MAX_PATH] = "";
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	DWORD dwByte;
	_uint MeshType = 0;
	_uint iNumMesh = 0;
	_uint iNumVertices = 0;
	_uint iNumIndices = 0;

	strcpy_s(szFilePath, pFilePathSpec);
	strcpy_s(m_szFilePathSpec, pFilePathSpec);
	strcat_s(m_szFilePathSpec, szFileName);
	FILE *fp;		// 객체
	fopen_s(&fp, pModelFilePath, "rb");

	fread(&MeshType, sizeof(_uint), 1, fp);
	fread(&iNumMesh, sizeof(_uint), 1, fp);
	CVIBuffer* pMeshContainer = nullptr;

	if (1 == MeshType)
	{
		m_eMeshType = TYPE_ANIM;
		for (_uint t = 0; t < iNumMesh; ++t)
		{
			fread(&dwByte, sizeof(DWORD), 1, fp);
			fread(&szPath, dwByte, 1, fp);
			fread(&iNumVertices, sizeof(_uint), 1, fp);
			fread(&iNumIndices, sizeof(_uint), 1, fp);
			VTXANIMMODEL* pVTXMODEL = new VTXANIMMODEL[iNumVertices];
			FACEINDICES32* pFaceIndices = new FACEINDICES32[iNumIndices];
			for (_uint i = 0; i < iNumVertices; ++i)
			{
				fread(&pVTXMODEL[i].vPosition, sizeof(_float3), 1, fp);
				fread(&pVTXMODEL[i].vNormal, sizeof(_float3), 1, fp);
				fread(&pVTXMODEL[i].vTexUV, sizeof(_float2), 1, fp);
				fread(&pVTXMODEL[i].vTangent, sizeof(_float3), 1, fp);
				fread(&pVTXMODEL[i].vBlendWeight, sizeof(_float4), 1, fp);
				fread(&pVTXMODEL[i].vBlendIndex, sizeof(_uint4), 1, fp);
				m_vMax.x = max(m_vMax.x, ((VTXMODEL*)pVTXMODEL)[i].vPosition.x);
				m_vMax.y = max(m_vMax.y, ((VTXMODEL*)pVTXMODEL)[i].vPosition.y);
				m_vMax.z = max(m_vMax.z, ((VTXMODEL*)pVTXMODEL)[i].vPosition.z);
				m_vMin.x = min(m_vMin.x, ((VTXMODEL*)pVTXMODEL)[i].vPosition.x);
				m_vMin.y = min(m_vMin.y, ((VTXMODEL*)pVTXMODEL)[i].vPosition.y);
				m_vMin.z = min(m_vMin.z, ((VTXMODEL*)pVTXMODEL)[i].vPosition.z);
			}
			for (_uint j = 0; j < iNumIndices; ++j)
			{
				fread(&pFaceIndices[j], sizeof(FACEINDICES32), 1, fp);
			}
			char szAnimContainerName[MAX_PATH] = "";
			_uint iMaterialIndex = 0;
			fread(&dwByte, sizeof(DWORD), 1, fp);
			fread(szAnimContainerName, dwByte, 1, fp);
			fread(&iMaterialIndex, sizeof(_uint), 1, fp);
			pMeshContainer = CAnimMeshContainer::Create(m_pDevice, m_pDeviceContext, pVTXMODEL, pFaceIndices, iNumVertices, iNumIndices, iMaterialIndex, 0, szAnimContainerName);
			m_MeshContainers.push_back(pMeshContainer);
		}
	}
	else
	{
		m_eMeshType = TYPE_NONANIM;
		if (TYPE_EFFECT == MeshType)
			m_eMeshType = TYPE_EFFECT;

		for (_uint t = 0; t < iNumMesh; ++t)
		{
			char		szMeshContainerName[MAX_PATH] = "";
			fread(&dwByte, sizeof(DWORD), 1, fp);
			fread(&szMeshContainerName, dwByte, 1, fp);
			fread(&iNumVertices, sizeof(_uint), 1, fp);
			fread(&iNumIndices, sizeof(_uint), 1, fp);
			VTXMODEL* pVTXMODEL = new VTXMODEL[iNumVertices];
			_float3* pVerticesPos = new _float3[iNumVertices];
			FACEINDICES32* pFaceIndices = new FACEINDICES32[iNumIndices];
			for (_uint i = 0; i < iNumVertices; ++i)
			{
				fread(&pVTXMODEL[i].vPosition, sizeof(_float3), 1, fp);
				XMStoreFloat3(&pVTXMODEL[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVTXMODEL[i].vPosition), PivotMatrix));
				pVerticesPos[i] = pVTXMODEL[i].vPosition;
				fread(&pVTXMODEL[i].vNormal, sizeof(_float3), 1, fp);
				XMStoreFloat3(&pVTXMODEL[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVTXMODEL[i].vNormal), PivotMatrix));
				fread(&pVTXMODEL[i].vTexUV, sizeof(_float2), 1, fp);
				fread(&pVTXMODEL[i].vTangent, sizeof(_float3), 1, fp);
				XMStoreFloat3(&pVTXMODEL[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVTXMODEL[i].vTangent), PivotMatrix));
				m_vMax.x = max(m_vMax.x, ((VTXMODEL*)pVTXMODEL)[i].vPosition.x);
				m_vMax.y = max(m_vMax.y, ((VTXMODEL*)pVTXMODEL)[i].vPosition.y);
				m_vMax.z = max(m_vMax.z, ((VTXMODEL*)pVTXMODEL)[i].vPosition.z);
				m_vMin.x = min(m_vMin.x, ((VTXMODEL*)pVTXMODEL)[i].vPosition.x);
				m_vMin.y = min(m_vMin.y, ((VTXMODEL*)pVTXMODEL)[i].vPosition.y);
				m_vMin.z = min(m_vMin.z, ((VTXMODEL*)pVTXMODEL)[i].vPosition.z);
			}
			for (_uint j = 0; j < iNumIndices; ++j)
			{
				fread(&pFaceIndices[j], sizeof(FACEINDICES32), 1, fp);
			}
			_uint iMaterialindex = 0;
			fread(&iMaterialindex, sizeof(_uint), 1, fp);
			pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, pVTXMODEL, pFaceIndices, iMaterialindex, iNumVertices, iNumIndices, pVerticesPos, szMeshContainerName);
			m_MeshContainers.push_back(pMeshContainer);
		}
	}

	if (FAILED(Create_Shader()))
	{
		fclose(fp);
		return E_FAIL;
	}

	char		szName[MAX_PATH] = "";
	_uint iNumMaterial = 0;
	_uint iTexture = 0;
	fread(&iNumMaterial, sizeof(_uint), 1, fp);
	for (_uint k = 0; k < iNumMaterial; ++k)
	{
		MESHMATERIAL*	pMeshMaterial = new MESHMATERIAL;
		ZeroMemory(pMeshMaterial, sizeof(MESHMATERIAL));
		for (_uint j = 0; j < TEXTURETYPE_END; ++j)
		{
			fread(&iTexture, sizeof(_uint), 1, fp);
			if (0 == iTexture)
			{
				fread(&dwByte, sizeof(DWORD), 1, fp);
				fread(szName, dwByte, 1, fp);
			}
			else
			{
				char		szTextureFileFullPath[MAX_PATH] = "";
				char        szTextureFile[MAX_PATH] = "";
				strcpy_s(szTextureFileFullPath, szFilePath);
				fread(&dwByte, sizeof(DWORD), 1, fp);
				fread(szTextureFile, dwByte, 1, fp);
				strcat_s(szTextureFileFullPath, szTextureFile);
				_tchar		szTextureFullPath[MAX_PATH] = TEXT("");
				MultiByteToWideChar(CP_ACP, 0, szTextureFileFullPath, strlen(szTextureFileFullPath), szTextureFullPath, MAX_PATH);
				pMeshMaterial->pTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szTextureFullPath);
			}
		}
		m_Materials.push_back(pMeshMaterial);
	}
	fclose(fp);

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	if (TYPE_ANIM == m_eMeshType)
	{
		DWORD dwByte;

		FILE *fp;		// 객체

		strcat_s(m_szFilePathSpec, "AnimMesh.dat");
		fopen_s(&fp, m_szFilePathSpec, "rb");

		_uint iNumBone = 0;
		fread(&iNumBone, sizeof(_uint), 1, fp);
		fread(&m_iNumAnimations, sizeof(_uint), 1, fp);

		for (_uint y = 0; y < iNumBone; ++y)
		{
			char szHierarchyNodeName[MAX_PATH] = "";
			char szParentName[MAX_PATH] = "";
			_float4x4 OffsetMatrix;
			_float4x4 OrgTransformationMatrix;
			fread(&dwByte, sizeof(DWORD), 1, fp);
			fread(szHierarchyNodeName, dwByte, 1, fp);
			fread(&dwByte, sizeof(DWORD), 1, fp);
			fread(szParentName, dwByte, 1, fp);
			fread(&OffsetMatrix, sizeof(_float4x4), 1, fp);
			fread(&OrgTransformationMatrix, sizeof(_float4x4), 1, fp);
			CHierarchyNode*			pHierarchyNode = nullptr;
			if (0 != y)
				pHierarchyNode = CHierarchyNode::Create(szHierarchyNodeName, szParentName, XMLoadFloat4x4(&OffsetMatrix), XMLoadFloat4x4(&OrgTransformationMatrix));
			else
				pHierarchyNode = CHierarchyNode::Create(szHierarchyNodeName, nullptr, XMLoadFloat4x4(&OffsetMatrix), XMLoadFloat4x4(&OrgTransformationMatrix));
			pHierarchyNode->Resize_AnimBones(m_iNumAnimations);
			m_HierarchyNodes.push_back(pHierarchyNode);
		}
		/* For.Division_HierachyNode */

		if (FAILED(Division_HierachyNode(pArg)))
			return E_FAIL;

		for (_uint k = 0; k < m_iNumAnimations; ++k)
		{
			_double TPS, DUR;
			fread(&TPS, sizeof(_double), 1, fp);
			fread(&DUR, sizeof(_double), 1, fp);
			CAnimation* pAnimation = CAnimation::Create(TPS, DUR);
			if (nullptr == pAnimation)
				return E_FAIL;

			/* For.Division_Animaition */
			CAnimation*	pUpperAnimation = nullptr;
			CAnimation*	pRootAnimation = nullptr;
			CAnimation*	pBottomAnimation = nullptr;

			if (true == m_bDivisionCheck)
			{
				pUpperAnimation = CAnimation::Create(TPS, DUR);
				pRootAnimation = CAnimation::Create(TPS, DUR);
				pBottomAnimation = CAnimation::Create(TPS, DUR);

				if (nullptr == pUpperAnimation ||
					nullptr == pRootAnimation ||
					nullptr == pBottomAnimation)
					return E_FAIL;
			}

			_uint iNumAnimBone = 0;
			fread(&iNumAnimBone, sizeof(_uint), 1, fp);
			_uint iNumKeyFrames = 0;
			for (_uint w = 0; w < iNumAnimBone; ++w)
			{
				CAnimBone*	pAnimBone = CAnimBone::Create();
				char szAnimBoneName[MAX_PATH] = "";
				fread(&dwByte, sizeof(DWORD), 1, fp);
				fread(szAnimBoneName, dwByte, 1, fp);
				CHierarchyNode* pHierarchyNode = Find_HierarchyNodes(szAnimBoneName);
				pHierarchyNode->Add_AnimBones(k, pAnimBone);
				if (nullptr == pHierarchyNode)
					return E_FAIL;
				fread(&iNumKeyFrames, sizeof(_uint), 1, fp);
				for (_uint r = 0; r < iNumKeyFrames; ++r)
				{
					KEYFRAME*		pKeyFrame = new KEYFRAME;
					fread(&pKeyFrame->vRotation, sizeof(_float4), 1, fp);
					fread(&pKeyFrame->vPosition, sizeof(_float3), 1, fp);
					fread(&pKeyFrame->vScale, sizeof(_float3), 1, fp);
					fread(&pKeyFrame->Time, sizeof(_double), 1, fp);

					/*For.DivisionCheck_BoneNode_Start*/
					pAnimBone->Set_BoneName(szAnimBoneName);
					/*For.DivisionCheck_BoneNode_End*/

					pAnimBone->Add_KeyFrame(pKeyFrame);
				}
				pAnimation->Add_AnimBones(pAnimBone);

				/*For.Animaition Division_Node_Start*/
				if (true == m_bDivisionCheck)
				{
					for (auto& iter : m_PlayerUppperNodes)
					{
						if (!strcmp(iter->Get_Name(), pAnimBone->Get_szName()))
						{
							pUpperAnimation->Add_AnimBones(pAnimBone);
							Safe_AddRef(pAnimBone);
						}
					}
					for (auto& iter : m_PlayerRootNodes)
					{
						if (!strcmp(iter->Get_Name(), pAnimBone->Get_szName()))
						{
							pRootAnimation->Add_AnimBones(pAnimBone);
							Safe_AddRef(pAnimBone);
						}
					}
					for (auto& iter : m_PlayerBottomNodes)
					{
						if (!strcmp(iter->Get_Name(), pAnimBone->Get_szName()))
						{
							pBottomAnimation->Add_AnimBones(pAnimBone);
							Safe_AddRef(pAnimBone);
						}
					}
				}
				/*For.Animaition Division_Node_End*/
			}

			m_Animations.push_back(pAnimation);

			/*For.Animaition Division_Start*/
			if (true == m_bDivisionCheck)
			{
				m_UpperAnimations.push_back(pUpperAnimation);
				m_RootAnimations.push_back(pRootAnimation);
				m_BottomAnimations.push_back(pBottomAnimation);
			}
			/*For.Animaition Division_End*/
		}

		for (auto& pHierarchyNode1 : m_HierarchyNodes) /* HierarchyNodes_End */
		{
			for (auto& pHierarchyNode2 : m_HierarchyNodes)
			{
				if (!strcmp(pHierarchyNode1->Get_Name(), pHierarchyNode2->Get_ParentName())&&
					(pHierarchyNode1!=pHierarchyNode2))
					pHierarchyNode2->SetUp_Parent(pHierarchyNode1);
			}
		}

		_uint iNumMeshContainer = 0;
		_uint iNumBones = 0;

		fread(&iNumMeshContainer, sizeof(_uint), 1, fp);
		for (_uint z = 0; z < iNumMeshContainer; ++z)
		{
			fread(&iNumBones, sizeof(_uint), 1, fp);
			if (0 == iNumBones)
			{
				((CAnimMeshContainer*)m_MeshContainers[z])->SetUp_MeshBone(this, nullptr);
			}
			else
			{
				for (_uint e = 0; e < iNumBones; ++e)
				{
					char szBoneName[MAX_PATH] = "";
					fread(&dwByte, sizeof(DWORD), 1, fp);
					fread(szBoneName, dwByte, 1, fp);
					CHierarchyNode* pNode = Find_HierarchyNodes(szBoneName);
					if (nullptr != pNode)
						((CAnimMeshContainer*)m_MeshContainers[z])->SetUp_MeshBone(this, Find_HierarchyNodes(szBoneName));
				}
			}
		}
		fclose(fp);
	}
	/*For.MotionBlur*/
	for (_uint i = 0; i < 300; ++i)
	{
		XMStoreFloat4x4(&m_OldBoneMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_OldBoneMatrix2[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_OldBoneMatrix3[i], XMMatrixIdentity());
	}
	return S_OK;
}

const CModel::MODELMESHDATA CModel::Get_MeshColliderDatas()
{
	MODELMESHDATA tDesc;

	_uint iVerticesSize = 0;
	_uint iIndiciesSize = 0;
	for (auto mesh : m_MeshContainers)
	{
		iVerticesSize += mesh->Get_NumPrimitive();
		iIndiciesSize += mesh->Get_PrimitiveIndicesSize();
	}
	_float3* pVerticesPos = new _float3[iVerticesSize];
	FACEINDICES32* pIndicies = new FACEINDICES32[iIndiciesSize];

	for (_uint i = 0; i < m_MeshContainers.size(); ++i)
	{
		for (_uint j = 0; j < iVerticesSize; ++j)
		{
			pVerticesPos[(i*m_MeshContainers.size()) + j] = m_MeshContainers[i]->Get_VerticesPos()[j];
		}

		for (_uint k = 0; k < iVerticesSize / 3; ++k)
		{
			pIndicies[(i*m_MeshContainers.size()) + k] = ((FACEINDICES32*)m_MeshContainers[i]->Get_Indices())[k];
			pIndicies[(i*m_MeshContainers.size()) + k + 1] = ((FACEINDICES32*)m_MeshContainers[i]->Get_Indices())[k + 1];
			pIndicies[(i*m_MeshContainers.size()) + k + 2] = ((FACEINDICES32*)m_MeshContainers[i]->Get_Indices())[k + 2];
		}
	}

	tDesc.pVerticesPos = pVerticesPos;
	tDesc.pIndicies = pIndicies;
	tDesc.iVerticesSize = iVerticesSize;
	tDesc.iIndiciesSize = iIndiciesSize;
	return tDesc;
}

_float CModel::Get_Mesh_Range()
{
	_vector vRange = XMLoadFloat3(&m_vMax) - XMLoadFloat3(&m_vMin);
	_float3 fLength;
	fLength.x = XMVectorGetX(XMVector3Length(vRange));
	fLength.y = XMVectorGetY(XMVector3Length(vRange));
	fLength.z = XMVectorGetZ(XMVector3Length(vRange));
	_float fRange = MaxScaleFromFloat3(fLength);
	return  fRange;
}

HRESULT CModel::Set_RawValue(const char * pConstantName, void * pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

HRESULT CModel::Set_TextureValue(const char * pConstantName, ID3D11ShaderResourceView * pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetResource(pSRV);
}

CHierarchyNode* CModel::Find_HierarchyNodes(const char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNodeName, pNode->Get_Name());
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;
}

HRESULT CModel::Create_Shader()
{
	if (TYPE_NONANIM == m_eMeshType)
	{
		D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_Model.hlsl"), Elements, 4)))
			return E_FAIL;
	}
	else if (TYPE_ANIM == m_eMeshType)
	{
		D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_AnimModel.hlsl"), Elements, 6)))
			return E_FAIL;
	}
	else if (TYPE_EFFECT == m_eMeshType)
	{
		D3D11_INPUT_ELEMENT_DESC		Elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_EffectModel.hlsl"), Elements, 4)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Division_HierachyNode(void* pArg)
{
	if (nullptr != pArg)
	{
		ZeroMemory(&m_tNodeNameDesc, sizeof(NODENAMEDESC));
		m_tNodeNameDesc = *(NODENAMEDESC*)pArg;


		CHierarchyNode* Test = Find_HierarchyNodes(m_tNodeNameDesc.szFinderBoneName);
		if (nullptr != Test)
		{
			_uint iPlayer_BodyIndex = 0;
			m_bDivisionCheck = true;
			for (auto& iter : m_HierarchyNodes)
			{
				if (!strcmp(m_tNodeNameDesc.szBottomBoneName, iter->Get_Name()))
					break;

				if (!strcmp(m_tNodeNameDesc.szRootBoneName, iter->Get_Name()))
					m_PlayerRootNodes.push_back(iter);
				else
					m_PlayerUppperNodes.push_back(iter);

				Safe_AddRef(iter);

				++iPlayer_BodyIndex;
			}
			for (size_t i = iPlayer_BodyIndex; i < m_HierarchyNodes.size(); ++i)
			{
				m_PlayerBottomNodes.push_back(m_HierarchyNodes[i]);
				Safe_AddRef(m_HierarchyNodes[i]);
			}
		}
		else
		{
			MSGBOX("Failed to Finder FinderNoneName");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CModel::Compile_Shader(const _tchar * pShaderFilePath, D3D11_INPUT_ELEMENT_DESC * pElements, _uint iNumElements)
{
	_uint			iFlag = 0;
#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;

#endif // _DEBUG

	ID3DBlob*		pCodes = nullptr;

	/* 셰이더파일을 컴파일하여 메모리에 올리고. */
	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pCodes, nullptr)))
		return E_FAIL;

	/* 올라온 메모리로부터 이펙트 객체를 생성한다. */
	if (FAILED(D3DX11CreateEffectFromMemory(pCodes->GetBufferPointer(), pCodes->GetBufferSize(), 0, m_pDevice, &m_pEffect)))
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	if (FAILED(pTechnique->GetDesc(&TechniqueDesc)))
		return E_FAIL;

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		PASSDESC			PassDesc;
		ZeroMemory(&PassDesc, sizeof(PASSDESC));

		PassDesc.pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC		Pass;
		ZeroMemory(&Pass, sizeof(D3DX11_PASS_DESC));

		PassDesc.pPass->GetDesc(&Pass);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, Pass.pIAInputSignature, Pass.IAInputSignatureSize, &PassDesc.pInputLayout)))
			return E_FAIL;

		m_Passes.push_back(PassDesc);
	}

	return S_OK;
}

HRESULT CModel::Update(_double TimeDelta)
{
	/* 현재 애니메이션 재생값에따라 모든 뼈의 상태행렬을 갱신한다. */
	m_Animations[m_iCurrentAnimation]->Update_AnimBoneTransformationMatrix(TimeDelta);

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimation);
	}

	return S_OK;
}

HRESULT CModel::Update_Finishied(_double TimeDelta)
{
	m_Animations[m_iCurrentAnimation]->Update_AnimBoneTransformationMatrix_Finishied(TimeDelta);

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimation);
	}

	return S_OK;

	return S_OK;
}

HRESULT CModel::Root_Update(_double TimeDelta, _bool bRootLockCheck)
{
	_uint iSelectedAnimation = 0;

	if (m_iRootPreAnimation != m_iRootCurAnim && false == m_bRootIsLefp)
		m_RootAnimations[m_iRootPreAnimation]->Reset_Animation();

	//if (m_iRootPastAnim != m_iRootPreAnimation && true == m_bRootIsLefp)
	//	m_RootAnimations[m_iRootPastAnim]->Reset_Animation();

	iSelectedAnimation = m_iRootCurAnim;

	if (true == m_bRootIsLefp)
	{
		iSelectedAnimation = m_iRootPreAnimation;


		if (true == m_RootAnimations[m_iRootPreAnimation]->Update_AnimBoneTransformationMatrix_Lefp(TimeDelta, m_RootAnimations[m_iRootCurAnim]))
		{
			m_RootAnimations[m_iRootPreAnimation]->Reset_Animation();
			m_RootAnimations[m_iRootCurAnim]->Reset_Animation();
			m_bRootIsLefp = false;
			return S_OK;
		}
		//else
		//	m_iRootPastAnim = m_iRootPreAnimation;

		for (auto& pHierarchy : m_PlayerRootNodes)
			pHierarchy->Update_RootCombinedTransformationMatrix(iSelectedAnimation, bRootLockCheck);
	}
	else
	{
		m_RootAnimations[m_iRootCurAnim]->Update_NextAnimBoneTransformationMatrix(TimeDelta);
		m_iRootPreAnimation = m_iRootCurAnim;

		for (auto& pHierarchy : m_PlayerRootNodes)
			pHierarchy->Update_RootCombinedTransformationMatrix(iSelectedAnimation, bRootLockCheck);

	}
	return S_OK;
}

HRESULT CModel::Upper_Update(_double TimeDelta)
{
	_uint iSelectedAnimation = 0;

	if (m_iUpperPreAnimation != m_iUpperCurAnim && false == m_bUpperIsLefp)
		m_UpperAnimations[m_iUpperPreAnimation]->Reset_Animation();

	//if (m_iUpperPastAnim != m_iUpperPreAnimation && true == m_bUpperIsLefp)
	//	m_RootAnimations[m_iUpperPastAnim]->Reset_Animation();

	iSelectedAnimation = m_iUpperCurAnim;

	if (true == m_bUpperIsLefp)
	{
		iSelectedAnimation = m_iUpperPreAnimation;

		if (true == m_UpperAnimations[m_iUpperPreAnimation]->Update_AnimBoneTransformationMatrix_Lefp(TimeDelta, m_UpperAnimations[m_iUpperCurAnim]))
		{
			m_UpperAnimations[m_iUpperPreAnimation]->Reset_Animation();
			m_UpperAnimations[m_iUpperCurAnim]->Reset_Animation();
			m_bUpperIsLefp = false;
			return S_OK;
		}
		//else
		//	m_iUpperPastAnim = m_iUpperPreAnimation;

		for (auto& pHierarchy : m_PlayerUppperNodes)
		{
			pHierarchy->Update_CombinedTransformationMatrix(iSelectedAnimation);
		}

	}
	else
	{
		m_UpperAnimations[m_iUpperCurAnim]->Update_NextAnimBoneTransformationMatrix(TimeDelta);
		m_iUpperPreAnimation = m_iUpperCurAnim;

		for (auto& pHierarchy : m_PlayerUppperNodes)
		{
			pHierarchy->Update_CombinedTransformationMatrix(iSelectedAnimation);
		}

	}


	return S_OK;
}

HRESULT CModel::Bottom_Update(_double TimeDelta)
{
	_uint iSelectedAnimation = 0;

	if (m_iBottomPreAnimation != m_iBottomCurAnim && false == m_bBottomIsLefp)
		m_BottomAnimations[m_iBottomPreAnimation]->Reset_Animation();

	//if (m_iBottomPastAnim != m_iBottomPreAnimation && true == m_bBottomIsLefp)
	//	m_RootAnimations[m_iBottomPastAnim]->Reset_Animation();

	iSelectedAnimation = m_iBottomCurAnim;

	if (true == m_bBottomIsLefp)
	{
		iSelectedAnimation = m_iBottomPreAnimation;

		if (true == m_BottomAnimations[m_iBottomPreAnimation]->Update_AnimBoneTransformationMatrix_Lefp(TimeDelta, m_BottomAnimations[m_iBottomCurAnim]))
		{
			m_BottomAnimations[m_iBottomPreAnimation]->Reset_Animation();
			m_BottomAnimations[m_iBottomCurAnim]->Reset_Animation();
			m_bBottomIsLefp = false;
			return S_OK;
		}
		//else
		//	m_iBottomPastAnim = m_iBottomPreAnimation;

		for (auto& pHierarchy : m_PlayerBottomNodes)
		{
			pHierarchy->Update_CombinedTransformationMatrix(iSelectedAnimation);
		}
	}
	else
	{
		m_BottomAnimations[m_iBottomCurAnim]->Update_NextAnimBoneTransformationMatrix(TimeDelta);
		m_iBottomPreAnimation = m_iBottomCurAnim;

		for (auto& pHierarchy : m_PlayerBottomNodes)
		{
			pHierarchy->Update_CombinedTransformationMatrix(iSelectedAnimation);
		}

	}

	return S_OK;
}

HRESULT CModel::Lerp_Update(_double TimeDelta)
{
	_uint iSelectedAnimation = 0;

	if (m_iPreAnimation != m_iCurrentAnimation && false == m_bAnimLefp)
		m_Animations[m_iPreAnimation]->Reset_Animation();

	iSelectedAnimation = m_iCurrentAnimation;

	if (true == m_bAnimLefp)
	{
		iSelectedAnimation = m_iPreAnimation;


		if (true == m_Animations[m_iPreAnimation]->Update_AnimBoneTransformationMatrix_Lefp(TimeDelta, m_Animations[m_iCurrentAnimation]))
		{
			m_Animations[m_iPreAnimation]->Reset_Animation();
			m_Animations[m_iCurrentAnimation]->Reset_Animation();
			m_bAnimLefp = false;
			return S_OK;
		}
		for (auto& pHierarchy : m_HierarchyNodes)
		{
			pHierarchy->Update_CombinedTransformationMatrix(iSelectedAnimation);
		}
	}
	else
	{
		m_Animations[m_iCurrentAnimation]->Update_NextAnimBoneTransformationMatrix(TimeDelta);
		m_iPreAnimation = m_iCurrentAnimation;

		for (auto& pHierarchy : m_HierarchyNodes)
		{
			pHierarchy->Update_CombinedTransformationMatrix(iSelectedAnimation);
		}

	}

	return S_OK;
}

HRESULT CModel::SetKeyFrame_Update(_double TimeDelta, _uint iCurKeyFrameIndex, _bool * bGetTheFinishied)
{
	if (m_iPreAnimation != m_iCurrentAnimation)
		m_Animations[m_iPreAnimation]->Reset_Animation();

	m_Animations[m_iCurrentAnimation]->Set_KeyFrameUpdate(TimeDelta, iCurKeyFrameIndex, bGetTheFinishied);

	m_iPreAnimation = m_iCurrentAnimation;

	for (auto& pHierarchy : m_HierarchyNodes)
		pHierarchy->Update_CombinedTransformationMatrix(m_iCurrentAnimation);

	return S_OK;
}

HRESULT CModel::Render(_uint iPassIndex)
{
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);


	for (auto& pMeshContainer : m_MeshContainers)
	{
		_uint iMaterialIndex = 0;

		if (TYPE_ANIM == m_eMeshType)
		{
			_float4x4			BoneMatrices[300];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 300);

			((CAnimMeshContainer*)pMeshContainer)->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			iMaterialIndex = ((CAnimMeshContainer*)pMeshContainer)->Get_MaterialIndex();

			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 300)))
				return E_FAIL;
			if (FAILED(Set_RawValue("g_OldBoneMatrices", m_OldBoneMatrix3, sizeof(_float4x4) * 300)))
				return E_FAIL;



			memcpy(&m_OldBoneMatrix, &BoneMatrices, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix2, &m_OldBoneMatrix, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix3, &m_OldBoneMatrix2, sizeof(_float4x4) * 300);

		}


		else
		{
			iMaterialIndex = ((CMeshContainer*)pMeshContainer)->Get_MaterialIndex();
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE])
		{
			if (FAILED(Set_TextureValue("g_DiffuseTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS])
		{
			if (FAILED(Set_TextureValue("g_NormalTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM])
		{
			if (FAILED(Set_TextureValue("g_RomTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM]->Get_SRV())))
				return E_FAIL;
		}
		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE])
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE]->Get_SRV())))
				return E_FAIL;
		}
		m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);

		pMeshContainer->Render(iPassIndex);


	}


	return S_OK;
}

HRESULT CModel::Render(_uint iPassIndex, _uint iMaterialCullIndex)
{
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);

	auto iter = m_MeshContainers.begin();
	for (_uint i = 0; i < iMaterialCullIndex; ++i)
	{
		_uint iMaterialIndex = 0;

		if (TYPE_ANIM == m_eMeshType)
		{
			_float4x4			BoneMatrices[300];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 300);

			((CAnimMeshContainer*)*iter)->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			iMaterialIndex = ((CAnimMeshContainer*)*iter)->Get_MaterialIndex();

			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 300)))
				return E_FAIL;
			if (FAILED(Set_RawValue("g_OldBoneMatrices", m_OldBoneMatrix3, sizeof(_float4x4) * 300)))
				return E_FAIL;



			memcpy(&m_OldBoneMatrix, &BoneMatrices, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix2, &m_OldBoneMatrix, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix3, &m_OldBoneMatrix2, sizeof(_float4x4) * 300);

		}


		else
		{
			iMaterialIndex = ((CMeshContainer*)*iter)->Get_MaterialIndex();
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE])
		{
			if (FAILED(Set_TextureValue("g_DiffuseTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS])
		{
			if (FAILED(Set_TextureValue("g_NormalTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM])
		{
			if (FAILED(Set_TextureValue("g_RomTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM]->Get_SRV())))
				return E_FAIL;
		}
		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE])
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE]->Get_SRV())))
				return E_FAIL;
		}
		m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);

		(*iter)->Render(iPassIndex);

		++iter;
	}


	return S_OK;
}

HRESULT CModel::Render_Custom(_uint * ArrRenderPass, _uint ArrSize)
{
	if (nullptr == ArrRenderPass)
		return E_FAIL;



	_uint iCount = 0;
	_uint iRenderPass = 0;
	for (auto& pMeshContainer : m_MeshContainers)
	{
		if (iCount > ArrSize)
			iRenderPass = 0;
		else
			iRenderPass = ArrRenderPass[iCount];

		m_pDeviceContext->IASetInputLayout(m_Passes[iRenderPass].pInputLayout);
		_uint iMaterialIndex = 0;

		if (TYPE_ANIM == m_eMeshType)
		{
			_float4x4			BoneMatrices[300];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 300);

			((CAnimMeshContainer*)pMeshContainer)->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			iMaterialIndex = ((CAnimMeshContainer*)pMeshContainer)->Get_MaterialIndex();

			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 300)))
				return E_FAIL;
			if (FAILED(Set_RawValue("g_OldBoneMatrices", m_OldBoneMatrix3, sizeof(_float4x4) * 300)))
				return E_FAIL;
			memcpy(&m_OldBoneMatrix, &BoneMatrices, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix2, &m_OldBoneMatrix, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix3, &m_OldBoneMatrix2, sizeof(_float4x4) * 300);
		}
		else
		{
			iMaterialIndex = ((CMeshContainer*)pMeshContainer)->Get_MaterialIndex();
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE])
		{
			if (FAILED(Set_TextureValue("g_DiffuseTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS])
		{
			if (FAILED(Set_TextureValue("g_NormalTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM])
		{
			if (FAILED(Set_TextureValue("g_RomTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM]->Get_SRV())))
				return E_FAIL;
		}
		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE])
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE]->Get_SRV())))
				return E_FAIL;
		}
		m_Passes[iRenderPass].pPass->Apply(0, m_pDeviceContext);

		pMeshContainer->Render(iRenderPass);

		++iCount;
	}


	return S_OK;
}

HRESULT CModel::Render_Custom_Light(_uint * ArrRenderPass, _uint ArrSize, _bool* pEmissiveActive)
{
	if (nullptr == ArrRenderPass)
		return E_FAIL;

	_uint iCount = 0;
	_uint iRenderPass = 0;
	for (auto& pMeshContainer : m_MeshContainers)
	{
		if (iCount > ArrSize)
			iRenderPass = 0;
		else
			iRenderPass = ArrRenderPass[iCount];

		m_pDeviceContext->IASetInputLayout(m_Passes[iRenderPass].pInputLayout);
		_uint iMaterialIndex = 0;

		if (TYPE_ANIM == m_eMeshType)
		{
			_float4x4			BoneMatrices[300];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 300);

			((CAnimMeshContainer*)pMeshContainer)->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			iMaterialIndex = ((CAnimMeshContainer*)pMeshContainer)->Get_MaterialIndex();

			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 300)))
				return E_FAIL;
			if (FAILED(Set_RawValue("g_OldBoneMatrices", m_OldBoneMatrix3, sizeof(_float4x4) * 300)))
				return E_FAIL;
			memcpy(&m_OldBoneMatrix, &BoneMatrices, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix2, &m_OldBoneMatrix, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix3, &m_OldBoneMatrix2, sizeof(_float4x4) * 300);
		}
		else
		{
			iMaterialIndex = ((CMeshContainer*)pMeshContainer)->Get_MaterialIndex();
		}
		
		_bool isEmissiveActive = pEmissiveActive[iCount];
		if (FAILED(Set_RawValue("g_bEmissive", &isEmissiveActive, sizeof(_bool))))
			return E_FAIL;

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE])
		{
			if (FAILED(Set_TextureValue("g_DiffuseTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS])
		{
			if (FAILED(Set_TextureValue("g_NormalTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM])
		{
			if (FAILED(Set_TextureValue("g_RomTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM]->Get_SRV())))
				return E_FAIL;
		}
		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE])
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE]->Get_SRV())))
				return E_FAIL;
		}

		m_Passes[iRenderPass].pPass->Apply(0, m_pDeviceContext);

		pMeshContainer->Render(iRenderPass);

		++iCount;
	}


	return S_OK;
}

HRESULT CModel::Render_IsMaterialMapping(_uint iPassIndex, _bool * ArrMaterial)
{
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);


	for (auto& pMeshContainer : m_MeshContainers)
	{
		_uint iMaterialIndex = 0;

		if (TYPE_ANIM == m_eMeshType)
		{
			_float4x4			BoneMatrices[300];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 300);

			((CAnimMeshContainer*)pMeshContainer)->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			iMaterialIndex = ((CAnimMeshContainer*)pMeshContainer)->Get_MaterialIndex();

			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 300)))
				return E_FAIL;
			if (FAILED(Set_RawValue("g_OldBoneMatrices", m_OldBoneMatrix3, sizeof(_float4x4) * 300)))
				return E_FAIL;



			memcpy(&m_OldBoneMatrix, &BoneMatrices, sizeof(_float4x4) * 300);
		}

		else
		{
			iMaterialIndex = ((CMeshContainer*)pMeshContainer)->Get_MaterialIndex();
		}

		if (true == ArrMaterial[0] && nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE])
		{
			if (FAILED(Set_TextureValue("g_DiffuseTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE]->Get_SRV())))
				return E_FAIL;
		}

		if (true == ArrMaterial[1] && nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS])
		{
			if (FAILED(Set_TextureValue("g_NormalTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS]->Get_SRV())))
				return E_FAIL;
		}

		if (true == ArrMaterial[2] && nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM])
		{
			if (FAILED(Set_TextureValue("g_RomTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM]->Get_SRV())))
				return E_FAIL;
		}
		if (true == ArrMaterial[3] && nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE])
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE]->Get_SRV())))
				return E_FAIL;
		}
		m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);

		pMeshContainer->Render(iPassIndex);

		if (FAILED(Set_TextureValue("g_EmissiveTexture", nullptr)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Render_IsEmissiveMapping(_uint iPassIndex, _bool * ArrEmissive)
{
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);

	_uint iCount = 0;

	for (auto& pMeshContainer : m_MeshContainers)
	{
		_uint iMaterialIndex = 0;

		if (TYPE_ANIM == m_eMeshType)
		{
			_float4x4			BoneMatrices[300];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 300);

			((CAnimMeshContainer*)pMeshContainer)->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			iMaterialIndex = ((CAnimMeshContainer*)pMeshContainer)->Get_MaterialIndex();

			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 300)))
				return E_FAIL;
			if (FAILED(Set_RawValue("g_OldBoneMatrices", m_OldBoneMatrix3, sizeof(_float4x4) * 300)))
				return E_FAIL;



			memcpy(&m_OldBoneMatrix, &BoneMatrices, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix2, &m_OldBoneMatrix, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix3, &m_OldBoneMatrix2, sizeof(_float4x4) * 300);

		}

		else
		{
			iMaterialIndex = ((CMeshContainer*)pMeshContainer)->Get_MaterialIndex();
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE])
		{
			if (FAILED(Set_TextureValue("g_DiffuseTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS])
		{
			if (FAILED(Set_TextureValue("g_NormalTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM])
		{
			if (FAILED(Set_TextureValue("g_RomTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM]->Get_SRV())))
				return E_FAIL;
		}
		if (true == ArrEmissive[iCount])
		{
			if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE])
			{
				if (FAILED(Set_TextureValue("g_EmissiveTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE]->Get_SRV())))
					return E_FAIL;
			}
		}
		else
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", nullptr)))
				return E_FAIL;
		}
		m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);

		pMeshContainer->Render(iPassIndex);

		++iCount;
	}

	return S_OK;
}

HRESULT CModel::Render(_uint iPassIndex, _uint iMeshContainerIndex, _uint iDesc_PassIndex, TYPE eType)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iPassIndex, _uint iSeparatePassIndex, _bool bMeshType,const char * pName)
{
	m_pDeviceContext->IASetInputLayout(m_Passes[iPassIndex].pInputLayout);

	for (auto& pMeshContainer : m_MeshContainers)
	{
		_uint iMaterialIndex = 0;

		if (TYPE_ANIM == m_eMeshType)
		{
			_float4x4			BoneMatrices[300];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * 300);

			((CAnimMeshContainer*)pMeshContainer)->Get_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			iMaterialIndex = ((CAnimMeshContainer*)pMeshContainer)->Get_MaterialIndex();

			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 300)))
				return E_FAIL;
			if (FAILED(Set_RawValue("g_OldBoneMatrices", m_OldBoneMatrix3, sizeof(_float4x4) * 300)))
				return E_FAIL;
			memcpy(&m_OldBoneMatrix, &BoneMatrices, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix2, &m_OldBoneMatrix, sizeof(_float4x4) * 300);
			memcpy(&m_OldBoneMatrix3, &m_OldBoneMatrix2, sizeof(_float4x4) * 300);
		}
		else
		{
			iMaterialIndex = ((CMeshContainer*)pMeshContainer)->Get_MaterialIndex();
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE])
		{
			if (FAILED(Set_TextureValue("g_DiffuseTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_DIFFUSE]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS])
		{
			if (FAILED(Set_TextureValue("g_NormalTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_NORMALS]->Get_SRV())))
				return E_FAIL;
		}

		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM])
		{
			if (FAILED(Set_TextureValue("g_RomTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_ROM]->Get_SRV())))
				return E_FAIL;
		}
		if (nullptr != m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE])
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", m_Materials[iMaterialIndex]->pTexture[TEXTURETYPE_EMISSIVE]->Get_SRV())))
				return E_FAIL;
		}
		else
		{
			if (FAILED(Set_TextureValue("g_EmissiveTexture", nullptr)))
				return E_FAIL;
		}


		if (true == bMeshType)
		{
			if (!strcmp(pName, ((CAnimMeshContainer*)pMeshContainer)->Get_Name()))
			{
				m_Passes[iSeparatePassIndex].pPass->Apply(0, m_pDeviceContext);
				pMeshContainer->Render(iSeparatePassIndex);
			}
			else
			{
				m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);
				pMeshContainer->Render(iPassIndex);
			}
		}
		else
		{
			if (!strcmp(pName, ((CMeshContainer*)pMeshContainer)->Get_Name()))
			{
				m_Passes[iSeparatePassIndex].pPass->Apply(0, m_pDeviceContext);
				pMeshContainer->Render(iSeparatePassIndex);
			}
			else
			{
				m_Passes[iPassIndex].pPass->Apply(0, m_pDeviceContext);
				pMeshContainer->Render(iPassIndex);
			}

		}
	}
		return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, char* pModelFilePath, char* pFilePathSpec, _fmatrix PivotMatrix)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pModelFilePath, pFilePathSpec, PivotMatrix)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	if (false == m_isCloned)
	{
		for (auto& pMaterial : m_Materials)
		{
			for (_uint i = 0; i < TEXTURETYPE_END; ++i)
				Safe_Release(pMaterial->pTexture[i]);

			Safe_Delete(pMaterial);
		}

		m_Materials.clear();
		m_Materials.swap(vector<MESHMATERIAL*>());
	}

	m_Materials.clear();

	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);

	m_MeshContainers.clear();
	m_MeshContainers.swap(vector<CVIBuffer*>());


	for (auto& pPass : m_Passes)
	{
		Safe_Release(pPass.pInputLayout);
		Safe_Release(pPass.pPass);
	}
	m_Passes.clear();

	Safe_Release(m_pEffect);

	if (true == m_bDivisionCheck)
	{
		for (auto& pAnimation : m_UpperAnimations)
			Safe_Release(pAnimation);

		m_UpperAnimations.clear();
		m_UpperAnimations.swap(vector<CAnimation*>());

		for (auto& pAnimation : m_BottomAnimations)
			Safe_Release(pAnimation);

		m_BottomAnimations.clear();
		m_BottomAnimations.swap(vector<CAnimation*>());

		for (auto& pAnimation : m_RootAnimations)
			Safe_Release(pAnimation);

		m_RootAnimations.clear();
		m_RootAnimations.swap(vector<CAnimation*>());
	}


	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();
	m_Animations.swap(vector<CAnimation*>());

	for (auto& RootNode : m_PlayerRootNodes)
		Safe_Release(RootNode);
	m_PlayerRootNodes.clear();
	m_PlayerRootNodes.swap(vector<CHierarchyNode*>());


	for (auto& UpperNode : m_PlayerUppperNodes)
		Safe_Release(UpperNode);
	m_PlayerUppperNodes.clear();
	m_PlayerUppperNodes.swap(vector<CHierarchyNode*>());


	for (auto& BottomNode : m_PlayerBottomNodes)
		Safe_Release(BottomNode);
	m_PlayerBottomNodes.clear();
	m_PlayerBottomNodes.swap(vector<CHierarchyNode*>());

	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		Safe_Release(pHierarchyNode);
	}

	m_HierarchyNodes.clear();
	m_HierarchyNodes.swap(vector<CHierarchyNode*>());


	__super::Free();
}
