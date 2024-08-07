#pragma once

namespace Engine
{
	typedef unsigned char				_ubyte;
	typedef signed char					_byte;

	typedef unsigned short				_ushort;
	typedef signed short				_short;

	typedef unsigned int				_uint;
	typedef signed int					_int;

	typedef unsigned long				_ulong;
	typedef signed long					_long;

	typedef float						_float;
	typedef double						_double;

	typedef bool						_bool;
	typedef wchar_t						_tchar;

	typedef XMFLOAT2					_float2;
	typedef XMFLOAT3					_float3;

	typedef XMUINT4						_uint4;

	typedef struct tagfloat4_Derived : public XMFLOAT4
	{
		tagfloat4_Derived() {}
		tagfloat4_Derived(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
		tagfloat4_Derived(XMFLOAT3 vFloat3, float _w) : XMFLOAT4(vFloat3.x, vFloat3.y, vFloat3.z, _w) {}
		explicit tagfloat4_Derived(_In_reads_(4) const float *pArray) : XMFLOAT4(pArray) {}

	}_float4;



	/* SIMD */
	typedef XMVECTOR					_vector;
	typedef FXMVECTOR					_fvector;
	typedef GXMVECTOR					_gvector;
	typedef HXMVECTOR					_hvector;

	typedef XMFLOAT4X4					_float4x4;

	typedef XMMATRIX					_matrix;
	typedef FXMMATRIX					_fmatrix;
	typedef FXMMATRIX					_cmatrix;
}