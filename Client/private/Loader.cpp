#include "stdafx.h"
#include "..\public\Loader.h"
#include "GameInstance.h"

#include "Camera_Fly.h"
#include "Navigation.h"
#include "Static_Mesh.h"
#include "Background.h"
#include "Skill_UI.h"
#include "Plane.h"
#include "NpcDoor.h"
#include "DoubleDoorBlocker.h"
#include "MoveCabinet.h"
#include "AnimatedShelv.h"
#include "FallObject.h"
#include "DoorZipper.h"
#include "DemoRect.h"
#include "OfficeWalls.h"
#include "FruitSpawner.h"
#include "Not_AFFECTED_Render.h"
#include "PlatingPlate_Phase0.h"
#include "PlatingPlate_Phase1.h"
#include "PlatingPlate_Phase2.h"
#include "PsyBLExit.h"
#include "AWaterCooler.h"
#include "PyroPainting.h"
#include "FadeInOut.h"
#include "Blade.h"
#include "HippoLegs.h"
#include "BlenderPath.h"
#include "Blender2.h"
#include "RailSlide.h"
#include "Blender.h"
#include "Griddle.h"
#include "StudioCam.h"
#include "Comput_Vent.h"
#include "Treasure.h"
#include "LightBar.h"
#include "SS_Cuff.h"
#include "HostZone.h"
#include "StationRamp0.h"
#include "StationRamp1.h"
#include "Cloud.h"
#include "VentDoor.h"
#include "Stage1File_Cabinet.h"
#include "Zip.h"
#include "Stage4Mercial.h"
#include "INGHole.h"
#include "GiantCloche.h" 
#include "Stage3BossDish.h"
#include "CookingEnd.h"
#include "BossWall.h"


#pragma region Fruit
#include "Egg.h"
#include "AWaterMelon.h"
#include "BeeHive.h"
#include "Bread.h"
#include "Onion.h"
#include "Pork.h"
#include "Strawberry.h"
#include "FruitCheckTrigger.h"
#include "CookingUI.h"
#include "TimerUI.h"
#include "CookingProgressBarUI.h"
#pragma endregion


#pragma region NPC
#include "Npc.h"
#include "Loboto.h"
#include "MoveNpc.h"
#include "Tuto_ScriptUI.h"
#include "NpcCoach.h"
#include "NpcSasha.h"
#include "NpcLoboto.h"
#include "QuestManager.h"
#include "QuestScriptUI.h"
#include "QuestUI.h"
#include "EndingObject.h"
#include "EndingUI.h"
#pragma endregion

#pragma region 피직스_트리거
#include "PhysXScene_Manager.h"
#include "PlayerTrigger_Sphere.h"
#include "WallCollision.h"
#pragma endregion

#pragma region 몬스터
#include "BadIdea.h"
#include "Censor.h"
#include "Censor_Burly.h"
#include "Doubt.h"
#include "ToothFairy.h"
#include "Judge.h"
#include "Denture.h"
#include "Boss_Goat.h"
#include "Tooth.h"
#include "Bulb.h"
#include "Cracked_Denture.h"
#include "Judge_Weapon.h"
#include "Judge_Book.h"
#include "Judge_Weaponhead.h"
#include "Censor_Spawner.h"
#include "BadIdea_Spawner.h"
#include "ZiantLoboto.h"
#include "BadIdea_AttTrigger.h"
#include "Censor_AttTrigger.h"
#include "CensorB_AttTriggerL.h"
#include "CensorB_AttTriggerR.h"
#include "Judge_AttTrigger.h"
#include "JudgeWeapon_AttTrigger.h"
#include "Monster_HpBar.h"
#include "Puke_Laser.h"
#include "NonBT_Ford.h"
#include "NonBT_Hollis.h"
#include "NonBT_Otto.h"
#include "MonsterInfo.h"
#include "Blob.h"
#include "Fruits.h"
#include "Puke_Ball.h"
#include "Board_R.h"
#include "Board_L.h"
#include "BossHpBar.h"
#pragma endregion

#pragma region 이펙트
/* Rect */
#include "RectEffect.h"
#include "RectEffect_Once.h"
#include "RectEffect_BoneMat.h"
#include "RectEffect_Environment.h"

/* Trail */
#include "Bulb_Trail.h"
#include "Judge_Trail.h"
#include "CencerB_Left_Trail.h"
#include "CencerB_Right_Trail.h"

/* Model */
#include "DonutRing.h"
#include "CencerDonut.h"
#include "JudgeDonut.h"
#include "BossDonut.h"
#include "ChopA.h"
#include "ChopB.h"
#include "ChopC.h"
#include "TumbleAttack.h"
#include "ChargeSingleWave.h"
#include "CencerCSW.h"
#include "ChargeWave.h"
#include "ImpactWave.h"
#include "DizziStar.h"
#include "SpinSwipe.h"
#include "PurseRing.h"
#include "SlidingMesh.h"
#include "Not_Affected_Render.h"
#include "Player_CutScene.h"
#include "Static_Bell.h"
#include "Balloon.h"
#include "Water.h"
#include "WaterWave.h"
#include "BlenderWave.h"
#include "BlenderHead.h"

/* Particle */
#include "Effect_PointParticle.h"
#include "Effect_EnviroParticle.h"
#include "Effect_RepeatParticle.h"
#include "Effect_RectParticle.h"
#include "Effect_RectRepeat.h"
#include "Effect_RectClone.h"
#include "SpinHole.h"
#pragma endregion

/* TEST 1: 쥐버튼/TutorialUI/PlayerHealthBar/Trampoline */
#include "Stage02Button.h"
#include "Tuto_MessageUI.h"
#include "PlayerHPBar.h"
#include "Trampoline.h"

// stage2 2D
#include "Lock_Object.h"
#include "Player_2D.h"
#include "Monster_2D.h"
#include "Plane_2D.h"
#include "Key_2D.h"
#include "Lock_2D.h"
#include "Skill_Info.h"

/* ChopPig*/
#include "Choppig_PullHand.h"
#include "Choppig_HandKnife.h"

/* PPlate 3스테이지 버튼*/
#include "PPlate.h"

#include "Bell.h"
#include "Bouncer.h"
#include "KitchenPan.h"
#include "KitchenPanButton.h"
#include "GasBurner.h"

/*	Stage5 */
#include "Boss_Dish.h"
#include "Boss_Food_Projectile.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

}

unsigned int APIENTRY EntryMain(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	HRESULT			hr = 0;

	if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		return E_FAIL;

	EnterCriticalSection(pLoader->Get_CriticalSection());



	switch (pLoader->Get_SceneID())
	{
	case SCENE_LOGO:
		hr = pLoader->Loading_ForLogoScene();
		break;
	case SCENE_DEMO:
		hr = pLoader->Loading_ForDemoScene();
		break;
	case SCENE_STAGE01:
		hr = pLoader->Loading_ForStage01();
		break;
	case SCENE_STAGE02:
		hr = pLoader->Loading_ForStage02();
		break;
	case SCENE_STAGE03:
		hr = pLoader->Loading_ForStage03();
		break;
	case SCENE_STAGE04:
		hr = pLoader->Loading_ForStage04();
		break;
	case SCENE_STAGE05:
		hr = pLoader->Loading_ForStage05();
		break;
	case SCENE_ENDING:
		hr = pLoader->Loading_Ending();
		break;
	case SCENE_THREADING:
		hr = pLoader->Loading_DuringPlaying();
		break;
	}

	if (FAILED(hr))
		return 0;

	LeaveCriticalSection(pLoader->Get_CriticalSection());

	CoUninitialize();

	return 0;

}

