#include "..\Public\VIBuffer_PointInstance.h"
#include "Transform.h"

CVIBuffer_PointInstance::CVIBuffer_PointInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer_Instance(pDevice, pDeviceContext)
{

}

CVIBuffer_PointInstance::CVIBuffer_PointInstance(const CVIBuffer_PointInstance & rhs)
	: CVIBuffer_Instance(rhs)
	, m_pReset(rhs.m_pReset)
	, m_pResetPos(rhs.m_pResetPos)
	, m_pResetDist(rhs.m_pResetDist)
{

}

HRESULT CVIBuffer_PointInstance::NativeConstruct_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXPOINT[m_iNumVertices];
	m_pVerticesPos = new _float3[m_iNumVertices];

	m_pVerticesPos[0] = ((VTXPOINT*)m_pVertices)[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	((VTXPOINT*)m_pVertices)[0].fPSize = 0.05f;

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(CVIBuffer::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertices);


	m_iNumInstance = iNumInstance;
	m_iInstStride = sizeof(VTXMATRIX);
	m_iIndexCountPerInstance = 1;

	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstDesc.ByteWidth = m_iInstStride * m_iNumInstance;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstDesc.MiscFlags = 0;
	m_VBInstDesc.StructureByteStride = m_iInstStride;

	m_pVertexMatrices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(m_pVertexMatrices, sizeof(VTXMATRIX) * m_iNumInstance);

	m_pResetPos = new _float3[m_iNumInstance];
	ZeroMemory(m_pResetPos, sizeof(_float3) * m_iNumInstance);

	m_pResetDist = new _float3[m_iNumInstance];
	ZeroMemory(m_pResetDist, sizeof(_float3) * m_iNumInstance);

	m_pReset = new _bool[m_iNumInstance];
	ZeroMemory(m_pReset, sizeof(_bool) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pVertexMatrices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		m_pVertexMatrices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		m_pVertexMatrices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_pVertexMatrices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);

		m_pResetPos[i] = _float3(0.f, 0.f, 0.f);
		m_pResetDist[i] = _float3(0.f, 0.f, 0.f);
		m_pReset[i] = true;
	}

	ZeroMemory(&m_VBInstSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBInstSubresourceData.pSysMem = m_pVertexMatrices;

	if (FAILED(CVIBuffer_Instance::Create_InstanceBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pVertexMatrices);

	m_iNumPrimitive = m_iNumInstance;
	m_iNumIndicesPerPrimitive = 1;
	m_iPrimitiveIndicesSize = sizeof(_ushort);
	m_eFormat = DXGI_FORMAT_R16_UINT;


	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pIndices = new _ushort[m_iNumPrimitive];



	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((_ushort*)m_pIndices)[i] = 0;
	}

	m_IBSubresourceData.pSysMem = m_pIndices;

	if (FAILED(CVIBuffer::Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC		Elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	if (FAILED(Compile_Shader(TEXT("../Bin/ShaderFiles/Shader_PointInstance.hlsl"), Elements, 6)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_PointInstance::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_PointInstance::Update_Default(EFFECTPARTICLEINFO* pParticleInfo, class CTransform* pTransform, _double TimeAcc, _double TimeDelta)
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

			m_pResetPos[i].x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, pParticleInfo->vMaxPos.x);
			m_pResetPos[i].y = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.y, pParticleInfo->vMaxPos.y);
			m_pResetPos[i].z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, pParticleInfo->vMaxPos.z);

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].x, pParticleInfo->vMaxPos.x, pParticleInfo->fLifeTime) * m_pResetDist[i].x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].y, pParticleInfo->vMaxPos.y, pParticleInfo->fLifeTime) * m_pResetDist[i].y;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].z, pParticleInfo->vMaxPos.z, pParticleInfo->fLifeTime) * m_pResetDist[i].z;

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
			{
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			}
		}
		if (pParticleInfo->fLifeTime <= TimeAcc)
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_PointInstance::Update_OnePoint(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
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

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].x, pParticleInfo->vMaxPos.x, pParticleInfo->fLifeTime) * m_pResetDist[i].x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].y, pParticleInfo->vMaxPos.y, pParticleInfo->fLifeTime) * m_pResetDist[i].y;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].z, pParticleInfo->vMaxPos.z, pParticleInfo->fLifeTime) * m_pResetDist[i].z;

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
			{
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			}
		}
		if (pParticleInfo->fLifeTime <= TimeAcc)
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_PointInstance::Update_Spiral(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
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

			m_pResetPos[i].x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, pParticleInfo->vMaxPos.x);
			m_pResetPos[i].y = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.y, pParticleInfo->vMaxPos.y);
			m_pResetPos[i].z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, pParticleInfo->vMaxPos.z);

			m_pResetPos[i].x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, pParticleInfo->vMaxPos.x);
			m_pResetPos[i].y = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.y, pParticleInfo->vMaxPos.y);
			m_pResetPos[i].z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, pParticleInfo->vMaxPos.z);

			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].x, pParticleInfo->vMaxPos.x, pParticleInfo->fLifeTime) * m_pResetDist[i].x;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].y, pParticleInfo->vMaxPos.y, pParticleInfo->fLifeTime) * m_pResetDist[i].y;
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].z, pParticleInfo->vMaxPos.z, pParticleInfo->fLifeTime) * m_pResetDist[i].z;

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
			{
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			}
		}
		if (pParticleInfo->fLifeTime <= TimeAcc)
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_PointInstance::Update_RandomPos(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
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

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			_float fX = m_pResetDist[i].x;
			_float fY = m_pResetDist[i].y;
			_float fZ = m_pResetDist[i].z;

			if (0.f <= fX)
				m_pResetPos[i].x = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxPos.x)
			else
				m_pResetPos[i].x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, 0.f);

			if (0.f <= fY)
				m_pResetPos[i].y = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxPos.y)
			else
				m_pResetPos[i].y = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.y, 0.f);

			if (0.f <= fZ)
				m_pResetPos[i].z = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxPos.z)
			else
				m_pResetPos[i].z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, 0.f);


			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].x, m_pResetPos[i].x * m_pResetDist[i].x, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].y, m_pResetPos[i].y * m_pResetDist[i].y, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].z, m_pResetPos[i].z * m_pResetDist[i].z, pParticleInfo->fLifeTime);

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
			{
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			}
		}
		if (pParticleInfo->fLifeTime <= TimeAcc)
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_PointInstance::Update_Environment(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
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

			m_pResetDist[i].x = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
			m_pResetDist[i].y = GET_RANDOMFLOAT(pParticleInfo->vMinDir.y, pParticleInfo->vMaxDir.y);
			m_pResetDist[i].z = GET_RANDOMFLOAT(pParticleInfo->vMinDir.z, pParticleInfo->vMaxDir.z);

			_float fX = m_pResetDist[i].x;
			_float fY = m_pResetDist[i].y;
			_float fZ = m_pResetDist[i].z;

			if (0.f <= fX)
				m_pResetPos[i].x = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxPos.x)
			else
				m_pResetPos[i].x = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.x, 0.f);

			if (0.f <= fY)
				m_pResetPos[i].y = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxPos.y)
			else
				m_pResetPos[i].y = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.y, 0.f);

			if (0.f <= fZ)
				m_pResetPos[i].z = GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxPos.z)
			else
				m_pResetPos[i].z = GET_RANDOMFLOAT(-pParticleInfo->vMaxPos.z, 0.f);


			((VTXMATRIX*)SubResource.pData)[i].vPosition.x = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 0.f;
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;
		}

		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].x, m_pResetPos[i].x * m_pResetDist[i].x, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].y, m_pResetPos[i].y * m_pResetDist[i].y, pParticleInfo->fLifeTime);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = Set_Option(pParticleInfo->iOptionX, TimeAcc, m_pResetPos[i].z, m_pResetPos[i].z * m_pResetDist[i].z, pParticleInfo->fLifeTime);

		if (pParticleInfo->isRandomUV)
		{
			if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
			{
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.x = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.x);
				((VTXMATRIX*)SubResource.pData)[i].vCurUV.y = (_uint)GET_RANDOMFLOAT(0.f, pParticleInfo->vMaxUV.y);
			}
		}
		if (pParticleInfo->fLifeTime <= TimeAcc)
		{
			m_pReset[i] = true;
		}
	}

	if (pParticleInfo->fUVDelay <= m_UVTimeAcc)
		m_UVTimeAcc = 0.0;

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_PointInstance::Update_Skate(EFFECTPARTICLEINFO * pParticleInfo, CTransform * pTransform, _double TimeAcc, _double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float fRandDistX = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
		_float fRandDistY = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
		_float fRandDistZ = GET_RANDOMFLOAT(pParticleInfo->vMinDir.x, pParticleInfo->vMaxDir.x);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y -= TimeDelta ;

		if (((VTXMATRIX*)SubResource.pData)[i].vPosition.z <= -pParticleInfo->vMaxPos.z)
		{
			((VTXMATRIX*)SubResource.pData)[i].vPosition.z = 0.f;

		}

	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_PointInstance::Update_Test(_double TimeDelta)
{
	if (nullptr == m_pDeviceContext)
		return;

	// D3D11_unordered_map_WRITE_DISCARD
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y -= TimeDelta;

		if (((VTXMATRIX*)SubResource.pData)[i].vPosition.y <= 0.0f)
			((VTXMATRIX*)SubResource.pData)[i].vPosition.y = 15.0f;

	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

_float CVIBuffer_PointInstance::Set_Option(_uint iOption, _float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
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

_float CVIBuffer_PointInstance::Linear(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return fEndValue * fCurTime / fEndTime + fStartValue;
}

_float CVIBuffer_PointInstance::Ease_In_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime;

	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return fEndValue * fCurTime * fCurTime + fStartValue;
}

_float CVIBuffer_PointInstance::Ease_Out_Quad(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime;

	if (0 == fEndTime)
		return fEndValue + fStartValue;

	return -fEndValue * fCurTime * (fCurTime - 2) + fStartValue;
}

_float CVIBuffer_PointInstance::Ease_In_Out(_float fCurTime, _float fStartValue, _float fEndValue, _float fEndTime)
{
	fCurTime /= fEndTime / 2;
	if (0 == fEndTime)
		return fEndValue + fStartValue;

	if (fCurTime < 1)
		return fEndValue / 2 * fCurTime * fCurTime + fStartValue;
	fCurTime--;
	return -fEndValue / 2 * (fCurTime * (fCurTime - 2) - 1) + fStartValue;
}

CVIBuffer_PointInstance * CVIBuffer_PointInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iNumInstance)
{
	CVIBuffer_PointInstance*	pInstance = new CVIBuffer_PointInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(iNumInstance)))
	{
		MSGBOX("Failed to Created CVIBuffer_PointInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_PointInstance::Clone(void * pArg)
{
	CVIBuffer_PointInstance*	pInstance = new CVIBuffer_PointInstance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_PointInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_PointInstance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pResetDist);
		Safe_Delete_Array(m_pResetPos);
		Safe_Delete_Array(m_pReset);
	}
}
