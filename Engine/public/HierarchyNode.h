#pragma once

/* 내 모델을 구성하는 데이터(1. 메시컨테이너, 2. 뼈)들의 상속구조.  */

#include "Base.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:

	const char* Get_Name() const {
		return m_szName;
	}

	const char* Get_ParentName() const {
		return m_szParentName;
	}

	_matrix Get_CombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	_matrix Get_RootCombinedTransformationMatrix() {
		return XMLoadFloat4x4(&m_RootCombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrixPtr() {
		return &m_CombinedTransformationMatrix;
	}

	_matrix Get_OffsetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

public:
	HRESULT NativeConstruct(const char* pNodeName, const char* pParentNodeName, _cmatrix OffSetMatrix, _cmatrix TransformationMatrix);
	void SetUp_OffsetMatrix(_fmatrix OffsetMatrix) {
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}
	void Update_CombinedTransformationMatrix(_uint iAnimIndex);
	void Update_UpperCombinedTransformationMatrix(_uint iAnimIndex);
	void Update_RootCombinedTransformationMatrix(_uint iAnimIndex, _bool bRootHoldCheck);

	void Add_AnimBones(_uint iAnimationIndex, class CAnimBone* pAnimBone) {
		m_AnimBones[iAnimationIndex] = pAnimBone;
	}

	void SetUp_Parent(CHierarchyNode* pNode);

	void Scaling(_float _fScale);
	void ResetScaling();

	void Resize_AnimBones(_uint iNumAnimations) {
		m_AnimBones.resize(iNumAnimations);
	}
private:
	char			m_szName[MAX_PATH] = "";
	char			m_szParentName[MAX_PATH] = "";

	/* 애니메이션 로컬스페이스에서 정점의 로컬스페이스로 변형해주기위한 행렬이다.  */
	_float4x4		m_OffsetMatrix;

	/* 현재 노드의 상태행렬. (부모관계가 있는 경우, m_TransformationMatrix부모기준의 상태행렬이다. else : 원점기준의 상태다.  */
	_float4x4		m_TransformationMatrix;

	/* m_TransformationMatrix * 부모`s m_CombinedTransformationMatrix */
	_float4x4		m_CombinedTransformationMatrix;

	_float4x4		m_RootCombinedTransformationMatrix;

	_bool			m_bUpperNameCheck = false;

	CHierarchyNode*	m_pParent = nullptr;

private:
	vector<CAnimBone*>				m_AnimBones;

public:
	static CHierarchyNode* Create(const char* pNodeName, const char* pParentNodeName, _cmatrix OffSetMatrix,_cmatrix TransformationMatrix);
	virtual void Free() override;
};

END