HRESULT CLoader::NativeConstruct(SCENE eSceneID)
{
	m_eSceneID = eSceneID;

	//if (FAILED(SetUp_Component()))
	//	return E_FAIL;

	InitializeCriticalSection(&m_CriticalSection);

	/* 또다른 코드의 흐름을 만들어준다. */
	/* 호출해야할 진입점함수를 지정해준다.  */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, EntryMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoScene()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	Ready_Model_Data(TEXT("../../Data/Stage0PathFind.dat"));
	Ready_Model_Data(TEXT("../../Data/Stage5PathFind.dat"));
	//Ready_Model_Data(TEXT("../../Data/Stage3PathFind.dat"));
	//Ready_Model_Data(TEXT("../../Data/Stage3FruitPathFind.dat"), _float3(0.0025f, 0.0025f, 0.0025f));
	//Loading_Monster();

	CQuestManager* pQuestMgr = GET_INSTANCE(CQuestManager);
	pQuestMgr->Load_QuestDatas(TEXT("../../Data/Stage01/QuestData1.txt"));
	RELEASE_INSTANCE(CQuestManager);
	Loading_Stage03_StaticObjects();

#pragma region 이펙트 프로토타입 모델
	/* For. RectEffect */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect"), CRectEffect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. RectEffect_Once */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_Once"), CRectEffect_Once::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. RectEffect_BoneMat */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_BoneMat"), CRectEffect_BoneMat::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. RectEffect_Environment */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_RectEffect_Environment"), CRectEffect_Environment::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. RectInsctance */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectParticle"), CEffect_RectParticle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. RectRepeat */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectRepeat"), CEffect_RectRepeat::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. RectRepeat */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RectClone"), CEffect_RectClone::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. SpinHole */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_SpinHole"), CSpinHole::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Component_VIBuffer_RectInstance_Clone */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_VIBuffer_RectInstance_Clone"), CVIBuffer_RectInstance_Clone::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. PointInsctance */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_PointParticle"), CEffect_PointParticle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. EnvirotInsctance */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_EnviroParticle"), CEffect_EnviroParticle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. RepeatParticle */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Effect_RepeatParticle"), CEffect_RepeatParticle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. DonutRing */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_DonutRing"), CDonutRing::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. CencerDonut */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CencerDonut"), CCencerDonut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. JudgeDonut */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeDonut"), CJudgeDonut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. BossDonut */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BossDonut"), CBossDonut::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. ChopA */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_ChopA"), CChopA::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. ChopB */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_ChopB"), CChopB::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. ChopC */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_ChopC"), CChopC::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. TumbleAttack */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_TumbleAttack"), CTumbleAttack::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. SlidingMesh */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_SlidingMesh"), CSlidingMesh::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. ChargeSingleWave */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_ChargeSingleWave"), CChargeSingleWave::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. CencerCSW */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CencerCSW"), CCencerCSW::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. PurseRing */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_PurseRing"), CPurseRing::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. DizziStar */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_DizziStar"), CDizziStar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Balloon */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Balloon"), CBalloon::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Water */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Water"), CWater::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. WaterWave */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_WaterWave"), CWaterWave::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. BlenderWave */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BlenderWave"), CBlenderWave::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. BlenderHead */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BlenderHead"), CBlenderHead::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Bulb_Trail */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Bulb_Trail"), CBulb_Trail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Judge_Trail */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Judge_Trail"), CJudge_Trail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. CencerB_Left_Trail */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CencerB_Left_Trail"), CCencerB_Left_Trail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. CencerB_Right_Trail */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CencerB_Right_Trail"), CCencerB_Right_Trail::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion

