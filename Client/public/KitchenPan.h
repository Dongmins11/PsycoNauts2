#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "EnviromentTrigger.h"
#include "Fruit.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CKitchenPan final : public CGameObject
{
private:
	explicit CKitchenPan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CKitchenPan(const CKitchenPan& rhs);
	virtual ~CKitchenPan() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg, _uint iCurrentScene);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	class CFruit* Get_Pan_Fruit() { return m_pTargetFruit; }
	void  Set_Pan_Fruit(class CFruit* pFruit) { m_pTargetFruit = pFruit; }
	void Set_RenderFire() { m_bRandStart = false; }
	void Set_IsCookFinished(_bool isFinished) { m_IsCookFinished = isFinished; }
private:
	HRESULT SetUp_Trigger();
	void Move_Pan(_double TimeDelta);
	void Rand_Make(_double TimeDelta);

private:
	CModel*					m_pModelCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CGameObject*			m_pEnvironTrigger = nullptr;

public:
	void Set_Move(_bool IsMove, _int iIndex) { m_IsMove = IsMove, m_iPosIndex += iIndex; }

private:
	EnviromentTriggerType m_eType = EnviromentTriggerType::ETTYPE_COOKING;
	// �Ķ����Ұ� ���� ��ȣ�ۿ�
	_bool	m_IsCollision = false;			// �Ķ����Ұ� ���� ��ġ�� �ִ��� �Ǵ�
	_bool	m_IsMove = false;				// �÷��̾ ��ư�� �������� True
	_bool	m_IsCookFinished = false;		// �丮�� �������� (m_dCookFinishedTimeAcc ���� m_dCollisionTimeAcc �� Ŀ���� True��ȯ)
	_int	m_iPosIndex = 2;				// ���� �Ķ����� ��ġ �ε���

	_double	m_dCollisionTimeAcc = 0.0;		// �Ķ����Ұ� ���� ��ġ�� �ð�
	_double m_dCookFinishedTimeAcc = 10.0;	// ���� �Ķ����Ұ� ���� ���� �丮�� �ϼ��ɶ����� �ɸ��� �ð�

	// �Ķ������� �⺻���� ����
	_float3 m_iPanPos[5] = {};		// �Ķ������� �̵������� ��ġ
	_float4x4 m_WorldMatrix;		// Trigger�� ����� ���� �������

	// ����
	_bool	m_bRandStart = true;	// ó�� ������ ������ 5�ʸ��� ������ ����� ����
	_uint	m_iRandIndex = 0;		// ���� �ε���
	_double m_dRandTimer = 0.0;		// ������ ���� ����� �ð� ���� Ÿ�̸�(5�ʿ� �ѹ��� ���� ����)
	_float3 m_vecObjectPos = { 0.f,0.f,0.f };
private:
	CFruit*					m_pTargetFruit = nullptr;

private:
	HRESULT SetUp_Component(_float3 vColSize, _float3 vColPos, _float3 vObjScale);
	HRESULT SetUp_ConstantTable();

public:
	static CKitchenPan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg, _uint iSceneIndex) override;
	virtual void Free() override;
};

END