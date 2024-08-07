#pragma once

#include "Component.h"
#include "VIBuffer.h"
#include "Animation.h"
BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_EFFECT, TYPE_END };
public:
	typedef struct tagNodeNameDesc
	{
		char*		szFinderBoneName;
		char*		szRootBoneName;
		char*		szBottomBoneName;
	}NODENAMEDESC;

	typedef struct tagModelMeshDataDesc
	{
		_float3* pVerticesPos = nullptr;
		FACEINDICES32* pIndicies = nullptr;
		_uint iVerticesSize = 0;
		_uint iIndiciesSize = 0;
	}MODELMESHDATA;

protected:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_float4x4 Get_OffsetMatrix(const char* pBoneName);
	_float4x4 Get_PivotMatrix() {
		return m_PivotMatrix;
	}
	const _float4x4* Get_CombinedMatrixPtr(const char* pBoneName);
	_vector Get_MiddlePos();

public:/* For.Set_AnimIndex */
	HRESULT Set_Animation(_uint iAnimationIndex);
	HRESULT Set_RootAnimation(_uint iAnimationIndex);
	HRESULT Set_UpperAnimation(_uint iAnimationIndex);
	HRESULT Set_BottomAnimation(_uint iAnimationIndex);

#pragma region 애니메이션 GET
public: /* For.Get_AnimCurIndex */
	_uint Get_AnimIndex() const { return m_iCurrentAnimation; }
	_uint Get_RootAnimIndex() const { return m_iRootCurAnim; }
	_uint Get_UpperAnimIndex() const { return m_iUpperCurAnim; }
	_uint Get_BottomAnimIndex() const { return m_iBottomCurAnim; }

public: /* For.Get_AnimPreIndex */
	_uint Get_PreAnimIndex() const { return m_iPreAnimation; }
	_uint Get_RootPreAnimIndex() const { return m_iRootPreAnimation; }
	_uint Get_UpperPreAnimIndex() const { return m_iUpperPreAnimation; }
	_uint Get_BottomPreAnimIndex() const { return m_iBottomPreAnimation; }

public: /* For.Get_AnimationCurrentFrame*/
	_uint	Get_AnimationCurrentFrame(_uint iAnimationIndex) const { return m_Animations[iAnimationIndex]->Get_FrameCurrentIndex(); }
	_uint	Get_UpperAnimationCurrentFrame(_uint iAnimationIndex) const { return m_UpperAnimations[iAnimationIndex]->Get_FrameCurrentIndex(); }
	_uint	Get_RootAnimationCurrentFrame(_uint iAnimationIndex) const { return m_RootAnimations[iAnimationIndex]->Get_FrameCurrentIndex(); }
	_uint	Get_BottomAnimationCurrentFrame(_uint iAnimationIndex) const { return m_BottomAnimations[iAnimationIndex]->Get_FrameCurrentIndex(); }
	_uint	Get_AnimationCurrentKeyFrame() const;

public:/*For.Get_AnimBlendSpeed*/
	_double Get_UpperAnimBlendSpeed(_uint iIndex) {
		return m_UpperAnimations[iIndex]->Get_BlendSpeed();
	}
	_double Get_RootAnimBlendSpeed(_uint iIndex) {
		return m_RootAnimations[iIndex]->Get_BlendSpeed();
	}
	_double Get_BottomAnimBlendSpeed(_uint iIndex) {
		return m_BottomAnimations[iIndex]->Get_BlendSpeed();
	}
	_double Get_AnimBlendSpeed(_uint iIndex) {
		return m_Animations[iIndex]->Get_BlendSpeed();
	}



public:/*For.Get_AnimSpeed*/
	_double Get_UpperAnimSpeed(_uint iIndex) {
		return m_UpperAnimations[iIndex]->Get_AnimSpeed();
	}
	_double Get_RootAnimSpeed(_uint iIndex) {
		return m_RootAnimations[iIndex]->Get_AnimSpeed();
	}
	_double Get_BottomAnimSpeed(_uint iIndex) {
		return m_BottomAnimations[iIndex]->Get_AnimSpeed();
	}
	_double Get_AnimSpeed(_uint iIndex) {
		return m_Animations[iIndex]->Get_AnimSpeed();
	}