#pragma region 이펙트 데이터
	/* Rect */

	/* For. DoubleJump */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/DoubleJump2.dat"), TEXT("Effect_DoubleJump"))))
		return E_FAIL;

	/* For. ChopCCharge3 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/ChopCCharge3.dat"), TEXT("Effect_ChopCCharge3"))))
		return E_FAIL;

	/* For. ChopCImpact */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/ChopCImpact.dat"), TEXT("Effect_ChopCImpact"))))
		return E_FAIL;

	/* For. JumpAtt3 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/JumpAtt3.dat"), TEXT("Effect_JumpAtt3"))))
		return E_FAIL;

	/* For. ChopCRending2 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/ChopCRending2.dat"), TEXT("Effect_ChopCRending2"))))
		return E_FAIL;

	/* For. BlueTumbleAttack */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/BlueTumbleAttack.dat"), TEXT("Effect_BlueTumbleAttack"))))
		return E_FAIL;

	/* For. GrabBrokenRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/GrabBrokenRect.dat"), TEXT("Effect_GrabBrokenRect"))))
		return E_FAIL;

	/* For. HealPackRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/HealPackRect.dat"), TEXT("Effect_HealPackRect"))))
		return E_FAIL;

	/* For. StopEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/StopEffect.dat"), TEXT("Effect_StopEffect"))))
		return E_FAIL;

	/* For. HitEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/HitEffect.dat"), TEXT("Effect_HitEffect"))))
		return E_FAIL;

	/* For. HitEffect2 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/HitEffect2.dat"), TEXT("Effect_HitEffect2"))))
		return E_FAIL;

	/* For. HitEffect3 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/HitEffect3.dat"), TEXT("Effect_HitEffect3"))))
		return E_FAIL;

	/* For. Impact */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/Impact.dat"), TEXT("Effect_Impact"))))
		return E_FAIL;

	/* For. JudgeAttack2 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/JudgeAttack2.dat"), TEXT("Effect_JudgeAttack2"))))
		return E_FAIL;

	/* For. JudgeChargeComplete2 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/JudgeChargeComplete2.dat"), TEXT("Effect_JudgeChargeComplete2"))))
		return E_FAIL;

	/* For. JudgeHead */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/JudgeHead.dat"), TEXT("Effect_JudgeHead"))))
		return E_FAIL;

	/* For. JudgeTakeDown */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/JudgeTakeDown2.dat"), TEXT("Effect_JudgeTakeDown"))))
		return E_FAIL;

	/* For. JudgeCharge2 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/JudgeCharge2.dat"), TEXT("Effect_JudgeCharge2"))))
		return E_FAIL;

	/* For. CencerSpanerRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/CencerSpanerRect.dat"), TEXT("Effect_CencerSpanerRect"))))
		return E_FAIL;

	/* For. HeavyCancerCharge */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/HeavyCancerCharge.dat"), TEXT("Effect_HeavyCancerCharge"))))
		return E_FAIL;

	/* For. HeavyCancerImpact */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/HeavyCancerImpact.dat"), TEXT("Effect_HeavyCancerImpact"))))
		return E_FAIL;

	/* For. BadIdeaSpawn */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/BadIdeaSpawn.dat"), TEXT("Effect_BadIdeaSpawn"))))
		return E_FAIL;

	/* For. BouncerEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/BouncerEffect.dat"), TEXT("Effect_BouncerEffect"))))
		return E_FAIL;

	/* For. FieldSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/FieldSmoke.dat"), TEXT("Effect_FieldSmoke"))))
		return E_FAIL;

	/* For. PukeShoot */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/PukeShoot.dat"), TEXT("Effect_PukeShoot"))))
		return E_FAIL;

	/* For. BallDieRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/BallDieRect.dat"), TEXT("Effect_BallDieRect"))))
		return E_FAIL;

	/* For. SpinWormhole */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/SpinWormhole.dat"), TEXT("Effect_SpinWormhole"))))
		return E_FAIL;

	/* For. FoodHitEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/FoodHitEffect.dat"), TEXT("Effect_FoodHitEffect"))))
		return E_FAIL;

	/* For. QuestOK */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/QuestOK.dat"), TEXT("Effect_QuestOK"))))
		return E_FAIL;

	/* For. MouseHeadIconRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Data(CEffect_Manager::TYPE_RECT, SCENE_STATIC,
		TEXT("../../Data/Effect/Rect/MouseHeadIconRect.dat"), TEXT("Effect_MouseHeadIconRect"))))
		return E_FAIL;

	/* Particle */

	/* For. Smoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Smoke.dat"), TEXT("Effect_Smoke"))))
		return E_FAIL;

	/* For. Smoke_Cencer */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Smoke_Cencer.dat"), TEXT("Effect_Smoke_Cencer"))))
		return E_FAIL;

	/* For. Smoke_Die */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Smoke_Die.dat"), TEXT("Effect_Smoke_Die"))))
		return E_FAIL;

	/* For. StarDieRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/StarDieRect.dat"), TEXT("Effect_StarDieRect"))))
		return E_FAIL;

	/* For. Smoke_JumpAtt */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Smoke_JumpAtt.dat"), TEXT("Effect_Smoke_JumpAtt"))))
		return E_FAIL;

	/* For. MoveCabinetBigRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/MoveCabinetBigRect.dat"), TEXT("Effect_MoveCabinetBigRect"))))
		return E_FAIL;

	/* For. MoveCabinetSmallRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/MoveCabinetSmallRect.dat"), TEXT("Effect_MoveCabinetSmallRect"))))
		return E_FAIL;

	/* For. JumpAttRact */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/JumpAttRact.dat"), TEXT("Effect_JumpAttRact"))))
		return E_FAIL;

	/* For. ChopCAttackRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/ChopCAttackRect.dat"), TEXT("Effect_ChopCAttackRect"))))
		return E_FAIL;

	/* For. Smoke_Gray */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Smoke_Gray.dat"), TEXT("Effect_Smoke_Gray"))))
		return E_FAIL;

	/* For. Spark */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Spark.dat"), TEXT("Effect_Spark"))))
		return E_FAIL;

	/* For. MeshBrokenPoint */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/MeshBrokenPoint.dat"), TEXT("Effect_MeshBrokenPoint"))))
		return E_FAIL;

	/* For. BlueSpark */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/BlueSpark.dat"), TEXT("Effect_BlueSpark"))))
		return E_FAIL;

	/* For. WhiteStarRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/WhiteStarRect.dat"), TEXT("Effect_WhiteStarRect"))))
		return E_FAIL;

	/* For. PaperRect0 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect0"))))
		return E_FAIL;

	/* For. PaperRect1 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect1"))))
		return E_FAIL;

	/* For. PaperRect2 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect2"))))
		return E_FAIL;

	/* For. PaperRect3 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect3"))))
		return E_FAIL;

	/* For. PaperRect4 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect4"))))
		return E_FAIL;

	/* For. PaperRect5 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect5"))))
		return E_FAIL;

	/* For. PaperRect6 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect6"))))
		return E_FAIL;

	/* For. PaperRect7 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect7"))))
		return E_FAIL;

	/* For. PaperRect8 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PaperRect.dat"), TEXT("Effect_PaperRect8"))))
		return E_FAIL;

	/* For. JudgePaperRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/JudgePaperAttackRect.dat"), TEXT("Effect_JudgePaperAttackRect"))))
		return E_FAIL;

	/* For. JudgePaperRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/JudgePaperRect.dat"), TEXT("Effect_JudgePaperRect"))))
		return E_FAIL;

	/* For. TumbleAttackPoint */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/TumbleAttackPoint.dat"), TEXT("Effect_TumbleAttackPoint"))))
		return E_FAIL;

	/* For. HealStarRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/HealStarRect.dat"), TEXT("Effect_HealStarRect"))))
		return E_FAIL;

	/* For. Smoke_Bottom */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Smoke_Bottom.dat"), TEXT("Effect_Smoke_Bottom"))))
		return E_FAIL;

	/* For. Smoke_Bottom */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Smoke_Bulb.dat"), TEXT("Effect_Smoke_Bulb"))))
		return E_FAIL;

	/* For. WaterRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/WaterRect.dat"), TEXT("Effect_WaterRect"))))
		return E_FAIL;

	/* For. SpawnSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/SpawnSmoke.dat"), TEXT("Effect_SpawnSmoke"))))
		return E_FAIL;

	/* For. StarSpownRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/StarSpownRect.dat"), TEXT("Effect_StarSpownRect"))))
		return E_FAIL;

	/* For. JudgeSpawnPaper */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/JudgeSpawnPaper.dat"), TEXT("Effect_JudgeSpawnPaper"))))
		return E_FAIL;

	/* For. JudgeSpawnSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/JudgeSpawnSmoke.dat"), TEXT("Effect_JudgeSpawnSmoke"))))
		return E_FAIL;

	/* For. BurnnerFire */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/BurnnerFire.dat"), TEXT("Effect_BurnnerFire"))))
		return E_FAIL;

	/* For. BurnnerFireSmall */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/BurnnerFireSmall.dat"), TEXT("Effect_BurnnerFireSmall"))))
		return E_FAIL;

	/* For. PigChopRandingSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/PigChopRandingSmoke.dat"), TEXT("Effect_PigChopRandingSmoke"))))
		return E_FAIL;
	
	/* For. SpinPointParticle */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/SpinPointParticle.dat"), TEXT("Effect_SpinPointParticle"))))
		return E_FAIL;

	/* For. Boss_SmokeEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_SmokeEffect.dat"), TEXT("Effect_Boss_SmokeEffectOtto"))))
		return E_FAIL;

	/* For. Boss_SmokeEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_SmokeEffect.dat"), TEXT("Effect_Boss_SmokeEffectFord"))))
		return E_FAIL;

	/* For. Boss_SmokeEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_SmokeEffect.dat"), TEXT("Effect_Boss_SmokeEffectHollis"))))
		return E_FAIL;

	/* For. Boss_SmokeEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_SmokeEffect.dat"), TEXT("Effect_Boss_SmokeEffectSpecula0"))))
		return E_FAIL;

	/* For. Boss_SmokeEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_SmokeEffect.dat"), TEXT("Effect_Boss_SmokeEffectSpecula1"))))
		return E_FAIL;

	/* For. Boss_SmokeEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_SmokeEffect.dat"), TEXT("Effect_Boss_SmokeEffectSpecula2"))))
		return E_FAIL;

	/* For. Boss_FoodDieStar */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_FoodDieStar.dat"), TEXT("Effect_Boss_FoodDieStar"))))
		return E_FAIL;

	/* For. Boss_StarEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/Boss_StarEffect.dat"), TEXT("Effect_Boss_StarEffect"))))
		return E_FAIL;

	/* For. FireEffectRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/FireEffectRect.dat"), TEXT("Effect_FireEffectRect"))))
		return E_FAIL;

	/* For. 2DStarParticle */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/2DStarParticle.dat"), TEXT("Effect_2DStarParticle"))))
		return E_FAIL;

	/* For. FireEffectRect2 */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Particle/FireEffectRect2.dat"), TEXT("Effect_FireEffectRect2"))))
		return E_FAIL;

	/* 환경 파티클 */
	/* For. WhiteSmokeEnv */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/DarkSmokeEnv2.dat"), TEXT("Effect_Env_WhiteSmokeEnv"))))
		return E_FAIL;

	/* For. PickingObject */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/PickingObject.dat"), TEXT("Effect_Env_PickingObject"))))
		return E_FAIL;

	/* For. BrokenObject */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/BrokenObject.dat"), TEXT("Effect_Env_BrokenObject"))))
		return E_FAIL;

	/* For. WaterObject */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/WaterObject.dat"), TEXT("Effect_Env_WaterObject"))))
		return E_FAIL;

	/* For. HealpackObject */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/HealpackObject.dat"), TEXT("Effect_Env_HealpackObject"))))
		return E_FAIL;

	/* For. VentSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/VentSmoke.dat"), TEXT("Effect_Env_VentSmoke"))))
		return E_FAIL;

	/* For. Boss_Particle */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/Boss_Particle.dat"), TEXT("Effect_Env_Boss_Particle"))))
		return E_FAIL;

	/* 반복 렉트 */
	/* For. GrapRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/GrapRect.dat"), TEXT("Effect_Repeat_GrapRect"))))
		return E_FAIL;

	/* For. ConnectRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/ConnectRect.dat"), TEXT("Effect_Repeat_ConnectRect"))))
		return E_FAIL;

	/* For. FireImpactRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/FireImpactRect.dat"), TEXT("Effect_Repeat_FireImpactRect"))))
		return E_FAIL;

	/* For. PaperRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/PaperRect.dat"), TEXT("Effect_Repeat_PaperRect"))))
		return E_FAIL;

	/* For. SkateEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/SkateEffect.dat"), TEXT("Effect_Repeat_SkateEffect"))))
		return E_FAIL;

	/* For. VentPowerSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/VentPowerSmoke.dat"), TEXT("Effect_Repeat_VentPowerSmoke"))))
		return E_FAIL;

	/* For. VentPowerSmokeZ */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/VentPowerSmokeZ.dat"), TEXT("Effect_Repeat_VentPowerSmokeZ"))))
		return E_FAIL;
	
	/* For. WalkSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/WalkSmoke.dat"), TEXT("Effect_Repeat_WalkSmoke"))))
		return E_FAIL;

	/* For. HippoReadySmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/HippoReadySmoke.dat"), TEXT("Effect_Repeat_HippoReadySmoke"))))
		return E_FAIL;

	/* For. HippoSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/HippoSmoke.dat"), TEXT("Effect_Repeat_HippoSmoke"))))
		return E_FAIL;

	/* For. HippoSmokeEnd */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/HippoSmokeEnd.dat"), TEXT("Effect_Repeat_HippoSmokeEnd"))))
		return E_FAIL;

	/* For. FieldSmoke */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/FieldSmoke.dat"), TEXT("Effect_Repeat_FieldSmoke"))))
		return E_FAIL;

	/* For. BalloonEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/BalloonEffect.dat"), TEXT("Effect_Repeat_BalloonEffect"))))
		return E_FAIL;

	/* For. FruitEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/FruitEffect.dat"), TEXT("Effect_Repeat_FruitEffect"))))
		return E_FAIL;

	/* For. WaterVaporRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/WaterVaporRect.dat"), TEXT("Effect_Repeat_WaterVaporRect"))))
		return E_FAIL;

	/* For. SlidingEffect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/SlidingEffect.dat"), TEXT("Effect_Repeat_SlidingEffect"))))
		return E_FAIL;

	/* For. HeatingRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/HeatingRect.dat"), TEXT("Effect_Repeat_HeatingRect"))))
		return E_FAIL;

	/* For. CookingCompleteStarRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/CookingCompleteStarRect.dat"), TEXT("Effect_Repeat_CookingCompleteStarRect"))))
		return E_FAIL;

	/* For. BoiledRect */
	if (FAILED(pGameInstance->Create_Prototype_Effect_Particle_Data(m_pDevice, m_pDeviceContext, SCENE_STATIC,
		TEXT("../../Data/Effect/Environment/BoiledRect.dat"), TEXT("Effect_Repeat_BoiledRect"))))
		return E_FAIL;
