#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;

END

BEGIN(Client)
class CPlayer_CutScene final : public CGameObject
{
public:
		/* For.Totu_AnimationIndex */	
	enum PLAYER_TUTO_ANIM{ TOTU_ONE, TOTU_TWO, TOTU_THREE, TOTU_FOR, TOTU_FIVE , TOTU_END };

private:
	CPlayer_CutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer_CutScene(const CPlayer_CutScene& rhs);
	virtual ~CPlayer_CutScene() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:/*For.Set*/
	void Set_Active() { m_IsAnimationPlay = true; }
	void Set_Enable_Stage2(_bool bEnable) { m_IsAnimationPlay_StageTwo = bEnable; }
public:
	void	Punch() { ++m_iPunchCount; }
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

private:
	_bool					m_IsAnimationPlay = false;
	_bool					m_IsAnimationPlay_StageTwo = false;
	_bool					m_bLerpCheck = false;
	_uint					m_iAnimIndex = TOTU_END;
	_double					m_lfRadianValue = 0.0;
	_double					m_lfTimeAcc = 0.0;
	_double					m_dAnimSpeed = 1.0;
	_uint					m_iPunchCount = 0;
	_float3					m_vBackUpPos;
	_double					m_dDecreaseY = 0.0;
	_double					m_dEndTimeAcc = 0.0;

private:
	_uint					m_iCurScene = 0;
private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();

private:	/* For.Stage_TutoCutScene */
	HRESULT Stage_Tuto_CutScene(_double TimeDelta);
	void	Anim_Playing(_double TimeDelta);
	void	Scene_Move(_double TimeDelta);
	void	Lerp_Check();
	void	Animation_IndexCheck(_uint iAnimIndex);
	void	Set_Animation_BlendSpeed(_uint iMaxIndex);


public:
	static CPlayer_CutScene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END