#include "..\Public\HierarchyNode.h"
#include "AnimBone.h"



CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char* pNodeName, const char* pParentNodeName, _cmatrix OffSetMatrix, _cmatrix TransformationMatrix)
{
	strcpy_s(m_szName, pNodeName);
	if(nullptr != pParentNodeName)
		strcpy_s(m_szParentName, pParentNodeName);
	XMStoreFloat4x4(&m_OffsetMatrix, OffSetMatrix);
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iAnimIndex)
{
	CAnimBone*		pAnimBone = m_AnimBones[iAnimIndex];

	_matrix			TransformationMatrix;

	if (nullptr == pAnimBone)
		TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	else
		TransformationMatrix = pAnimBone->Get_TransformationMatrix();


	if(nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * m_pParent->Get_CombinedTransformationMatrix());
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * XMMatrixIdentity());
}

void CHierarchyNode::Update_UpperCombinedTransformationMatrix(_uint iAnimIndex)
{
	CAnimBone*		pAnimBone = m_AnimBones[iAnimIndex];

	_matrix			TransformationMatrix;

	if (nullptr == pAnimBone)
		TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	else
	{
		if (false == m_bUpperNameCheck && !strcmp(pAnimBone->Get_szName(), "spine0"))
			m_bUpperNameCheck = true;

		TransformationMatrix = pAnimBone->Get_TransformationMatrix();
	}

	if (nullptr != m_pParent)
	{
		if(true == m_bUpperNameCheck)
			XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * m_pParent->Get_RootCombinedTransformationMatrix());
		else
			XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * m_pParent->Get_CombinedTransformationMatrix());
	}
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * XMMatrixIdentity());
}

void CHierarchyNode::Update_RootCombinedTransformationMatrix(_uint iAnimIndex, _bool bRootHoldCheck)
{
	CAnimBone*		pAnimBone = m_AnimBones[iAnimIndex];

	_matrix			TransformationMatrix;

	_matrix			RootTransformationMatrix;

	_vector			RootScale;
	_vector			RootRight, RootUp, RootLook;

	if (nullptr == pAnimBone)
		TransformationMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
	else
	{
		if (true == bRootHoldCheck)
		{
			RootTransformationMatrix = pAnimBone->Get_TransformationMatrix();

			RootScale = XMVectorSet(XMVectorGetX(XMVector3Length(RootTransformationMatrix.r[0])), XMVectorGetY(XMVector3Length(RootTransformationMatrix.r[1])), XMVectorGetZ(XMVector3Length(RootTransformationMatrix.r[2])), 1.f);

			RootRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(RootScale);
			RootUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(RootScale);
			RootLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(RootScale);

			RootTransformationMatrix.r[0] = RootRight;
			RootTransformationMatrix.r[1] = RootUp;
			RootTransformationMatrix.r[2] = RootLook;

			TransformationMatrix = RootTransformationMatrix;
		}
		else
		{
				TransformationMatrix = pAnimBone->Get_TransformationMatrix();
		}
	}


	if (nullptr != m_pParent)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * m_pParent->Get_CombinedTransformationMatrix());
		m_RootCombinedTransformationMatrix = m_CombinedTransformationMatrix;
	}
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, TransformationMatrix * XMMatrixIdentity());

}

void CHierarchyNode::SetUp_Parent(CHierarchyNode* pNode)
{
	m_pParent = pNode;
	Safe_AddRef(pNode);
}

void CHierarchyNode::Scaling(_float _fScale)
{
	for (auto& pNode : m_AnimBones)
	{
		vector<KEYFRAME*> vKeyFrames = pNode->Get_KeyFrames();
		for (auto& pKeyFrame : vKeyFrames)
		{
			pKeyFrame->vScale.x = _fScale;
			pKeyFrame->vScale.y = _fScale;
			pKeyFrame->vScale.z = _fScale;
		}
	}
}

void CHierarchyNode::ResetScaling()
{
	for (auto& pNode : m_AnimBones)
	{
		vector<KEYFRAME*> vKeyFrames = pNode->Get_KeyFrames();
		for (auto& pKeyFrame : vKeyFrames)
		{
			pKeyFrame->vScale.x = 1.f;
			pKeyFrame->vScale.y = 1.f;
			pKeyFrame->vScale.z = 1.f;
		}
	}
}

CHierarchyNode * CHierarchyNode::Create(const char* pNodeName, const char* pParentNodeName, _cmatrix OffSetMatrix, _cmatrix TransformationMatrix)
{
	CHierarchyNode*	pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct(pNodeName, pParentNodeName, OffSetMatrix, TransformationMatrix)))
	{
		MSGBOX("Failed to Created CHierarchyNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);	
}