public:/* For.Get_AnimFinishied */
	_bool	Get_IsAnimFinishied(_uint _iIndex) {
		return m_Animations[_iIndex]->Get_IsFinished();
	}
	_bool	Get_IsUpperAnimFinishied(_uint _iIndex) {
		return m_UpperAnimations[_iIndex]->Get_IsFinished();
	}
	_bool	Get_IsRootAnimFinishied(_uint _iIndex) {
		return m_RootAnimations[_iIndex]->Get_IsFinished();
	}
	_bool	Get_IsBottomAnimFinishied(_uint _iIndex) {
		return m_BottomAnimations[_iIndex]->Get_IsFinished();
	}

public:/* For.Get_AnimNextFinishied */
	_bool	Get_AnimNextFinish(_uint _iIndex) {
		return m_Animations[_iIndex]->Get_NextFinish();
	}
	_bool	Get_UpperAnimNextFinish(_uint _iIndex) {
		return m_UpperAnimations[_iIndex]->Get_NextFinish();
	}
	_bool	Get_RootAnimNextFinish(_uint _iIndex) {
		return m_RootAnimations[_iIndex]->Get_NextFinish();
	}
	_bool	Get_BottomAnimNextFinish(_uint _iIndex) {
		return m_BottomAnimations[_iIndex]->Get_NextFinish();
	}

public: /* For.AnimLerp_Check */
	_bool	Get_IsAnimLerp() {
		return m_bAnimLefp;
	}
	_bool	Get_IsUpperLerp() {
		return m_bUpperIsLefp;
	}
	_bool	Get_IsRootLerp() {
		return m_bRootIsLefp;
	}
	_bool	Get_IsBottomLerp() {
		return m_bBottomIsLefp;
	}


#pragma endregion

#pragma region 애니메이션 SET


public:/*For.Set_AnimLefp*/
	void Set_IsAnimLefp(_bool _bCheck) {
		m_bAnimLefp = _bCheck;
	}
	void Set_IsRootLefp(_bool _bCheck) {
		m_bRootIsLefp = _bCheck;
	}
	void Set_IsUpperLefp(_bool _bCheck) {
		m_bUpperIsLefp = _bCheck;
	}
	void Set_IsBottomLefp(_bool _bCheck) {
		m_bBottomIsLefp = _bCheck;
	}

public:/*For.NextFinished*/
	void Set_IsNextFinished(_bool _bCheck, _uint _iIndex) {
		m_Animations[_iIndex]->Set_NextFinish(_bCheck);
	}
	void Set_IsUpperNextFinished(_bool _bCheck, _uint _iIndex) {
		m_UpperAnimations[_iIndex]->Set_NextFinish(_bCheck);
	}
	void Set_IsRootNextFinished(_bool _bCheck, _uint _iIndex) {
		m_RootAnimations[_iIndex]->Set_NextFinish(_bCheck);
	}
	void Set_IsBottomNextFinished(_bool _bCheck, _uint _iIndex) {
		m_BottomAnimations[_iIndex]->Set_NextFinish(_bCheck);
	}

public:/*For.Set_Loop*/
	void Set_IsLoop(_bool _bCheck, _uint _iMaxTickCount) {
		m_Animations[m_iBottomCurAnim]->Set_IsLoop(_bCheck, _iMaxTickCount);
	}
	void Set_IsUpperLoop(_bool _bCheck, _uint _iMaxTickCount) {
		m_UpperAnimations[m_iUpperCurAnim]->Set_IsLoop(_bCheck, _iMaxTickCount);
	}
	void Set_IsRootLoop(_bool _bCheck, _uint _iMaxTickCount) {
		m_RootAnimations[m_iUpperCurAnim]->Set_IsLoop(_bCheck, _iMaxTickCount);
	}
	void Set_IsBottomLoop(_bool _bCheck, _uint _iMaxTickCount) {
		m_BottomAnimations[m_iBottomCurAnim]->Set_IsLoop(_bCheck, _iMaxTickCount);
	}