#pragma endregion

#pragma region Player_HPBar
	/* for.Prototype_Component_Texture_HPBar */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_HPBar"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/HPBar/Health_Brains%d.dds"), 6))))
		return E_FAIL;

	/* For.Prototype_GameObject_PlayerHPBar */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_PlayerHPBar"), CPlayerHPBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skill_UI */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_UI"), CSkill_UI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Skill */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Skill"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Skill/Skill%d.dds"), 8))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Skill_BG */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Skill_BG"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Skill/Skill_BG%d.dds"), 4))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_GameObject_Background */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_LOGO, TEXT("Prototype_GameObject_Background"), CBackground::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Cloud"), CCloud::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

#pragma region 이펙트

	/* 이펙트 프로토 타입 */
	_matrix 	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	/* For.PulseRing */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_PulseRing"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/PulseRing/PulseRing.dat"), ("../bin/Resources/Effect/Mesh/PulseRing/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(30.f)) *  XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(255.f));
	/* For.ChopA */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_ChopA"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Melee/ChopA.dat"), ("../bin/Resources/Effect/Mesh/Melee/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));/* * XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians*/;
	/* For.ChopB */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_ChopB"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Melee/ChopB.dat"), ("../bin/Resources/Effect/Mesh/Melee/"), PivotMatrix))))
		return E_FAIL;

	/* For.ChopC */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_ChopC"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Melee/ChopC.dat"), ("../bin/Resources/Effect/Mesh/Melee/"), PivotMatrix))))
		return E_FAIL;

	/* For.TumbleAttack */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_TumbleAttack"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Melee/TumbleAttack.dat"), ("../bin/Resources/Effect/Mesh/Melee/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	/* For.DountRing */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_DonutRing"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/DonutRing/DonutRing.dat"), ("../bin/Resources/Effect/Mesh/DonutRing/"), PivotMatrix))))
		return E_FAIL;

	/* For.SlidingMesh */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_SlidingMesh"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Skill/SlidingMesh.dat"), ("../bin/Resources/Effect/Mesh/Skill/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixTranslation(-0.0004f, 0.0f, 0.f);
	/* For.PSIBlast */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_PSIBlast"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Skill/BrainStorm.dat"), ("../bin/Resources/Effect/Mesh/Skill/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	/* For.BrainStorm */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_BrainStorm"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Skill/BrainStorm.dat"), ("../bin/Resources/Effect/Mesh/Skill/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	/* For.Balloon */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Balloon"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Skill/BrainStorm.dat"), ("../bin/Resources/Effect/Mesh/Skill/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	/* For.DizziStar */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_DizziStar"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Stun/DizzyStar.dat"), ("../bin/Resources/Effect/Mesh/Stun/"), PivotMatrix))))
		return E_FAIL;

	/* For.ChargeSingleWave */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_ChargeSingleWave"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Wave/ChargeSingleWave.dat"), ("../bin/Resources/Effect/Mesh/Wave/"), PivotMatrix))))
		return E_FAIL;

	/* For.ChargeWave */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_ChargeWave"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Wave/ChargeWave.dat"), ("../bin/Resources/Effect/Mesh/Wave/"), PivotMatrix))))
		return E_FAIL;

	/* For.ImpactWave */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_ImpactWave"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Wave/ImpactWave.dat"), ("../bin/Resources/Effect/Mesh/Wave/"), PivotMatrix))))
		return E_FAIL;

	PivotMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	/* For.WaterWaveModel */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_WaterWave"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/WaterWave/WaterWaveModel.dat"), ("../bin/Resources/Effect/Mesh/WaterWave/"), PivotMatrix))))
		return E_FAIL;

	/* For.WaterModel */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Water"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/Water/WaterModel.dat"), ("../bin/Resources/Effect/Mesh/Water/"), PivotMatrix))))
		return E_FAIL;

	/* For.BlendWave */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_BlendWave"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/BlendWave/BlendWave.dat"), ("../bin/Resources/Effect/Mesh/BlendWave/"), PivotMatrix))))
		return E_FAIL;

	/* For.BlendHead */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_BlendHead"),
		CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Effect/Mesh/BlendHead/BlendHead.dat"), ("../bin/Resources/Effect/Mesh/BlendHead/"), PivotMatrix))))
		return E_FAIL;
