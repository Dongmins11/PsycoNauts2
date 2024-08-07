#pragma once

namespace Engine
{
	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation;
		XMFLOAT3		vPosition;
		double			Time;
	}KEYFRAME;

	typedef struct tagMaterial
	{
		class CTexture*		pTexture[TEXTURETYPE_END];
	}MESHMATERIAL;


	typedef struct PassDesc
	{
		ID3D11InputLayout*	pInputLayout = nullptr;
		ID3DX11EffectPass*	pPass = nullptr;
	}PASSDESC;

	typedef struct tagMtrlDesc
	{
		_float4		vDiffuse;
		_float4		vAmbient;
		_float4		vSpecular;
		_float		fPower;
	}MTRLDESC;


	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_POINT, TYPE_DIRECTIONAL, TYPE_SPOT, TYPE_END };

		_uint		iType = TYPE_END;

		_float4		vPosition;
		_float		fRange = 0.f;
		_float		fOuterConeAngle = 0.f;
		_float		fInnerConeAngle = 0.f;
		_float		fPower = 5.f;
		_float4		vDirection;

		_float4		vDiffuse;
		_float4		vAmbient;
		_float4		vSpecular;
	}LIGHTDESC;

	typedef struct tagVertexCubeTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vTexUV;
	}VTXCUBETEX;

	typedef struct tagVertexColor
	{
		XMFLOAT3			vPosition;
		XMFLOAT4			vColor;
	}VTXCOL;

	typedef struct tagVertexTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT2			vTexUV;
	}VTXTEX;

	typedef struct tagVertexPSize
	{
		XMFLOAT3			vPosition;
		float				fPSize;
	}VTXPOINT;

	typedef struct tagVertexModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;
	}VTXMODEL;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
		XMFLOAT3			vTangent;

		XMUINT4				vBlendIndex;
		XMFLOAT4			vBlendWeight;
	}VTXANIMMODEL;

	typedef struct tagVertexNormalTexture
	{
		XMFLOAT3			vPosition;
		XMFLOAT3			vNormal;
		XMFLOAT2			vTexUV;
	}VTXNORTEX;

	typedef struct tagVertexMatrix
	{
		XMFLOAT4			vRight;
		XMFLOAT4			vUp;
		XMFLOAT4			vLook;
		XMFLOAT4			vPosition;
		XMFLOAT2			vCurUV;
		float				fAlpha;
	}VTXMATRIX;

	typedef struct tagNaviMeshDesc
	{
		_float3		vPoint[3];
		_uint		iByOption;
	}NAVIMESHDESC;

	typedef struct tagTexturePath
	{
		wstring		wstrMesh = L"";
		wstring		wstrObjectMeshName= L"";
		wstring		wstrMeshStatekey = L"";
		wstring     wstrMeshPath = L"";
		int			iMeshCount= 0;
		_bool       bModleAnim = 0;
	}MESHPATH;

	typedef struct tagObjectInfo
	{
		//_bool    Anim;
		wstring		StrObjectFullName;
		wstring		StrLayerName;
		wstring		StrModelName;
		_int		iNavigationIndex;
		_int		iCurlingIndex;
		_int        iDeleteCurlingIndex;
		_uint	    iRenderPass;
		_uint       iPX_Index;
		_float      fFrustumRange;
		_float3     fCollider_Center;
		_float3     fCollider_Extent;
		_matrix		WorldMatrix;

	}OBJINFO;


	typedef struct tagFaceIndices16
	{
		unsigned short		_1, _2, _3;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long		_1, _2, _3;
	}FACEINDICES32;

	typedef struct tagLineIndices16
	{
		unsigned short		_1, _2;
	}LINEINDICES16;

	typedef struct tagLineIndices32
	{
		unsigned long		_1, _2;
	}LINEINDICES32;

	typedef struct tagInputDesc
	{
		HINSTANCE			hInst;
		HWND				hWnd;
	} INPUTDESC;
	
	typedef struct tagEffectInfo
	{
		enum EFFECT_OPTION { OPTION_LINEAR, OPTION_EASE_IN_QUAD, OPTION_EASE_OUT_QUAD, OPTION_EASE_IN_OUT, OPTION_END };

		/* 이펙트 파일 이름으로 레이어 이름이 설정되고 */

		/* 디퓨즈 텍스쳐 활성화*/
		_bool  isDiffuse = false;
		_uint	iDiffuseNum = 0;

		/* 디퓨즈 텍스쳐 활성화*/
		_bool  isAlpha = false;
		_uint	iAlphaNum = 0;
		/* 상세 데이터값 */
		_float3 vPos;
		_float3 vScale;
		_float3 vRot;

		/* 첫 상태값 */
		_float4x4	WorldMatrix;

		/* 쉐이더 패스 */
		_uint	iPass = 0;

		/* 색상 값 */
		_float3	C_vColor = _float3(0.f, 0.f, 0.f);
		_float3	C_vEndColor = _float3(0.f, 0.f, 0.f);

		/* 체크 확인 */
		_bool	PP_EffectChackX[OPTION_END] = { false };
		_bool	PP_EffectChackY[OPTION_END] = { false };
		_bool	PP_EffectChackZ[OPTION_END] = { false };

		_bool	PS_EffectChackX[OPTION_END] = { false };
		_bool	PS_EffectChackY[OPTION_END] = { false };
		_bool	PS_EffectChackZ[OPTION_END] = { false };

		_bool	PR_EffectChackX[OPTION_END] = { false };
		_bool	PR_EffectChackY[OPTION_END] = { false };
		_bool	PR_EffectChackZ[OPTION_END] = { false };

		/* 활성화된 패턴 체크 */
		_bool	PP_Chack = false;
		_bool	PS_Chack = false;
		_bool	PR_Chack = false;
		_bool	FI_Chack = false;
		_bool	FO_Chack = false;
		_bool	US_Chack = false;
		_bool	UA_Chack = false;

		/* 빌보드 체크 */
		_bool	I_isBillboardY = false;
		_bool	I_isBillboard = false;

		/* 패턴 */
		_float3	PP_vEnd;
		_float3	PS_vEnd;
		_float3	PR_vEnd;

		/* 시간 */
		_float	CDT_fStartTime = 0.f;
		_float	CDT_fEndTime = 0.f;
		_float	PP_fStartTime = 0.f;
		_float	PP_fEndTime = 0.f;
		_float	PS_fStartTime = 0.f;
		_float	PS_fEndTime = 0.f;
		_float	PR_fStartTime = 0.f;
		_float	PR_fEndTime = 0.f;
		_float	FI_fStartTime = 0.f;
		_float	FI_fEndTime = 0.f;
		_float	FO_fStartTime = 0.f;
		_float	FO_fEndTime = 0.f;
		_float	UA_fStartTime = 0.f;
		_float	UA_fEndTime = 0.f;

		/* 반복 */
		_float	PP_Refeat = 0.f;
		_float	PS_Refeat = 0.f;
		_float	PR_Refeat = 0.f;

		/* 지속 시간 */
		_float	FI_Degree = 0.f;
		_float	FO_Degree = 0.f;

		/* 스프라이트 */
		_uint   US_MaxNumX = 1;
		_uint   US_MaxNumY = 1;
		_uint   US_StarUVX = 0;
		_uint   US_StarUVY = 0;

		/* UV 애니메이션 */
		_float	UA_AniX = 0.f;
		_float	UA_AniY = 0.f;
	}EFFECTRECTINFO;

	typedef struct tagEffectMeshInfo
	{
		/* 매쉬 타이틀 */
		/* 
			CWS : ChargeWaveSingle
			CW : ChargeWave
			IW : ImpactWave
			SS : SpinSwipe
		*/
		enum MESHTYPE { MESH_CWS, MESH_CW, MESH_IW, MESH_SS, MESH_END };
		MESHTYPE eType;

		/* 상세 데이터값 */
		_float3 vPos;
		_float3 vScale;
		_float3 vRot;

		/* 첫 상태값 */
		_matrix	WorldMatrix;

		/* 시간 */
		_float	CDT_fStartTime = 0.f;
		_float	CDT_fEndTime = 0.f;
		_float	CDT_fSpeed = 0.f;

		/* 색상 */
		_float3	C_vColor;

	}EFFECTMESHINFO;

	typedef struct tagEffectParticleInfo
	{
		_bool	isPoint;

		_bool	isDistAlpha;
		_bool	isRandomUV;

		_uint	iPrototype;
		_uint	iOption;
		

		_uint	iPass;

		_uint	iTexNum;

		_uint	iOptionX;
		_uint	iOptionY;
		_uint	iOptionZ;

		_float	fLifeTime;
		_float	fCreateTime;
		
		_float	fUVDelay;
		_float2 vMaxUV;

		_float2 vSize;

		_float3 vColor;
		_float3 vEndColor;

		_float3 vMinDir;
		_float3 vMaxDir;

		_float3 vMaxPos;
	}EFFECTPARTICLEINFO;

	typedef struct tagParticleCreate
	{
		wstring				wstLayerTag;
		EFFECTPARTICLEINFO tParticleInfo;
	}PARTICLECREATE;

	typedef struct tagUISizeDesc
	{
		_float	fX;
		_float	fY;
		_float fSizeX;
		_float fSizeY;
		_uint	iPassIndx;
	}UISIZEDESC;

	typedef struct tagOrthUITextureDesc
	{
		_bool			bRenderInit = true;
		unsigned int	iSceneType;
		_uint		iSortZ = 0;
		int			iImgNum = 0;
		int			iPassNum = 0;
		_float2			iSize;
		_float2			iPos;
		const wchar_t*		szTag;
	}ORTHTEXUTREDESC;
}

