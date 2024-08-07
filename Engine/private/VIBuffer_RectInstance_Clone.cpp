#include "..\Public\VIBuffer_RectInstance_Clone.h"
#include "Transform.h"
#include "GameInstance.h"

CVIBuffer_RectInstance_Clone::CVIBuffer_RectInstance_Clone(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer_Instance(pDevice, pDeviceContext)
{

}

CVIBuffer_RectInstance_Clone::CVIBuffer_RectInstance_Clone(const CVIBuffer_RectInstance_Clone & rhs)
	: CVIBuffer_Instance(rhs)
{

}

HRESULT CVIBuffer_RectInstance_Clone::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_RectInstance_Clone::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXTEX[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	m_pVerticesPos[0] = ((VTXTEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.f, 0.f);

	m_pVerticesPos[1] = ((VTXTEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.f, 0.f);

	m_pVerticesPos[2] = ((VTXTEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.f, 1.f);

	m_pVerticesPos[3] = ((VTXTEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices)[3].vTexUV = _float2(0.f, 1.f);

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(CVIBuffer::Create_VertexBuffer()))
		return E_FAIL;
	

	m_iNumInstance = 10;
	m_iInstStride = sizeof(VTXMATRIX);
	m_iIndexCountPerInstance = 6;

	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstDesc.ByteWidth = m_iInstStride * m_iNumInstance;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstDesc.MiscFlags = 0;
	m_VBInstDesc.StructureByteStride = m_iInstStride;

	m_pVertexMatrices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(m_pVertexMatrices, sizeof(VTXMATRIX) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pVertexMatrices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_pVertexMatrices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_pVertexMatrices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_pVertexMatrices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		m_pVertexMatrices[i].vCurUV = _float2(0.f, 0.f);
		m_pVertexMatrices[i].fAlpha = (_float)i / m_iNumInstance;

		m_pResetDist[i] = _float3(0.f, 0.f, 0.f);
		m_pResetPos[i] = _float3(0.f, 0.f, 0.f);
		m_pReset[i] = true;
	}

	ZeroMemory(&m_VBInstSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBInstSubresourceData.pSysMem = m_pVertexMatrices;

	if (FAILED(CVIBuffer_Instance::Create_InstanceBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertexMatrices);

	m_iNumPrimitive = m_iNumInstance * 2;
	m_iNumIndicesPerPrimitive = 3;
	m_iPrimitiveIndicesSize = sizeof(FACEINDICES16);
	m_eFormat = DXGI_FORMAT_R16_UINT;


	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pIndices = new FACEINDICES16[m_iNumPrimitive];

	_uint			iNumFace = 0;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((FACEINDICES16*)m_pIndices)[iNumFace]._1 = 0;
		((FACEINDICES16*)m_pIndices)[iNumFace]._2 = 1;
		((FACEINDICES16*)m_pIndices)[iNumFace]._3 = 2;
		++iNumFace;

		((FACEINDICES16*)m_pIndices)[iNumFace]._1 = 0;
		((FACEINDICES16*)m_pIndices)[iNumFace]._2 = 2;
		((FACEINDICES16*)m_pIndices)[iNumFace]._3 = 3;
		++iNumFace;
	}

	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(CVIBuffer::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pIndices);

	D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 6, DXGI_FORMAT_R32_FLOAT, 1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_RectInstance.hlsl"), Elements, 8)))
		return E_FAIL;



	return S_OK;
}

void CVIBuffer_RectInstance_Clone::Update_Smoke_Landing(EFFECTPARTICLEINFO * pParticleInfo, _fvector vCamPos, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_UVTimeAcc += TimeDelta;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i])
		{
			m_pReset[i] = false;
			_float fRandAngle = GET_RANDOMFLOAT(0, 360);
			((VTXMATRIX*)SubResource.pData)[i].vUp.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vUp.z = 1.f;
			((VTXMATRIX*)SubResource.pData)[i].vLook.y = 1.f;
			((VTXMATRIX*)SubResource.pData)[i].vLook.z = 0.f;
			m_pResetPos[i].x = fRandAngle;

			_vector vUp = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, XMVector4Normalize(vUp) * pParticleInfo->vSize.x);
			_vector vRight = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, XMVector4Normalize(vRight) * pParticleInfo->vSize.y);

			_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
			_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x += cosf(XMConvertToRadians(m_pResetPos[i].x)) *  static_cast<_float>(TimeDelta) * m_pResetDist[i].x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += static_cast<_float>(TimeDelta) * 0.01f * static_cast<_float>(i);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z += sinf(XMConvertToRadians(m_pResetPos[i].x)) * static_cast<_float>(TimeDelta) * m_pResetDist[i].x;


		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}
		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Billboard(EFFECTPARTICLEINFO * pParticleInfo, _fvector vCamPos, CTransform* pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_UVTimeAcc += TimeDelta;



	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i])
		{
			_float fRandAngle = GET_RANDOMFLOAT(0, 360);
			fRandAngle = XMConvertToRadians(fRandAngle);
			m_pReset[i] = false;

			_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
			_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;

			m_pResetPos[i].x = 0.f;
			m_pResetPos[i].y = 0.f;
			m_pResetPos[i].z = 0.f;
		}

		_vector		vLook = XMVector3Normalize(vCamPos - pTransform->Get_Position());
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
		_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		vLook = XMVectorSetW(vLook, 0.f);
		vRight = XMVectorSetW(vRight, 0.f);
		vUp = XMVectorSetW(vUp, 0.f);

		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, vRight * pParticleInfo->vSize.x);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, vUp * pParticleInfo->vSize.y);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, vLook);

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].x, pParticleInfo->vMaxPos.x, pParticleInfo->fLifeTime) * m_pResetDist[i].x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].y, pParticleInfo->vMaxPos.y, pParticleInfo->fLifeTime) * m_pResetDist[i].y;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].z, pParticleInfo->vMaxPos.z, pParticleInfo->fLifeTime) * m_pResetDist[i].z;

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}
		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Option(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	m_UVTimeAcc += TimeAcc;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i])
		{
			m_pReset[i] = false;

			m_pResetPos[i].x = 0.f;
			m_pResetPos[i].y = 0.f;
			m_pResetPos[i].z = 0.f;

			_vector vUp = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, XMVector4Normalize(vUp) * pParticleInfo->vSize.x);
			_vector vRight = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, XMVector4Normalize(vRight) * pParticleInfo->vSize.y);

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].x, pParticleInfo->vMaxPos.x, pParticleInfo->fLifeTime) * m_pResetDist[i].x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].y, pParticleInfo->vMaxPos.y, pParticleInfo->fLifeTime) * m_pResetDist[i].y;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].z, pParticleInfo->vMaxPos.z, pParticleInfo->fLifeTime) * m_pResetDist[i].z;

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}
		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Smoke_Die(EFFECTPARTICLEINFO * pParticleInfo, _fvector vCamPos, CTransform* pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARDclass CTransform* pTransform,
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	m_UVTimeAcc += TimeAcc;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i])
		{
			m_pReset[i] = false;

			m_pResetPos[i].x = GET_RANDOMFLOAT(-0.5f, 0.5f);
			m_pResetPos[i].y = GET_RANDOMFLOAT(-0.5f, 0.5f);
			m_pResetPos[i].z = GET_RANDOMFLOAT(-0.5f, 0.5f);

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = GET_RANDOMFLOAT(-0.5f, 0.5f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = GET_RANDOMFLOAT(-0.5f, 0.5f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = GET_RANDOMFLOAT(-0.5f, 0.5f);
		}

		_vector		vLook = XMVector3Normalize(vCamPos - pTransform->Get_Position());
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
		_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		vLook = XMVectorSetW(vLook, 0.f);
		vRight = XMVectorSetW(vRight, 0.f);
		vUp = XMVectorSetW(vUp, 0.f);

		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, vRight * pParticleInfo->vSize.x);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, vUp * pParticleInfo->vSize.y);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, vLook);

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].x, m_pResetDist[i].x, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].y, m_pResetDist[i].y, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].z, m_pResetDist[i].z, pParticleInfo->fLifeTime);

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}
		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	list<VIEWDESC> ListViewZ;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		VIEWDESC ViewZ;
		ViewZ.fViewZ = XMVectorGetX(XMVector4Length(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - pGameInstance->Get_CamPosition()));
		XMStoreFloat4(&ViewZ.vPosition, XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition));

		ListViewZ.emplace_back(ViewZ);
		Safe_Release(pGameInstance);
	}

	ListViewZ.sort(stlListSort());
	auto iter = ListViewZ.begin();
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition = iter->vPosition;
		++iter;
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Spark(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARDclass CTransform* pTransform,
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	m_UVTimeAcc += TimeAcc;
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i])
		{
			m_pReset[i] = false;

			m_pResetPos[i].x = 0.f;
			m_pResetPos[i].y = 0.f;
			m_pResetPos[i].z = 0.f;

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;
		}

		_vector		vUp = XMVector3Normalize(XMLoadFloat3(&m_pResetDist[i]));
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), vUp));
		_vector		vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

		vLook = XMVectorSetW(vLook, 0.f);
		vRight = XMVectorSetW(vRight, 0.f);
		vUp = XMVectorSetW(vUp, 0.f);

		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, vRight * pParticleInfo->vSize.x);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, vUp * pParticleInfo->vSize.y);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, vLook);

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].x, m_pResetDist[i].x, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].y, m_pResetDist[i].y, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, static_cast<_float>(TimeAcc), m_pResetPos[i].z, m_pResetDist[i].z, pParticleInfo->fLifeTime);

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}
		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	list<VIEWDESC> ListViewZ;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		VIEWDESC ViewZ;
		ViewZ.fViewZ = XMVectorGetX(XMVector4Length(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition) - pGameInstance->Get_CamPosition()));
		XMStoreFloat4(&ViewZ.vPosition, XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vPosition));

		ListViewZ.emplace_back(ViewZ);
		Safe_Release(pGameInstance);
	}

	ListViewZ.sort(stlListSort());
	auto iter = ListViewZ.begin();
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition = iter->vPosition;
		++iter;
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Smoke_Working(EFFECTPARTICLEINFO * pParticleInfo, _fvector vTargetPos, _fvector vCamPos, _bool isUpdate, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_UVTimeAcc += TimeDelta;

	if (!m_isTick)
	{
		if (0 >= m_fResetTime)
			m_isTick = true;
		else
			m_fResetTime -= (_float)TimeDelta * pParticleInfo->fLifeTime;
	}

	if (m_isUpdate && !isUpdate)
	{
		m_isUpdate = false;
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			if (1.f <= truncf(((VTXMATRIX*)SubResource.pData)[i].fAlpha))
				((VTXMATRIX*)SubResource.pData)[i].fAlpha = 1.f;
		}
	}

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i] || !m_isFirst)
		{
			m_pReset[i] = false;
			((VTXMATRIX*)SubResource.pData)[i].vUp.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vUp.z = 1.f;
			((VTXMATRIX*)SubResource.pData)[i].vLook.y = 1.f;
			((VTXMATRIX*)SubResource.pData)[i].vLook.z = 0.f;

			_float fRandUVX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
			_float fRandUVY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandUVX);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandUVY);

			_float fRandX = GET_RANDOMFLOAT(-0.4f, 0.4f);
			_float fRandZ = GET_RANDOMFLOAT(-0.4f, 0.4f);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = (XMVectorGetX(vTargetPos) / pParticleInfo->vSize.x) + fRandX;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = XMVectorGetY(vTargetPos);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = (XMVectorGetZ(vTargetPos) / pParticleInfo->vSize.y) + fRandZ;

			_vector vUp = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, XMVector4Normalize(vUp) * pParticleInfo->vSize.x);
			_vector vRight = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, XMVector4Normalize(vRight) * pParticleInfo->vSize.y);

			if (!m_isFirst)
				((VTXMATRIX*)SubResource.pData)[i].fAlpha = 1.f;
			else
				((VTXMATRIX*)SubResource.pData)[i].fAlpha = 0.f;
		}

		((VTXMATRIX*)SubResource.pData)[i].fAlpha += (_float)TimeDelta * pParticleInfo->fLifeTime;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += (_float)TimeDelta * 0.02f;


		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}

		if (1.f <= ((VTXMATRIX*)SubResource.pData)[i].fAlpha)
		{
			if (!isUpdate && !m_isUpdate && m_isTick)
			{
				m_pReset[i] = true;
				m_isTick = false;
				m_fResetTime = 1.f / m_iNumInstance;
			}
			else
				m_isUpdate = true;
		}
	}
	m_isFirst = true;

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Smoke_Object(EFFECTPARTICLEINFO * pParticleInfo, _fvector vCamPos, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_UVTimeAcc += TimeDelta;


	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i])
		{
			_float fRandAngle = GET_RANDOMFLOAT(0, 360);
			fRandAngle = XMConvertToRadians(fRandAngle);
			m_pReset[i] = false;

			_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
			_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);

			_vector vUp = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, XMVector4Normalize(vUp) * pParticleInfo->vSize.x);
			_vector vRight = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, XMVector4Normalize(vRight) * pParticleInfo->vSize.y);

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			if (0 <= m_pResetDist[i].x)
			{
				_float fRand = GET_RANDOMFLOAT(-1, 1.f);
				if (0 <= fRand)
				{
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = pParticleInfo->vMaxDir.x;
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);
				}
				else
				{
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = pParticleInfo->vMinDir.x;
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);
				}
			}
			else
			{
				_float fRand = GET_RANDOMFLOAT(-1, 1.f);
				if (0 <= fRand)
				{
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = pParticleInfo->vMaxDir.z;
				}
				else
				{
					((VTXMATRIX*)SubResource.pData)[i].vPosition.x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
					((VTXMATRIX*)SubResource.pData)[i].vPosition.z = pParticleInfo->vMinDir.z;
				}
			}

			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
		}
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += (_float)TimeDelta * 0.04f * (_float)i;


		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}
		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Paper(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_UVTimeAcc += TimeDelta;



	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i])
		{
			_float fRandAngle = GET_RANDOMFLOAT(0, 360);
			fRandAngle = XMConvertToRadians(fRandAngle);
			m_pReset[i] = false;

			_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
			_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);

			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, XMVectorSet(1.f, 0.f, 0.f, 0.f) * pParticleInfo->vSize.x);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f) * pParticleInfo->vSize.y);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f));

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			m_pResetPos[i].y = GET_RANDOMFLOAT(pParticleInfo->vMaxPos.y * 0.5f, pParticleInfo->vMaxPos.y);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, pParticleInfo->vMaxPos.x);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = GET_RANDOMFLOAT(0.f, 0.3f);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, pParticleInfo->vMaxPos.z);
		}


		((VTXMATRIX*)SubResource.pData)[i].vPosition.x += m_pResetDist[i].x * static_cast<_float>(TimeDelta);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += (static_cast<_float>(TimeAcc) * static_cast<_float>(TimeAcc) * (-9.8f) * 0.5f + static_cast<_float>(TimeAcc) * m_pResetDist[i].y) * static_cast<_float>(TimeDelta) * m_pResetPos[i].y;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z += m_pResetDist[i].z * static_cast<_float>(TimeDelta);

		_float fX = GET_RANDOMFLOAT(-0.5f, 0.5f);
		_float fZ = GET_RANDOMFLOAT(-0.5f, 0.5f);
		_vector vAxis = XMVectorSet(fX, 0.f, fZ, 0.f);

		_matrix RotMat = XMMatrixRotationAxis(XMVector3Normalize(vAxis), XMConvertToRadians(90.f) * static_cast<_float>(TimeDelta) * 2.f);
		_vector vRight = XMVector3TransformNormal(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight), RotMat);
		_vector vUp = XMVector3TransformNormal(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp), RotMat);
		_vector vLook = XMVector3TransformNormal(XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook), RotMat);

		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, vRight);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, vUp);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, vLook);

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
			{
				_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);
			}
		}
		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Fire(EFFECTPARTICLEINFO * pParticleInfo, _fvector vCamPos, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	m_UVTimeAcc += TimeDelta;


	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i] || !m_isFirst)
		{
			_float fRandAngle = GET_RANDOMFLOAT(0, 360);
			fRandAngle = XMConvertToRadians(fRandAngle);
			m_pReset[i] = false;

			_float fRandX = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
			_float fRandY = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = truncf(fRandX);
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = truncf(fRandY);

			_vector vUp = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, XMVector4Normalize(vUp) * pParticleInfo->vSize.x);
			_vector vRight = XMLoadFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight);
			XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, XMVector4Normalize(vRight) * pParticleInfo->vSize.y);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			if (1 <= pParticleInfo->vMaxPos.y)
			{
				((VTXMATRIX*)SubResource.pData)[i].vUp.y = 0.f;
				((VTXMATRIX*)SubResource.pData)[i].vUp.z = 1.f;
				((VTXMATRIX*)SubResource.pData)[i].vLook.y = 1.f;
				((VTXMATRIX*)SubResource.pData)[i].vLook.z = 0.f;
			}
		}

		if (pParticleInfo->fUVDelay <= static_cast<_float>(m_UVTimeAcc))
		{
			((VTXMATRIX*)SubResource.pData)[i].vCurUV.x += 1;
			if (((VTXMATRIX*)SubResource.pData)[i].vCurUV.x >= pParticleInfo->vMaxUV.x)
			{
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y += 1;
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = 0;
				if (((VTXMATRIX*)SubResource.pData)[i].vCurUV.y >= pParticleInfo->vMaxUV.y)
				{
					((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = 0;
				}
			}

		}

		if (pParticleInfo->fLifeTime <= static_cast<_float>(TimeAcc))
		{
			m_pReset[i] = true;
		}
	}
	m_isFirst = true;

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_Particle(EFFECTPARTICLEINFO* pParticleInfo, _fvector vTargetPos, _fvector vCamPos, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i] || !m_isFirst)
		{
			m_pReset[i] = false;
			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, pParticleInfo->vMaxPos.x);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.y, pParticleInfo->vMaxPos.y);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, pParticleInfo->vMaxPos.z);

			if (!m_isFirst)
				((VTXMATRIX*)SubResource.pData)[i].fAlpha = (_float)i / m_iNumInstance;
			else
				((VTXMATRIX*)SubResource.pData)[i].fAlpha = 0.f;
		}
		((VTXMATRIX*)SubResource.pData)[i].vPosition.x += (_float)TimeDelta * m_pResetDist[i].x * pParticleInfo->fLifeTime;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += (_float)TimeDelta * m_pResetDist[i].y * pParticleInfo->fLifeTime;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z += (_float)TimeDelta * m_pResetDist[i].z * pParticleInfo->fLifeTime;

		_vector		vLook = XMVector3Normalize(vCamPos - vTargetPos);
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
		_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		vLook = XMVectorSetW(vLook, 0.f);
		vRight = XMVectorSetW(vRight, 0.f);
		vUp = XMVectorSetW(vUp, 0.f);

		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, vRight * pParticleInfo->vSize.x);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, vUp * pParticleInfo->vSize.y);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, vLook);

		((VTXMATRIX*)SubResource.pData)[i].fAlpha += (_float)TimeDelta * pParticleInfo->fLifeTime;

		if (((VTXMATRIX*)SubResource.pData)[i].fAlpha >= 1.f)
		{
			m_pReset[i] = true;
		}
	}
	m_isFirst = true;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_BulletFire(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _fvector vCamPos, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i] || !m_isFirst)
		{
			m_pReset[i] = false;

			_float3 vPos;

			vPos.x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, pParticleInfo->vMaxPos.x);
			vPos.y = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.y, pParticleInfo->vMaxPos.y);
			vPos.z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, pParticleInfo->vMaxPos.z);

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = (XMVectorGetX(pTransform->Get_Position()) + vPos.x) / pParticleInfo->vSize.x;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = (XMVectorGetY(pTransform->Get_Position()) + vPos.y) / pParticleInfo->vSize.x;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = (XMVectorGetZ(pTransform->Get_Position()) + vPos.z) / pParticleInfo->vSize.x;

			if (m_isFirst)
				((VTXMATRIX*)SubResource.pData)[i].fAlpha = 0.f;
		}

		_vector		vUp = XMVector3Normalize(XMLoadFloat3(&m_pResetDist[i]));
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 0.f, 1.f, 0.f), vUp));
		_vector		vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

		vLook = XMVectorSetW(vLook, 0.f);
		vRight = XMVectorSetW(vRight, 0.f);
		vUp = XMVectorSetW(vUp, 0.f);

		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, vRight * pParticleInfo->vSize.x);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, vUp * pParticleInfo->vSize.y);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, vLook);


		_vector vDist = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_RIGHT)) * (_float)TimeDelta * m_pResetDist[i].x +
			XMVector3Normalize(pTransform->Get_State(CTransform::STATE_UP)) * (_float)TimeDelta * m_pResetDist[i].y +
			XMVector3Normalize(pTransform->Get_State(CTransform::STATE_LOOK)) * (_float)TimeDelta * m_pResetDist[i].z;

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x += XMVectorGetX(vDist);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += XMVectorGetY(vDist);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z += XMVectorGetZ(vDist);

		((VTXMATRIX*)SubResource.pData)[i].fAlpha += (_float)TimeDelta * pParticleInfo->fLifeTime;

		if (((VTXMATRIX*)SubResource.pData)[i].fAlpha >= 1.f)
		{
			m_pReset[i] = true;
		}
	}
	m_isFirst = true;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_RectInstance_Clone::Update_VentSmoke(EFFECTPARTICLEINFO * pParticleInfo, _fvector vTargetPos, _fvector vCamPos, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		if (m_pReset[i] || !m_isFirst)
		{
			m_pReset[i] = false;
			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = XMVectorGetX(vTargetPos);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = XMVectorGetY(vTargetPos);
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = XMVectorGetZ(vTargetPos);
		}

		m_pResetPos[i].x += (_float)TimeDelta * m_pResetDist[i].x;
		m_pResetPos[i].y += (_float)TimeDelta * m_pResetDist[i].y;
		m_pResetPos[i].z += (_float)TimeDelta * m_pResetDist[i].z;

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x += (_float)TimeDelta * m_pResetDist[i].x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += (_float)TimeDelta * m_pResetDist[i].y;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z += (_float)TimeDelta * m_pResetDist[i].z;

		_vector		vLook = XMVector3Normalize(vCamPos - vTargetPos);
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
		_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

		vLook = XMVectorSetW(vLook, 0.f);
		vRight = XMVectorSetW(vRight, 0.f);
		vUp = XMVectorSetW(vUp, 0.f);

		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vRight, vRight * pParticleInfo->vSize.x);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vUp, vUp * pParticleInfo->vSize.y);
		XMStoreFloat4(&((VTXMATRIX*)SubResource.pData)[i].vLook, vLook);

		((VTXMATRIX*)SubResource.pData)[i].fAlpha += (_float)TimeDelta * pParticleInfo->fLifeTime;
		if (((VTXMATRIX*)SubResource.pData)[i].fAlpha >= 1.f)
		{

			((VTXMATRIX*)SubResource.pData)[i].fAlpha = 0.f;

			m_pResetPos[i].x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = XMVectorGetX(vTargetPos);
			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);

			m_pResetPos[i].y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = XMVectorGetY(vTargetPos);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);

			m_pResetPos[i].z = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = XMVectorGetZ(vTargetPos);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);
		}
	}
	m_isFirst = true;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

