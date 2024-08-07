#pragma once

#include "Model.h"
#include "VIBuffer.h"

BEGIN(Engine)

class CAnimMeshContainer final : public CVIBuffer
{
private:
	CAnimMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CAnimMeshContainer(const CAnimMeshContainer& rhs);
	virtual ~CAnimMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}
	HRESULT SetUp_MeshBone(CModel * pModel, CHierarchyNode* pNode);
	void Get_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);

public:
	virtual HRESULT NativeConstruct_Prototype(VTXANIMMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint _iNumVertices, _uint _iNumFaceIndices, _uint iMaterialIndex, _uint iNumBone, char* _szName);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint iPassIndex) override;

public:
	const char* Get_Name() { return m_szName; }

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;
	_uint				m_iNumBones = 0;

	vector<class CHierarchyNode*>		m_BoneNodes;

public:
	static CAnimMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, VTXANIMMODEL* pVTXModelVertices, FACEINDICES32* _pIndices, _uint _iNumVertices, _uint _iNumFaceIndices, _uint iMaterialIndex, _uint iNumBone, char* _szName);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END