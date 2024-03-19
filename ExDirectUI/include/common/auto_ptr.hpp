/**
 * @file auto_ptr.hpp
 * @brief 自动指针实现文件
 * @author EternalZSY
 * @version 5.4.0.0
 * @date 2024-03-10
 */
#pragma once

namespace ExDirectUI
{

	template <class T>
	class ExAutoPtr
	{
	public:
		ExAutoPtr() throw() { m_obj = nullptr; }

		ExAutoPtr(int nNull) throw()
		{
			(void)nNull;
			m_obj = nullptr;
		}

		ExAutoPtr(T* ptr, bool bRef = false) throw()
		{
			m_obj = ptr;
			if (m_obj != nullptr && bRef)
			{
				m_obj->AddRef();
			}
		}

		ExAutoPtr(const ExAutoPtr& pSrc) throw()
		{
			m_obj = pSrc.m_obj;
			if (m_obj)
			{
				m_obj->AddRef();
			}
		}

		~ExAutoPtr() throw()
		{
			if (m_obj)
			{
				m_obj->Release();
			}
		}

		T* operator->() const throw()
		{
			return m_obj;
		}

		operator T* () const throw()
		{
			return m_obj;
		}
		T& operator*() const
		{
			return *m_obj;
		}

		T** operator&() throw()
		{
			return &m_obj;
		}
		T** operator~() throw()
		{
			return ReleaseGetAddr();
		}
		T** operator-() throw()
		{
			return ReleaseGetAddr();
		}
		bool operator!() const throw()
		{
			return (m_obj == nullptr);
		}
		bool operator<(T* pT) const throw()
		{
			return m_obj < pT;
		}
		bool operator!=(T* pT) const
		{
			return !operator==(pT);
		}
		bool operator==(T* pT) const throw()
		{
			return m_obj == pT;
		}

		T* operator=(T* lp) throw()
		{
			if (*this != lp)
			{
				if (m_obj)
					m_obj->Release();

				m_obj = lp;
				if (m_obj)
					m_obj->AddRef();

			}
			return *this;
		}

		T* operator<<(T* p) throw()
		{
			if (*this != p)
			{
				Attach(p);
			}
			return *this;
		}

		T* operator=(const ExAutoPtr<T>& p) throw()
		{
			if (*this != p)
			{
				if (m_obj)
					m_obj->Release();

				m_obj = p;
				if (m_obj)
					m_obj->AddRef();

			}
			return *this;
		}

		T* Get() const throw()
		{
			return m_obj;
		}

		T** GetAddr() const throw()
		{
			return &m_obj;
		}

		T** ReleaseGetAddr() throw()
		{
			T* pTemp = m_obj;
			if (pTemp)
			{
				m_obj = nullptr;
				pTemp->Release();
			}
			return &m_obj;
		}
		long Release() throw()
		{
			long nRef = 0;
			T* pTemp = m_obj;
			if (pTemp)
			{
				m_obj = nullptr;
				nRef = pTemp->Release();
			}
			return nRef;
		}

		void Attach(T* p2) throw()
		{
			if (m_obj)
				m_obj->Release();

			m_obj = p2;
		}

		T* Detach() throw()
		{
			T* pt = m_obj;
			m_obj = nullptr;
			return pt;
		}

		void CopyTo(T** ppT)
		{
			if (!ppT) throw ExException(ES_NULL);

			*ppT = m_obj;
			if (m_obj)
				m_obj->AddRef();
		}

	protected:
		T* m_obj;
	};

}
