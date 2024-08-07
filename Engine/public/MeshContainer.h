#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT NativeConstruct_Prototype(_uint iMaterialIndex, VTXMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint _iNumVertices, _uint _iNumFaceIndices, _float3* _pVerticesPos, char* szMeshContainerName);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint iPassIndex) override;

public:
	const char* Get_Name() {return m_szName;}

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;


public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, VTXMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint iMaterialIndex, _uint _iNumVertices, _uint _iNumFaceIndices, _float3* _pVerticesPos, char* szMeshContainerName);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END