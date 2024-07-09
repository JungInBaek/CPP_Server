#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"


/*---------------------
		IocpCore
---------------------*/

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, 0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (false == ::GetQueuedCompletionStatus(_iocpHandle, OUT &numOfBytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO: 로그 찍기
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			return true;
		}
	}

	IocpObjectRef iocpObject = iocpEvent->owner;
	iocpObject->Dispatch(iocpEvent, numOfBytes);
	return true;
}