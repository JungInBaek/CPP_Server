#pragma once
#include <iostream>
#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"


class GameSession : public Session
{
public:
	virtual ~GameSession() override
	{
		cout << "~GameSession" << endl;
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "OnRecv Len = " << len << endl;

		Send(buffer, len);

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}
};

const int32 BUFSIZE = 1000;

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(NetAddress(L"127.0.0.1", 7777), MakeShared<IocpCore>(), MakeShared<GameSession>, 100);

	ASSERT_CRASH(service->Start());

	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch(
			[=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			}
		);
	}

	GThreadManager->Join();

	::WSACleanup();
}