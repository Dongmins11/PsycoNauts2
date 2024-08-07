#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CSkill_UI final : public CGameObject
{
private:
	explicit CSkill_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSkill_UI(const CSkill_UI& rhs);
	virtual ~CSkill_UI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_BrainStorm() { m_bKeyCheck[0] = true; }
	void Set_Pisblast() { m_bKeyCheck[1] = true; }
	void Set_AllSkillRenderSet(_bool bCheck) {
		for (_uint i = 0; i < 6; i++) { m_bRenderCheck[i] = bCheck; }
	}
	void Set_SkillRenderSet(_uint iIndex, _bool bCheck) { m_bRenderCheck[iIndex] = bCheck; }

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureSkillCom = nullptr;
	CTexture*			m_pTextureBGCom = nullptr;
	CTexture*			m_pTextureKeyCom = nullptr;


	CVIBuffer_Rect*		m_pModelSkillCom[6] = {};
	CVIBuffer_Rect*		m_pModelBGCom[6] = {};
	CVIBuffer_Rect*		m_pModeyKeyCom[6] = {};

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:
	_bool				m_bRenderCheck[6] = {};		// 0 : Q, 1 : E  2 : H  3 : RB 4: Q 5 : T
	_bool				m_bKeyCheck[6] = {};			// 각자 키가 눌렸는지 
	_float				m_fTimeAcc[6] = {};				// 각자 키가 눌린 시간
	_float				m_fX, m_fY, m_fSkillSize, m_fKeySize, m_fBGSize;
	_float4x4			m_ProjMatrix;

public:
	static CSkill_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene)override;
	virtual void Free()override;
};

END