#pragma endregion

	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForDemoScene()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	Loading_NaviData(TEXT("../../Data/NaviData.dat"));
	//m_pNavigation->Clear_Cells();
	//m_pNavigation->Create_Cells(TEXT("../../Data/NaviDataTest.dat"));
	Loading_TutorialScripts();

	//Loading_ForStage03();

	/* For.Prototype_GameObject_Camera */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_Camera"), CCamera_Fly::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Plane*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_Plane"), CPlane::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_NpcDoor0"), CNpcDoor::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_DoubleDoorBlocker0"), CDoubleDoorBlocker::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Npc"), CNpc::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Loboto"), CLoboto::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Player_CutScene"), CPlayer_CutScene::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_Wall"), CWallCollision::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_MoveCabinet"), CMoveCabinet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_AnimatedShelv"), CAnimatedShelv::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_FallObject"), CFallObject::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_DemoRect"), CDemoRect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_OfficeWalls"), COfficeWalls::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_ZiantLoboto0"), CZiantLoboto::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Sky*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Test/SkyBox%d.dds"), 1))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_DemoRect */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_Component_Texture_DemoRect"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/DemoRect/DemoRect%d.dds"), 2))))
		return E_FAIL;

	/*For.Model_OfficeWall*/
	_matrix PivotMatrixs;

	PivotMatrixs = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_Component_Model_OfficeWalls"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/NonAnim/OfficeWall/OfficeWall6.dat", "../Bin/Resources/Meshes/Stage0/NonAnim/OfficeWall/", PivotMatrixs))))
		return E_FAIL;

	PivotMatrixs = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Coach"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/Coach/Coach0.dat", "../Bin/Resources/Meshes/Stage0/Anim/Coach/", PivotMatrixs))))
		return E_FAIL;

	PivotMatrixs = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));	
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Lili"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/Lili/Lili0.dat", "../Bin/Resources/Meshes/Stage0/Anim/Lili/", PivotMatrixs))))
		return E_FAIL;

	PivotMatrixs = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));	
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Sasha"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/Sasha/Sasha0.dat", "../Bin/Resources/Meshes/Stage0/Anim/Sasha/", PivotMatrixs))))
		return E_FAIL;

	PivotMatrixs = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Milla"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage0/Anim/Milla/Milla0.dat", "../Bin/Resources/Meshes/Stage0/Anim/Milla/", PivotMatrixs))))
		return E_FAIL;

	/* For. PurseRing */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_MoveNpc"), CMoveNpc::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