public:/*For.Set_AnimBlendSpeed*/     ////////* Anim_BlendSpeed_LayOut *///////////
	void Set_UpperAnimBlendSpeed(_uint iIndex, _double AnimSpped) {
		m_UpperAnimations[iIndex]->Set_BlendSpeed(AnimSpped);
	}
	void Set_RootAnimBlendSpeed(_uint iIndex, _double AnimSpped) {
		m_RootAnimations[iIndex]->Set_BlendSpeed(AnimSpped);
	}
	void Set_BottomAnimBlendSpeed(_uint iIndex, _double AnimSpped) {
		m_BottomAnimations[iIndex]->Set_BlendSpeed(AnimSpped);
	}
	void Set_AnimBlendSpeed(_uint iIndex, _double AnimSpped) {
		m_Animations[iIndex]->Set_BlendSpeed(AnimSpped);
	}


public:/*For.Set_AnimSpeed*/		////////* Anim_Speed_LayOut *//////
	void Set_UpperAnimSpeed(_uint iIndex, _double AnimSpped) {
		m_UpperAnimations[iIndex]->Set_AnimSpeed(AnimSpped);
	}
	void Set_RootAnimSpeed(_uint iIndex, _double AnimSpped) {
		m_RootAnimations[iIndex]->Set_AnimSpeed(AnimSpped);
	}
	void Set_BottomAnimSpeed(_uint iIndex, _double AnimSpped) {
		m_BottomAnimations[iIndex]->Set_AnimSpeed(AnimSpped);
	}
	void Set_AnimSpeed(_uint iIndex, _double AnimSpped) {
		m_Animations[iIndex]->Set_AnimSpeed(AnimSpped);
	}

public:/*For. AnimBoneScailing*/
	void BoneScailing(char* BoneName, _float _fScale);

public:/*For. NextAnimation*/
	void Set_NextAnimation();

public: /* For.Animation_Reset */
	void	Set_ResetAnimation(_uint iAnimIndex) { m_Animations[iAnimIndex]->Reset_Animation(); }
	void	Set_ResetAnimation_KeyFrame(_uint iAnimIndex) { m_Animations[iAnimIndex]->Reset_Animation_KeyFrame(); }

public: /* For.Animation_Duration*/
		const _double	Get_Animation_Duration() { return m_Animations[m_iCurrentAnimation]->Get_Duration(); }
#pragma endregion

public:
	virtual HRESULT NativeConstruct_Prototype(char* pModelFilePath, char* pFilePathSpec, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);

public:/* For.Nomal_Update */
	HRESULT Update(_double TimeDelta);
	HRESULT	Update_Finishied(_double TimeDelta);

public:/* For.Division_Update*/
	HRESULT Root_Update(_double TimeDelta, _bool bRootLockCheck = false);
	HRESULT	Upper_Update(_double TimeDelta);
	HRESULT Bottom_Update(_double TimeDelta);
	HRESULT	Lerp_Update(_double TimeDelta);
	HRESULT SetKeyFrame_Update(_double TimeDelta, _uint iCurKeyFrameIndex, _bool* bGetTheFinishied); /* 키프레임넣으면 현재넣은 키프레임 업데이트만돔 러프X */



public:/* For.Render */
	HRESULT Render(_uint iPassIndex);
	HRESULT Render(_uint iPassIndex,_uint iMaterialCullIndex);
	HRESULT Render_Custom(_uint* ArrRenderPass, _uint ArrSize);/*메쉬 컨테이너마다 패스 구별 */
	HRESULT Render_Custom_Light(_uint* ArrRenderPass, _uint ArrSize, _bool* pEmissiveActive);/*메쉬 컨테이너마다 패스 구별 */
	HRESULT Render_IsMaterialMapping(_uint iPassIndex, _bool* ArrMaterial);/*메쉬 컨테이너마다 텍스쳐 입힐지 선택*/
	HRESULT Render_IsEmissiveMapping(_uint iPassIndex, _bool* ArrEmissive);/*메쉬 컨테이너마다 텍스쳐 입힐지 선택*/
	HRESULT Render(_uint iPassIndex,_uint iMeshContainerIndex, _uint iDesc_PassIndex, TYPE eType);
	HRESULT Render(_uint iPassIndex, _uint iSeparatePassIndex,_bool MeshType, const char* pName = nullptr);/* 0번째 인자: 기본 인덱스 1:원하는 Pass  2: Anim T STATIC F  3:어떤 이름을 가지고있는 */

