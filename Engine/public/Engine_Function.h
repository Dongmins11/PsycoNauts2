#pragma once


namespace Engine
{
	template <typename T>
	_float MaxScaleFromFloat3(T vScale)
	{
		_float fMax = vScale.x >= vScale.y ? vScale.x : vScale.y;
		fMax = fMax >= vScale.z ? fMax : vScale.z;
		return fMax;
	}

	template <typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete [] pPointer;
			pPointer = nullptr;
		}
	}

	template <typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long	dwRefCnt = 0;
		if (nullptr != pInstance)		
			dwRefCnt = pInstance->AddRef();			
		
		return dwRefCnt;
	}

	template <typename T>
	unsigned long Safe_Release(T& pInstance)
	{
		unsigned long	dwRefCnt = 0;
		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();
			if (0 == dwRefCnt)
				pInstance = nullptr;			
		}
		return dwRefCnt;
	}

	template<typename T>
	void Safe_PxRelease(T& pInstance)
	{
		if (pInstance)
		{
			pInstance->release();
			pInstance = nullptr;
		}
	}

	class CTagFinder
	{
	public:
		explicit CTagFinder(const wchar_t* pTag) : m_pTag(pTag) {}
		virtual ~CTagFinder() = default;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{
			return !lstrcmp(Pair.first, m_pTag);
		}
	private:
		const wchar_t*		m_pTag = nullptr;
	};

	class CCharTagListFinder
	{
	public:
		explicit CCharTagListFinder(const char* pTag) : m_pTag(pTag) {}
		virtual ~CCharTagListFinder() = default;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{
			return !strcmp(Pair, m_pTag);
		}
	private:
		const char*		m_pTag = nullptr;
	};

	class CWCharTagListFinder
	{
	public:
		explicit CWCharTagListFinder(const wchar_t* pTag) : m_pTag(pTag) {}
		virtual ~CWCharTagListFinder() = default;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{
			return !lstrcmp(Pair, m_pTag);
		}
	private:
		const wchar_t*		m_pTag = nullptr;
	};

	class CTagFinderDefAdd
	{
	public:
		explicit CTagFinderDefAdd(const wchar_t* pTag) : m_pTag(pTag) {}
		virtual ~CTagFinderDefAdd() = default;
	public:
		template <typename T>
		bool operator () (T& Pair)
		{
			return !wcscmp(Pair.first, m_pTag);
		}
	private:
		const wchar_t*		m_pTag = nullptr;
	};

}