#pragma region Tuto_MessageUITest
	/* for.Prototype_Component_Texture_Message */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_Component_Texture_Message"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Tutorial/Message/Message%d.dds"), 8))))
		return E_FAIL;

	/* For.Prototype_GameObject_TutoMessageUI */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_TutoMessageUI"), CTuto_MessageUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion


	m_isFinished = true;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForStage01()
{
	Loading_NaviData(TEXT("../../Data/NaviData1.dat"));
	Loading_Stage01_Npc();

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Plane*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_Plane"), CPlane::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DoorZipper0*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_DoorZipper0"), CDoorZipper::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Not_AFFECTED_Render*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_Not_AFFECTED_Render"), CNot_Affected_Render::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_PsyBLExit*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_PsyBLExit"), CPsyBLExit::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_AWaterCooler*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_AWaterCooler"), CAWaterCooler::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Stage1File_Cabinet*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_Stage1File_Cabinet"), CStage1File_Cabinet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Stage1File_Zip*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_Stage1File_Zip"), CZip::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Skill_Info */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Skill_Info"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Skill_Info/Skill_Info%d.dds"), 7))))
		return E_FAIL;

	/* for.Prototype_GameObject_Skill_Info */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Skill_Info"), CSkill_Info::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	m_isFinished = true;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Stage01_Npc()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* for.Prototype_Component_Texture_Quest_Script */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Quest_Script"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Quest/Quest_Script/Quest_Scripts%d.dds"), 12))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* for.Prototype_Component_Texture_Quest_Desc */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Quest_Desc"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Quest/Quest_Desc/Quest_Desc%d.dds"), 3))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Prototype_GameObject_QuestScriptUI */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_QuestScriptUI"), CQuestScriptUI::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Prototype_GameObject_QuestUI */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_QuestUI"), CQuestUI::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Prototype_GameObject_NpcCoach */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_NpcCoach"), CNpcCoach::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Prototype_GameObject_NpcSasha */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_NpcSasha"), CNpcSasha::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Prototype_GameObject_NpcLoboto */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE01, TEXT("Prototype_GameObject_NpcLoboto"), CNpcLoboto::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForStage02()
{

	Loading_NaviData(TEXT("../../Data/NaviData2.dat"));
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Plane*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_Plane"), CPlane::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_PyroPainting*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_PyroPainting"), CPyroPainting::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	
#pragma region Stage02ButtonTest

	/* for.Prototype_Component_Texture_Stage02Button */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Stage02Button"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage02/DoorButtons/btn%d.dds"), 12))))
		return E_FAIL;
	/* for.Prototype_Component_Texture_Stage02ButtonShow */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Stage02ButtonShow"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage02/DoorButtons/btn_show%d.dds"), 11))))
		return E_FAIL;

	/* For.Prototype_GameObject_Stage02DoorButtons */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Stage02DoorButtons"), CStage02Button::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Stage02DoorButtons */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_Door2"), CVentDoor::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lock_Object */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_Lock_Object"), CLock_Object::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Player_2D */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_Component_Texture_Player_2D"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage02/2D/2D_Player.dds"), 1))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Monster_2D */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_Component_Texture_Monster_2D"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage02/2D/2D_Monster%d.dds"), 2))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Key_2D */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_Component_Texture_Key_2D"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage02/2D/2D_Key.dds"), 1))))
		return E_FAIL;

	/* for.Prototype_Component_Texture_Lock_2D */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_Component_Texture_Lock_2D"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage02/2D/2D_Lock%d.dds"), 2))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_2D */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_Player_2D"), CPlayer_2D::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster_2D */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_Monster_2D"), CMonster_2D::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Plane_2D"), CPlane_2D::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_Key_2D"), CKey_2D::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE02, TEXT("Prototype_GameObject_Lock_2D"), CLock_2D::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion

	m_isFinished = true;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_ForStage03()
{
	Loading_NaviData(TEXT("../../Data/NaviData3.dat"));

	/* For.ChopC */
	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Compt_Trapoline"),
	//	CModel::Create(m_pDevice, m_pDeviceContext, ("../bin/Resources/Meshes/Stage3/Anim/Compt_Trapoline/Compt_Trapoline0.dat"), ("../bin/Resources/Meshes/Stage3/Anim/Compt_Trapoline/"), PivotMatrix))))
	//	return E_FAIL;

	///* For. ChargeSingleWave */
	//if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Trampoline"), CTrampoline::Create(m_pDevice, m_pDeviceContext))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Plane*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_Plane"), CPlane::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FruitSpawner */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_FruitSpawner"), CFruitSpawner::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FruitTrigger */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_FruitTrigger"), CFruitCheckTrigger::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Plating_Plate0 */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate0"), CPlatingPlate_Phase0::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Plating_Plate0 */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate1"), CPlatingPlate_Phase1::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Plating_Plate0 */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_Plating_Plate2"), CPlatingPlate_Phase2::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RailSlide */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_RailSlide"), CRailSlide::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_GlantCloche */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_GiantCloche"), CGiantCloche::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Stage3BossDish */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_Stage3BossDish"), CStage3BossDish::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CookingEnd */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingEnd"), CCookingEnd::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Loading_Stage03UI();

	RELEASE_INSTANCE(CGameInstance);
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_Stage03_StaticObjects()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* For. Prototpye_GameObject_EGG */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototpye_GameObject_EGG"), CEgg::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Prototpye_GameObject_AWATERMELON */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototpye_GameObject_AWATERMELON"), CAWaterMelon::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;

	}

	/* For. Prototpye_GameObject_BEEHIVE */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototpye_GameObject_BEEHIVE"), CBeeHive::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Prototpye_GameObject_BREAD */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototpye_GameObject_BREAD"), CBread::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Prototpye_GameObject_ONION */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototpye_GameObject_ONION"), COnion::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototpye_GameObject_PORK */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototpye_GameObject_PORK"), CPork::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototpye_GameObject_STRAWBERRY */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototpye_GameObject_STRAWBERRY"), CStrawberry::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_BLADE */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BLADE"), CBlade::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_HippoLegs*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_HippoLegs"), CHippoLegs::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_BlenderPath*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BlenderPath"), CBlenderPath::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_BlenderPath*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Blender2"), CBlender2::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_CBlender */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Blender"), CBlender::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
	
		return E_FAIL;
	}

	/* For. Prototype_GameObject_Griddle */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Griddle"), CGriddle::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
	
		return E_FAIL;
	}


	/* For. Prototype_GameObject_StudioCam */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_StudioCam"), CStudioCam::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Treasure */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Treasure"), CTreasure::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Compt_Vent*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_Vent"), CComput_Vent::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Compt_LightBar*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_LightBar"), CLightBar::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Compt_SS_Cuff*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_SS_Cuff"), CSS_Cuff::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Compt_HostZone*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_HostZone"), CHostZone::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Compt_StationRamp0*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_StationRamp0"), CStationRamp0::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Compt_StationRamp1*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_StationRamp1"), CStationRamp1::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* 애니 메쉬 원본 겍체 Prototype_GameObject_PPlate0 */

	/* For. Prototype_GameObject_PPlate0 */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_PPlate0"), CPPlate::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_HandKnife0 */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_HandKnife0"), CChoppig_HandKnife::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Bell */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_Bell0"), CBell::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Bell */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_PigChopping"), CChoppig_PullHand::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Bouncer*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_Bouncer0"), CBouncer::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_Trampoline*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Compt_Trapoline0"), CTrampoline::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);

		return E_FAIL;
	}

	/* For. Prototype_GameObject_KitchenPan*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_KitchenPan"), CKitchenPan::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Prototype_GameObject_KitchenPanButton*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_KitchenPanButton"), CKitchenPanButton::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Prototype_GameObject_GasBurner */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_GasBurner"), CGasBurner::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Prototype_GameObject_Static_Bell */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Static_Bell"), CStatic_Bell::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For. Prototype_GameObject_BossWall */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BossWall"), CBossWall::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Stage03UI()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);


	/* for.Prototype_Component_Texture_CookingUI_Background */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_Component_Texture_CookingUI_Background"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage03/CookingUI/Phase_Background/Phase_Background%d.dds"), 3))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	/* for.Prototype_Component_Texture_CookingUI_Fruit */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_Component_Texture_CookingUI_Fruit"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage03/CookingUI/Phase_Fruit/Phase_Fruit%d.dds"), 7))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	/* for.Prototype_Component_Texture_CookingUI_OnPlate */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_Component_Texture_CookingUI_OnPlate"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage03/CookingUI/Phase_OnPlate/Phase_OnPlate%d.dds"), 3))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* for.Prototype_Component_Texture_Stage03Timer */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_Component_Texture_Stage03Timer"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage03/TimerUI/TimerNumber%d.dds"), 11))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* for.Prototype_Component_Texture_Cooking_ProgressBar */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_Component_Texture_Cooking_ProgressBar"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Stage03/CookingUI/Phase_ProgressBar/Phase_ProgressBar%d.dds"), 4))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* for.Prototype_GameObject_CookingUI */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingUI"), CCookingUI::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* for.Prototype_GameObject_TimerUI */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_TimerUI"), CTimerUI::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* for.Prototype_GameObject_CookingProgressBar */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE03, TEXT("Prototype_GameObject_CookingProgressBar"), CCookingProgressBarUI::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForStage04()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	Loading_NaviData(TEXT("../../Data/NaviData4.dat"));

	_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	/* For.Prototype_GameObject_Plane*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE04, TEXT("Prototype_GameObject_Plane"), CPlane::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Stage4Mercial */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE04, TEXT("Prototype_GameObject_Stage4Mercial"), CStage4Mercial::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage05()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	Loading_NaviData(TEXT("../../Data/NaviData5.dat"));

	/* For. Prototype_GameObject_INGHole */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_INGHole"), CINGHole::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_matrix PivotMatrix = XMMatrixScaling(1.5f, 1.5f, 1.5f);

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	/* For.Prototype_GameObject_Plane*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STAGE05, TEXT("Prototype_GameObject_Plane"), CPlane::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Boss_Dish*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Dish"), CBoss_Dish::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Boss_Dish*/
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Boss_Projectile"), CBoss_Food_Projectile::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_matrix PivotMatrixs = XMMatrixScaling(1.3f, 1.3f, 1.3f) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_MainDish"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/Boss_Dish0.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Dish_BroCcoli"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/Boss_Dish1.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Dish_Potato"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/Boss_Dish3.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Dish_Hash"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/Boss_Dish2.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Dish/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Main_Soup"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Soup/Boss_Soup0.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Soup/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Soup_Blob"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Soup/Boss_Soup1.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Soup/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Main_Cookie"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/Boss_Cookie0.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_One"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/Boss_Cookie1.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_Two"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/Boss_Cookie2.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_Three"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/Boss_Cookie3.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Boss_Cookie_For"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/Boss_Cookie4.dat", "../Bin/Resources/Meshes/Stage5/NonAnim/Boss_Cookie/", PivotMatrixs))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}


	RELEASE_INSTANCE(CGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_DuringPlaying()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	//Ready_Model_Data(TEXT("../../Data/Stage1PathFind.dat"));

	if (m_eThreadingNextID == SCENE_DEMO)
	{
		Ready_Model_Data(TEXT("../../Data/Stage1PathFind.dat"));
		Loading_Monster();
		m_isFinished = true;
	}
	else if (m_eThreadingNextID == SCENE_STAGE01)
	{
		Ready_Model_Data(TEXT("../../Data/Stage2PathFind.dat"));
		Ready_Model_Data(TEXT("../../Data/Stage3FruitPathFind.dat"), _float3(0.0025f, 0.0025f, 0.0025f));
		m_isFinished = true;
	}
	else if (m_eThreadingNextID == SCENE_STAGE02)
	{
		Ready_Model_Data(TEXT("../../Data/Stage3PathFind.dat"));
		//Ready_Model_Data(TEXT("../../Data/Stage3FruitPathFind.dat"), _float3(0.0025f, 0.0025f, 0.0025f));
		m_isFinished = true;	
	}
	else if (m_eThreadingNextID == SCENE_STAGE03)
	{
		//Loading_Monster();
		Ready_Model_Data(TEXT("../../Data/Stage4PathFind.dat"));
		//Ready_Model_Data(TEXT("../../Data/Stage3FruitPathFind.dat"), _float3(0.0025f, 0.0025f, 0.0025f));
		m_isFinished = true;
	}
	else if (m_eThreadingNextID == SCENE_STAGE04)
	{
		//Loading_Monster();
		//Ready_Model_Data(TEXT("../../Data/Stage3FruitPathFind.dat"), _float3(0.0025f, 0.0025f, 0.0025f));
		m_isFinished = true;
	}
	else if (m_eThreadingNextID == SCENE_STAGE05)
	{
		//Loading_Monster();
		m_isFinished = true;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoading);

	return S_OK;
}

