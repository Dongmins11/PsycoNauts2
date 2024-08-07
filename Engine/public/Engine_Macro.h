#pragma once

#define GET_INSTANCE(CLASSNAME)	[](){											\
	CLASSNAME*	pInstance = CLASSNAME::Get_Instance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->AddRef();}														\
	return pInstance;															\
	}();

#define RELEASE_INSTANCE(CLASSNAME)	[](){										\
	CLASSNAME*	pInstance = CLASSNAME::Get_Instance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->Release();}														\
}();


#define MSGBOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("System Message"), MB_OK)

#define BEGIN(NAMESPACE) namespace NAMESPACE {

#define END }

#ifdef ENGINE_EXPORTS /*엔진프로젝트라고 정의되어있다면. */
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


#define DECLARE_SINGLETON(ClassName)							\
public:															\
static ClassName* Get_Instance()								\
{																\
	if (!m_pInstance)											\
		m_pInstance = new ClassName;							\
	return m_pInstance;											\
}																\
static unsigned long Destroy_Instance()							\
{																\
	unsigned long		dwRefCnt = 0;							\
	if (nullptr != m_pInstance)									\
	{															\
		dwRefCnt = m_pInstance->Release();						\
		if(0 == dwRefCnt)										\
			m_pInstance = nullptr;								\
	}															\
	return dwRefCnt;											\
}																\
private:														\
	static ClassName* m_pInstance;								

#define IMPLEMENT_SINGLETON(ClassName)							\
ClassName* ClassName::m_pInstance = nullptr; 

#define COLOR_BULE XMVectorSet(0.f,0.f,1.f,1.f)
#define COLOR_RED XMVectorSet(1.f,0.f,0.f,1.f)
#define COLOR_GREEN XMVectorSet(0.f,1.f,0.f,1.f)
#define COLOR_BLACK XMVectorSet(0.f,0.f,0.f,1.f)
#define COLOR_GRAY XMVectorSet(0.5f,0.5f,0.5f,1.f)
#define COLOR_ORANGE XMVectorSet(1.f,0.5f,0.f,1.f)
#define COLOR_YELLOW XMVectorSet(1.f,1.0f,0.f,1.f)
#define COLOR_PURPLE XMVectorSet(0.5f,0.f,1.f,1.f)
#define COLOR_PINK XMVectorSet(1.0f,0.f,1.f,1.f)
#define COLOR_MINT XMVectorSet(0.12, 0.91, 0.7, 1.f)
#define COLOR_DarkViolet XMVectorSet(0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f)
#define COLOR_DeepPink XMVectorSet(1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f)
#define COLOR_LemonChiffon XMVectorSet(1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f)
#define COLOR_BurlyWood XMVectorSet(0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f)
#define COLOR_White XMVectorSet(1.f, 1.f, 1.f, 1.000000000f)

#define GET_RANDOMFLOAT(LOW, HIGH)[&]()->_float{ \
if(LOW >= HIGH) return LOW; \
_float f = (rand() % 10000) * 0.0001f; \
return (f * (HIGH - LOW)) + LOW; }();

#define LOADFLOAT4(float4Vector)[&]()->_vector{ \
return XMLoadFloat4(&float4Vector); \
}();

#define STOREFLOAT3(Vector4)[&]()->_float3{ \
_float3 fVector =_float3(0.f,0.f,0.f); \
XMStoreFloat3(&fVector, Vector4); \
return fVector; \
}();

#define STOREFLOAT4(Vector4)[&]()->_float4{ \
_float4 fVector =_float4(0.f,0.f,0.f,0.f); \
XMStoreFloat4(&fVector, Vector4); \
return fVector; }();

#define STOREMATRIX(Matrix)[&]()->_float4x4{ \
_float4x4 matFloat4x4; \
XMStoreFloat4x4(&matFloat4x4, Matrix); \
return matFloat4x4; }();

#define GET_VECTORLENGTH(Vector)[&]()->_float{ \
_float fLength = 0.f; \
fLength = XMVectorGetX(XMVector3Length(Vector)); \
return fLength; }();


#define GET_FLOAT3LENGTH(Vector)[&]()->_float{ \
_float fLength = 0.f; \
fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&Vector))); \
return fLength; }();

#define GET_VECTORDOT(vSrc, vDest)[&]()->_float{ \
_float fLength = 0.f; \
fLength = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vSrc), XMVector3Normalize(vDest))); \
return fLength; }();

#define GET_SCALEFROMMAT(Matrix)[&]()->_float3{ \
_float fLengthX,fLengthY,fLengthZ; \
fLengthX = GET_VECTORLENGTH(Matrix.r[0]); \
fLengthY = GET_VECTORLENGTH(Matrix.r[1]); \
fLengthZ = GET_VECTORLENGTH(Matrix.r[2]); \
return _float3(fLengthX, fLengthY,fLengthZ);}();

#define MULTI_FLOAT3(Vector, fScale)[&]()->_float3{	\
_vector vVector = XMLoadFloat3(&Vector);	\
vVector = vVector * fScale;					\
return STOREFLOAT3(vVector); }();

#define TO_STRING(eString) #eString

#define CACULATE_DEGREE(vSrcDir, vDestDir)[&]()->_float{	\
_vector Look = XMVector3Normalize(XMVectorSetY(vSrcDir, 0.f)); \
_vector DestDir = XMVector3Normalize(XMVectorSetY(vDestDir, 0.f)); \
_float radians = acosf(XMVectorGetX(XMVector3Dot(Look, DestDir))); \
_float degree = XMConvertToDegrees(radians); \
_float direction = XMVectorGetY(XMVector3Cross(Look, DestDir)); \
if (direction < 0) {	degree = 360 - degree;} \
return degree;}();
