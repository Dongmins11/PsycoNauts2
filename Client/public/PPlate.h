#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CPhysXCollider;
END

BEGIN(Client)

#define BUTTON_DOWNIDLE 0
#define BUTTON_DOWNANIM 1
#define BUTTON_UPIDLE 2
#define BUTTON_UPANIM 3

class CPPlate final :public CGameObject
{
public:
	enum BUTTON_STATE
	{
		BUTTON_STATE_DOWNIDLE,
		BUTTON_STATE_DOWNANIM,
		BUTTON_STATE_UPIDLE,
		BUTTON_STATE_UPANIM,
		BUTTON_STATE_END
	};

	enum BUTTON_TYPE
	{
		BUTTON_TYPE_KNIFE,
		BUTTON_TYPE_FRUIT,
		BUTTON_TYPE_MIXER,
		BUTTON_TYPE_END
	};

protected:
	explicit CPPlate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPPlate(const CPPlate& rhs);
	virtual ~CPPlate() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int    Tick(_double TimeDelta);
	virtual _int    LateTick(_double TimeDelta);
	virtual HRESULT Render();

public: /*For. Get_Button */
	const _bool Get_ButtonUpStateCheck() const { return m_IsUpStateCheck; }
	const _uint Get_ButtonType() const { return m_eButtonType; }

public: /*For. Set_Button */
	void	Set_ButtonUpStateCheck(BUTTON_TYPE eButtonType, _bool bUpStateCheck)
	{
		m_eButtonType = eButtonType;
		m_IsUpStateCheck = bUpStateCheck;
	}
	//나의 버튼 종류와 버튼을 키겠다는 true 신호만주면 현재클래스내부의 트리거충돌함수에서 내가쓸것이 분기가가능함 
	//true 한번만신호주면 내부적으로 false 바꿔줌

private:
	virtual void OnTriggerEnter(CCollisionObject& collision);

private:
	CRenderer*				m_pRendererCom = nullptr;

	CTransform*				m_pTransformCom = nullptr;

	CModel*					m_pModelCom = nullptr;

	CPhysXCollider*			m_pColliderCom = nullptr;

private:
	_float4x4				m_WorldMatrix;
	_float4x4				m_OffsetMatrix;
	_float4x4				m_PivotMatrix;
	const _float4x4*		m_pCombinedMatrix = nullptr;
private:
	_float4x4				m_ColiderMatirx = {};
	_float4x4*				m_pBoneMatrix = nullptr;

private:
	_uint					m_iCurAnimIndex = 0;
	_uint					m_iPreAnimIndex = 0;

	BUTTON_STATE			m_eButtonState = BUTTON_STATE_END;
	BUTTON_TYPE				m_eButtonType = BUTTON_TYPE_END;

private:
	_bool					m_IsUpStateCheck = false;
	_bool					m_isMoveScene = false;
private:
	_uint					m_iMyCount = 0;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT SetUp_MyBoneMatrices(const char* pBoneName);
private:
	HRESULT	Colider_PosUpdate();
	HRESULT Bone_MatrixCalculation();

private:
	HRESULT	PPlate_Animation();
	HRESULT	Animation_StateCheck();

private: /* For.Button_Interaction */
	HRESULT Button_KnifeType_Interaction();
	HRESULT Button_MoveStage();
	HRESULT Button_BlenderStart();
public:
	static CPPlate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iCurrentScene);
	virtual void Free()override;
};

END