HRESULT CLoader::Loading_TutorialScripts()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* for.Prototype_Component_Texture_TutoScript */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_Component_Texture_TutoScript"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Tutorial/Script/Script/TextUI%d.dds"), 31))))
		return E_FAIL;

	/* For.Prototype_GameObject_ScriptUI */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_DEMO, TEXT("Prototype_GameObject_ScriptUI"), CTuto_ScriptUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Ending()
{
	
	_matrix PivotMatrix = XMMatrixIdentity();
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	/* for.Prototype_Component_Texture_Skill */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_ENDING, TEXT("Prototype_Component_Texture_EndingUI"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Ending/EndingUI%d.dds"), 7))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	//if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Coach"), CModel::Create(m_pDevice, m_pDeviceContext
	//	, "../Bin/Resources/Meshes/Stage0/Anim/Coach/Coach0.dat", "../Bin/Resources/Meshes/Stage0/Anim/Coach/", PivotMatrix))))
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//
	//	return E_FAIL;
	//}
	//
	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	//if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_CensorBurly0"), CModel::Create(m_pDevice, m_pDeviceContext
	//	, "../Bin/Resources/Meshes/Monster/CensorBurly/CensorBurly0.dat", "../Bin/Resources/Meshes/Monster/CensorBurly/", PivotMatrix))))
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//
	//	return E_FAIL;
	//}


	if (FAILED(pGameInstance->Add_Prototype(SCENE_ENDING, TEXT("Prototype_GameObject_EndingObject"), CEndingObject::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_ENDING, TEXT("Prototype_GameObject_EndingUI"), CEndingUI::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::SetUp_Component()
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//
	//m_pNavigation = dynamic_cast<CNavigation*>(pGameInstance->Clone_Component(SCENE_STATIC, TEXT("Prototype_Component_Navigation")));
	//if (nullptr == m_pNavigation)
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return E_FAIL;
	//}
	//RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


HRESULT CLoader::Ready_Model_Data(const _tchar* FinderFilePath, _float3 pivotScale)
{
	HANDLE		hFile = 0;

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	// TEXT("../../Data/Stage0PathFind.dat")
	hFile = CreateFile(FinderFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	while (true)
	{
		MESHPATH* meshpath = new MESHPATH;

		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR MeshName[MAX_PATH] = L"";
		ReadFile(hFile, MeshName, dwStrByte, &dwByte, nullptr);
		meshpath->wstrMesh = MeshName;

		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		TCHAR MeshFilePath[MAX_PATH] = L"";
		ReadFile(hFile, MeshFilePath, dwStrByte, &dwByte, nullptr);
		meshpath->wstrMeshStatekey = MeshFilePath;

		ReadFile(hFile, &meshpath->iMeshCount, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &meshpath->bModleAnim, sizeof(_bool), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(meshpath);
			break;
		}
		wstring MeshPrototypeName = L"Prototype_Component_Model_";
		for (_uint i = 0; i < meshpath->iMeshCount; ++i)
		{
			wstring  FullModel;
			if (meshpath->bModleAnim)
				FullModel = MeshPrototypeName + meshpath->wstrMesh;
			else
				FullModel = MeshPrototypeName + meshpath->wstrMesh;
			const _tchar* pPrototypeTag = nullptr;
			TCHAR  MeshFullName[MAX_PATH] = L"";
			swprintf_s(MeshFullName, FullModel.c_str(), i);
			TCHAR* pPrototypeName = new TCHAR[MAX_PATH];
			lstrcpy(pPrototypeName, MeshFullName);

			pPrototypeTag = pGameInstance->Add_PrototypeDataToList(pPrototypeName);
			if (nullptr != pPrototypeTag)
			{
				Safe_Delete(pPrototypeName);
			}
			 
			//m_MeshFullName.emplace_back(pPrototypeName);

			wstring  Filepath = meshpath->wstrMeshStatekey;
			char*    FilepathName = new char[MAX_PATH];

			//WideCharToMultiByte(CP_ACP, 0, Filepath.c_str(), MAX_PATH, FilepathName, MAX_PATH - 1, nullptr, nullptr);
			
			//wchar_t strUnicode[256] = { 0, };
			//char	strMultibyte[256] = { 0, };
			//wcscpy_s(strUnicode, 256, L"유니코드");
			//int len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
			//WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, strMultibyte, len, NULL, NULL);

			wchar_t strUnicode[MAX_PATH] = { 0, };
			wcscpy_s(strUnicode, MAX_PATH, Filepath.c_str());
			int len = WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, strUnicode, -1, FilepathName, len, NULL, NULL);

			_matrix PivotMatrix;
			char*    cMeshFullNamePath = new char[MAX_PATH];
			if (false == meshpath->bModleAnim)
			{
				wstring MeshPath = Filepath + meshpath->wstrMesh + L".dat";
				TCHAR  MeshFullPath[MAX_PATH] = L"";
				swprintf_s(MeshFullPath, MeshPath.c_str(), i);
				WideCharToMultiByte(CP_ACP, 0, MeshFullPath, MAX_PATH, cMeshFullNamePath, MAX_PATH - 1, nullptr, nullptr);
				PivotMatrix = XMMatrixIdentity();
				//m_MeshFullNamePath.emplace_back(cMeshFullNamePath);
				//pGameInstance->Add_PathDataToList(cMeshFullNamePath);

			}
			else
			{
				wstring MeshPath = Filepath + meshpath->wstrMesh + L".dat";
				TCHAR  MeshFullPath[MAX_PATH] = L"";
				swprintf_s(MeshFullPath, MeshPath.c_str(), i);
				WideCharToMultiByte(CP_ACP, 0, MeshFullPath, MAX_PATH, cMeshFullNamePath, MAX_PATH - 1, nullptr, nullptr);
				PivotMatrix = XMMatrixScaling(pivotScale.x, pivotScale.y, pivotScale.z)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
			}
			if (nullptr == pPrototypeTag)
			{
				if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, pPrototypeName, CModel::Create(m_pDevice, m_pDeviceContext, cMeshFullNamePath, FilepathName, PivotMatrix))))
				{
					Safe_Delete(FilepathName);
					Safe_Delete(cMeshFullNamePath);
					Safe_Delete(meshpath);
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}
			}
			Safe_Delete(FilepathName);
			Safe_Delete(cMeshFullNamePath);

		}

		Safe_Delete(meshpath);
		//m_PathFind.push_back(meshpath);

	}
	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_NaviData(const _tchar * pNaviDataFilePath)
{

	HANDLE			hFile = 0;
	_uint			iCntPoint = 3;
	_ulong	dwByte = 0;
	hFile = CreateFile(pNaviDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Clear_NaviPoints();
	NAVIMESHDESC NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

	_float3			vPoints[3];
	_uint			iOption = 0;
	while (true)
	{
		ZeroMemory(&NaviDesc, sizeof(NAVIMESHDESC));

		ReadFile(hFile, vPoints, sizeof(_float3) * iCntPoint, &dwByte, nullptr);
		ReadFile(hFile, &iOption, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		memcpy(NaviDesc.vPoint, vPoints, sizeof(_float3) *iCntPoint);
		NaviDesc.iByOption = iOption;
		pGameInstance->Add_PointsToVector(vPoints);
		//m_vecPoints.emplace_back(vPoints);
	}


	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;

}

HRESULT CLoader::Loading_Monster()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Censor0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Censor/Censor0.dat", "../Bin/Resources/Meshes/Monster/Censor/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_CensorBurly0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/CensorBurly/CensorBurly0.dat", "../Bin/Resources/Meshes/Monster/CensorBurly/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Censor1"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Censor/Censor1.dat", "../Bin/Resources/Meshes/Monster/Censor/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_BadIdea0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/BadIdea/BadIdea0.dat", "../Bin/Resources/Meshes/Monster/BadIdea/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f))*XMMatrixTranslation(0.f, -1.f, 0.f);
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_BadIdeaSpawner"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/BadIdea/BadIdeaSpawner.dat", "../Bin/Resources/Meshes/Monster/BadIdea/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_CensorSpawner"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Censor/CensorSpawner.dat", "../Bin/Resources/Meshes/Monster/Censor/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Judge0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Judge/Judge0.dat", "../Bin/Resources/Meshes/Monster/Judge/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Judge1"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Judge/Judge1.dat", "../Bin/Resources/Meshes/Monster/Judge/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Judge2"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Judge/Judge2.dat", "../Bin/Resources/Meshes/Monster/Judge/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}


	PivotMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_GoatFord0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Goat_Ford/Goat_Ford0.dat", "../Bin/Resources/Meshes/Monster/Goat_Ford/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_GoatOtto0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Goat_Otto/Goat_Otto0.dat", "../Bin/Resources/Meshes/Monster/Goat_Otto/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_GoatHollis0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Goat_Hollis/Goat_Hollis0.dat", "../Bin/Resources/Meshes/Monster/Goat_Hollis/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Spatula"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Spatula/Spatula0.dat", "../Bin/Resources/Meshes/Monster/Spatula/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Beater"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Beater/Beater0.dat", "../Bin/Resources/Meshes/Monster/Beater/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(3.f, 5.f, 3.f)*XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f));
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_PukeLaser"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Puke/Laser.dat", "../Bin/Resources/Meshes/Monster/Puke/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixIdentity();


	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeWeapon0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Gravel/Gravel0.dat", "../Bin/Resources/Meshes/Monster/Gravel/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeWeapon1"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Gravel/Gravel1.dat", "../Bin/Resources/Meshes/Monster/Gravel/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeWeapon2"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Gravel/Gravel2.dat", "../Bin/Resources/Meshes/Monster/Gravel/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Anvil0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Anvil/Anvil0.dat", "../Bin/Resources/Meshes/Monster/Anvil/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Tooth"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Tooth/Tooth0.dat", "../Bin/Resources/Meshes/Monster/Tooth/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Bulb0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Bulb/Bulb0.dat", "../Bin/Resources/Meshes/Monster/Bulb/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeBook0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Book/Book0.dat", "../Bin/Resources/Meshes/Monster/Book/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeBook1"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Book/Book1.dat", "../Bin/Resources/Meshes/Monster/Book/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeBook2"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Book/Book2.dat", "../Bin/Resources/Meshes/Monster/Book/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeWeaponhead0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/JudgeWeaponhead/JudgeWeaponhead0.dat", "../Bin/Resources/Meshes/Monster/JudgeWeaponhead/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeWeaponhead1"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/JudgeWeaponhead/JudgeWeaponhead1.dat", "../Bin/Resources/Meshes/Monster/JudgeWeaponhead/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_JudgeWeaponhead2"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/JudgeWeaponhead/JudgeWeaponhead2.dat", "../Bin/Resources/Meshes/Monster/JudgeWeaponhead/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}


	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_DentureCracked0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/DentureCracked/DentureCracked0.dat", "../Bin/Resources/Meshes/Monster/DentureCracked/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_DentureCracked1"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/DentureCracked/DentureCracked1.dat", "../Bin/Resources/Meshes/Monster/DentureCracked/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_DentureCracked2"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/DentureCracked/DentureCracked2.dat", "../Bin/Resources/Meshes/Monster/DentureCracked/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(1.5f, 1.5f, 1.5f);

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_FruitsBlob"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/Blob/Blob.dat", "../Bin/Resources/Meshes/Monster/Blob/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(0.0025f, 0.0025f, 0.0025f)* XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));


	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Fruits0"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Fruits/Fruits0.dat", "../Bin/Resources/Meshes/Fruits/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Fruits1"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Fruits/Fruits1.dat", "../Bin/Resources/Meshes/Fruits/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Fruits2"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Fruits/Fruits2.dat", "../Bin/Resources/Meshes/Fruits/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Fruits3"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Fruits/Fruits3.dat", "../Bin/Resources/Meshes/Fruits/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Fruits4"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Fruits/Fruits4.dat", "../Bin/Resources/Meshes/Fruits/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Fruits5"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Fruits/Fruits5.dat", "../Bin/Resources/Meshes/Fruits/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_Fruits6"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Fruits/Fruits6.dat", "../Bin/Resources/Meshes/Fruits/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	PivotMatrix = XMMatrixScaling(2.f, 2.f, 2.f);

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Model_PukeBall"), CModel::Create(m_pDevice, m_pDeviceContext
		, "../Bin/Resources/Meshes/Monster/PukeBall/PukeBall.dat", "../Bin/Resources/Meshes/Monster/PukeBall/", PivotMatrix))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BossGoat"), CBoss_Goat::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Fruits"), CFruits::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Blob"), CBlob::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_PukeBall"), CPuke_Ball::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BadIdea"), CBadIdea::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Denture0"), CDenture::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_ToothFairy0"), CToothFairy::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Censor"), CCensor::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CensorBurly"), CCensor_Burly::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Doubt0"), CDoubt::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Judge"), CJudge::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Tooth"), CTooth::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_Bulb"), CBulb::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeWeapon"), CJudge_Weapon::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeBook"), CJudge_Book::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CrackedDenture"), CCracked_Denture::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}	

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CensorSpawner"), CCensor_Spawner::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BadIdeaSpawner"), CBadIdea_Spawner::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeWeaponhead"), CJudge_Weaponhead::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BadIdeaAttTrigger"), CBadIdea_AttTrigger::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CensorBAttTriggerL"), CCensorB_AttTriggerL::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_CensorBAttTriggerR"), CCensorB_AttTriggerR::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeAttTrigger"), CJudge_AttTrigger::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_JudgeWeaponAttTrigger"), CJudgeWeapon_AttTrigger::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterHpBar"), CMonster_HpBar::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_PukeLaser"), CPuke_Laser::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_NonBTOtto"), CNonBT_Otto::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_NonBTFord"), CNonBT_Ford::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_NonBTHollis"), CNonBT_Hollis::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_MonsterInfo"), CMonsterInfo::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BoardR"), CBoard_R::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BoardL"), CBoard_L::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_GameObject_BossHpBar"), CBossHpBar::Create(m_pDevice, m_pDeviceContext))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	/* for.Prototype_Component_Texture_Skill */
	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_MonsterHpBar"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/MonsterHPBar/HpBar%d.dds"), 3))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_MonsterInfo"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Note/MonsterUI%d.dds"), 8))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Prototype(SCENE_STATIC, TEXT("Prototype_Component_Texture_Fruits"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Fruit/Fruit%d.dds"), 7))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

 	return S_OK;
}

HRESULT CLoader::Loading_PlayerTrigger()
{
	return S_OK;
}


CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SCENE eSceneID)
{
	CLoader*	pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eSceneID)))
	{
		MSGBOX("Failed to Created CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	//Safe_Release(m_pNavigation);

}
