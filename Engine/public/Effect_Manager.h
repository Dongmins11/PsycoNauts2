#pragma once

#include "Base.h"

BEGIN(Engine)
class CEffect_Manager final : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
public:
	enum EFFECT_TYPE { TYPE_RECT, TYPE_MESH, TYPE_PARTICLE, TYPE_TOOL_RECT, TYPE_END };
	enum MESH_PASS {
		PASS_ALPHA_TEST, PASS_ALPHA_MAP, PASS_ALPHA_ALPHAANI, PASS_ALPHA_DIFFUSEANI,
		PASS_CHOP_EFFECT, PASS_LEAP_ATTACK_EFFECT, PASS_DIZZISTAR, PASS_CHARGE_SINGLE_WAVE, PASS_CHARGE_WAVE,
		PASS_IMPACT_WAVE, PASS_PSI_BLAST, PASS_DONUTRING, PASS_PURSERING, PASS_WATERWAVE, PASS_BRAINSTORM,
		PASS_SILDINGMESH, PASS_BALLOON, PASS_WATER, PASS_BLENDWAVE, PASS_END
	};

private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;
	
public:
	_bool Get_Ball_Confirm(_uint iIndex) { return m_isBall[iIndex]; }

public:
	void Set_Ball_Confirm(_uint iIndex, _bool isConfirm) { m_isBall[iIndex] = isConfirm; }

public: /* 상속 관련 */
	HRESULT Effect_Call_Inheritance(_uint iLayerSceneIndex, _bool isOnce, _bool isNonBillboard, class CTransform* pTransform, const _tchar* pLayerTag);
	HRESULT Effect_Call_WorldPos(_uint iLayerSceneIndex, const _tchar* pLayerTag, _vector vPos, _bool isWorldPosEnable);
	HRESULT Effect_ParentBoneMatrix(_uint iLayerSceneIndex, const _tchar* pLayerTag, class CTransform* pTransform, class CModel* pModel, const char* pBoneName);

public: /* 생성 관련 */
	HRESULT	Create_Prototype_Effect_Data(EFFECT_TYPE eType, _uint iPrototypeEffectSceneIndex, const _tchar* pEffectPath, const _tchar* pEffectTag);
	HRESULT	Create_Prototype_Effect_Particle_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iPrototypeEffectSceneIndex, const _tchar* pEffectPath, const _tchar* pEffectTag);

public: /* 이펙트 속도 관련 */
	void Setting_Effect_Speed(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _double EffectSpeed);
	void Setting_Effect_UVSprite(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _double SprtieTime);
	void Setting_Effect_MovePos(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _vector vMovePos);
	void Setting_Effect_TransformPos(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _uint iType, _float fScale = 1.f);
	void Setting_Effect_Scale(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _vector vScale);
	void Setting_Effect_NonRotation(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _bool isNonRotation);
	void Setting_Effect_LocalBillboard(_uint iLayerSceneIndex, const _tchar* pLayerTag, const _tchar* pComTag, _bool isLocalBillboard);

private:
	class CObject_Manager* m_pObject_Manager = nullptr;
	class CComponent_Manager* m_pComponent_Manager = nullptr;

private:
	list<const TCHAR*>	m_listEffectTag;

	_double				m_EffectSpeed = 1.0;

	_bool				m_isBall[8] = { false, false, false, false, false, false, false, false };

public:
	virtual void Free() override;
};

END