_float CVIBuffer_RectInstance_Clone::Set_Option(_uint iOption, _float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	switch (iOption)
	{
	case Engine::CVIBuffer_PointInstance::OPTION_NONE:
		return fStartValue;
		break;
	case Engine::CVIBuffer_PointInstance::OPTION_LINEAR:
		return Linear(fCurTime, fStartValue, fEndValue, fEndTime);
		break;
	case Engine::CVIBuffer_PointInstance::OPTION_EIQ:
		return Ease_In_Quad(fCurTime, fStartValue, fEndValue, fEndTime);
		break;
	case Engine::CVIBuffer_PointInstance::OPTION_EOQ:
		return Ease_Out_Quad(fCurTime, fStartValue, fEndValue, fEndTime);
		break;
	case Engine::CVIBuffer_PointInstance::OPTION_EIO:
		return Ease_In_Out(fCurTime, fStartValue, fEndValue, fEndTime);
		break;
	}
	return 0.f;
}

_float CVIBuffer_RectInstance_Clone::Linear(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return fEndValue * fCurTime / fEndTime + fStartValue;
}

_float CVIBuffer_RectInstance_Clone::Ease_In_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime;

	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return fEndValue * fCurTime * fCurTime + fStartValue;
}

_float CVIBuffer_RectInstance_Clone::Ease_Out_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime;

	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return -fEndValue * fCurTime * (fCurTime - 2) + fStartValue;
}

_float CVIBuffer_RectInstance_Clone::Ease_In_Out(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime / 2;
	if (0 == fEndTime)
		return fEndValue + fStartValue;

	if (fCurTime < 1)
		return fEndValue / 2 * fCurTime * fCurTime + fStartValue;
	fCurTime--;
	return -fEndValue / 2 * (fCurTime * (fCurTime - 2) - 1) + fStartValue;
}

CVIBuffer_RectInstance_Clone * CVIBuffer_RectInstance_Clone::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_RectInstance_Clone*	pInstance = new CVIBuffer_RectInstance_Clone(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CVIBuffer_RectInstance_Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectInstance_Clone::Clone(void * pArg)
{
	CVIBuffer_RectInstance_Clone*	pInstance = new CVIBuffer_RectInstance_Clone(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_RectInstance_Clone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_RectInstance_Clone::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pVerticesPos);
	Safe_Delete_Array(m_pIndices);
}
