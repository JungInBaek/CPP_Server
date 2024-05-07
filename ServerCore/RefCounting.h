#pragma once

/*------------------
	RefCountable
------------------*/
class RefCountable
{
public:
	RefCountable() : _refCount(1) {}
	virtual ~RefCountable() {}

	int32 AddRef()
	{
		return ++_refCount;
	}

	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}

	int32 GetRefCount()
	{
		return _refCount;
	}

protected:
	int32 _refCount;
};


/*--------------
	TSharedPtr
--------------*/

template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr)
	{
		Set(ptr);
	}

	// ����
	TSharedPtr(const TSharedPtr& rhs)
	{
		Set(rhs._ptr);
	}

	// �̵�
	TSharedPtr(TSharedPtr&& rhs)
	{
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
	}

	~TSharedPtr()
	{
		Release();
	}

public:
	// ����
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// �̵�
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool operator==(const TSharedPtr& rhs) const
	{
		return _ptr == rhs._ptr;
	}

	bool operator==(T* ptr) const
	{
		return _ptr == ptr;
	}

	bool operator!=(const TSharedPtr& rhs) const
	{
		return _ptr != rhs._ptr;
	}

	bool operator!=(T* ptr) const
	{
		return _ptr != ptr;
	}

	T* operator*()
	{
		return _ptr;
	}

	const T* operator*() const
	{
		return _ptr;
	}

	T* operator->()
	{
		return _ptr;
	}

	const T* operator->() const
	{
		return _ptr;
	}

	operator T* ()
	{
		return _ptr;
	}

	bool IsNull()
	{
		return _ptr == nullptr;
	}

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
		{
			ptr->AddRef();
		}
	}

	inline void Release()
	{
		if (_ptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};