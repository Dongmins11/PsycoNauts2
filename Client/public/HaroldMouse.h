#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#define MAX_KEY 5

BEGIN(Engine)
class CTexture;
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CPhysXCollider;
class CNavigation;
class CCharacterController;
class CPhysXScene_Manager;
END


BEGIN(Client)
class CHaroldMouse final : public CGameObject
{
private:
	enum LOOK_STATE {
		LOOK_FRONT, LOOK_BACK,
		LOOK_RIGHT, LOOK_RIGHT_UP, LOOK_RIGHT_DOWN,
		LOOK_LEFT, LOOK_LEFT_UP, LOOK_LEFT_DOWN,
		LOOK_END
	};
private:
	enum KEY_STATE { KEY_PRESING, KEY_UP, KEY_DOWN, KEY_END };
private:
	typedef struct tagKeyDesc
	{
		_uint iKey[MAX_KEY];
		_uint eKeyState[MAX_KEY];
		_uint iKeyCount;
		_uint iOnceKey;
		_uint eOneceKeyState;
	}KEYDESC;
private:
	CHaroldMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHaroldMouse(const CHaroldMouse& rhs);
	virtual ~CHaroldMouse() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();
private:
	HRESULT	Key_Check(_double TimeDelta);

private:
	HRESULT Brian_Washing(_double TimeDelta);
	HRESULT Cut_Scene_Anim(_double TimeDelta);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CPhysXCollider*			m_pColliderCom = nullptr;
	CCollider*				m_pAABBCom = nullptr;
	CCollider*				m_pOBBCom = nullptr;
	CCharacterController*	m_pCharacterController = nullptr;

private:
	_uint					m_iAnimIndex = 0;
	_uint					m_iKeyStack = 0;
	KEYDESC					m_tAnimDesc;

private:	/*For.KeyState */
	LOOK_STATE				m_eCurrentLookState = LOOK_END;
	LOOK_STATE				m_ePreLookState = LOOK_END;

private: /* For. RotationLerp */
	_float3				m_vSourRotation;
	_float3				m_vDestRotation;
	_float3				m_vDestBackUp;
	_float				m_fRatio = 0.f;
	_float				m_fTime = 0.f;
	_double				m_lfLookLerpSpeed = 0.0;

private: /* For. MoveValue */
	_double				m_lfMoveSpeed = 0.0;

private:	/* For.RotationLerpCheck */
	_bool				m_bIsLefp = false;
	_bool				m_bFirstRotationCheck = false;
	_bool				m_bReverse = false;

private:
	_float				m_fHeight = 0.f;

private:	/*For.KeyInputCheck*/
	_bool				m_bKeyInputCheck = false;

private:	/*For.TrigerCheck*/
	_bool				m_bTrigerCheck = false;

private:	/* For.CutScene */
	_bool				m_bCutSceneStart = false;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_TargetBoneMatrices(const char* pBoneName);

public:
	static CHaroldMouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END