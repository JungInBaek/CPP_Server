#pragma once
#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Room.h"


void HealByValue(uint64 target, uint32 healValue)
{
	cout << target << "에게 " << healValue << "만큼 힐" << endl;
}

class Knight
{
public:
	void HealMe(uint32 value)
	{
		cout << "HealMe! " << value << endl;
	}
};


int main()
{
	// TEST JOB
	{
		FuncJob<void, uint64, uint32> job(HealByValue, 100, 10);

		job.Execute();
	}
	{
		Knight k1;
		MemberJob<Knight, void, uint32> job(&k1, &Knight::HealMe, 10);
		job.Execute();
	}
	// JOB

	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(NetAddress(L"127.0.0.1", 7777), MakeShared<IocpCore>(), MakeShared<GameSession>, 100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
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

	while (true)
	{
		GRoom.FlushJob();
		this_thread::sleep_for(1ms);
	}

	GThreadManager->Join();

	::WSACleanup();
}