public:
	vector<class CVIBuffer*> Get_MeshContainers() { return m_MeshContainers; }
	const MODELMESHDATA Get_MeshColliderDatas();
	_float Get_Mesh_Range();
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT Set_TextureValue(const char* pConstantName, ID3D11ShaderResourceView* pSRV);

public:
	class CHierarchyNode* Find_HierarchyNodes(const char* pNodeName);

private:
	TYPE					m_eMeshType = TYPE_END;
	_float3				m_vMax = { 0,0,0 };
	_float3				m_vMin = { 0,0,0 };
private: /* For.Meshes*/
	_uint								m_iNumMeshes;
	vector<class CVIBuffer*>			m_MeshContainers;
	typedef vector<class CVIBuffer*>	MESHCONTAINERS;
	_float4x4							m_PivotMatrix;

private: /* For.Material */
	_uint								m_iNumMaterials;
	vector<MESHMATERIAL*>				m_Materials;
	typedef vector<MESHMATERIAL*>		MATERIALS;

private: /* For.HierarchyNodes */
	vector<class CHierarchyNode*>			m_HierarchyNodes;
	typedef vector<class CHierarchyNode*>	HIERARCHYNODES;

private: /* For.Animation */
	_uint									m_iNumAnimations = 0;
	_uint									m_iCurrentAnimation = 0;
	_uint									m_iPreAnimation = 0;
	vector<class CAnimation*>				m_Animations;
	typedef vector<class CAnimation*>		ANIMATIONS;

private: /* For.Division_Animation */
	vector<class CAnimation*>				m_UpperAnimations;
	vector<class CAnimation*>				m_RootAnimations;
	vector<class CAnimation*>				m_BottomAnimations;

private:/* For.Division_HierarchyNode */
	vector<class CHierarchyNode*>			m_PlayerUppperNodes;
	vector<class CHierarchyNode*>			m_PlayerRootNodes;
	vector<class CHierarchyNode*>			m_PlayerBottomNodes;

private: /* For.AnimIndex */
	_uint									m_iRootCurAnim = 0;
	_uint									m_iRootPreAnimation = 0;
	_uint									m_iUpperCurAnim = 0;
	_uint									m_iBottomCurAnim = 0;
	_uint									m_iUpperPreAnimation = 0;
	_uint									m_iBottomPreAnimation = 0;
	_uint									m_iRootPastAnim = 0;
	_uint									m_iUpperPastAnim = 0;
	_uint									m_iBottomPastAnim = 0;

private:/* For.LefpCheck */
	_bool									m_bAnimLefp = false;
	_bool									m_bRootIsLefp = false;
	_bool									m_bUpperIsLefp = false;
	_bool									m_bBottomIsLefp = false;

private:/* For.NodeDesc */
	NODENAMEDESC							m_tNodeNameDesc;

private:/* For.DivisionCheck */
	_bool									m_bDivisionCheck = false;

private:/*For.MotionBlur*/
	_float4x4								m_OldBoneMatrix[300];
	_float4x4								m_OldBoneMatrix2[300];
	_float4x4								m_OldBoneMatrix3[300];
	_uint									m_iMotionBlurUpdateCnt = 0;
protected:
	vector<PASSDESC>					m_Passes;
	ID3DX11Effect*						m_pEffect = nullptr;

private:
	char		m_szFilePathSpec[MAX_PATH] = "";

private:
	HRESULT Create_Shader();
	HRESULT Division_HierachyNode(void* pArg);
private:
	HRESULT Compile_Shader(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);


public:  /* Device , DeviceContext , 메쉬전체경로 ex) ../../Mesh/Test.dat , 메쉬폴더경로 ex)../../Mesh/, PivotMatrix*/
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, char* pModelFilePath, char* pFilePathSpec, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END