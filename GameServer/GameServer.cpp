#pragma once
#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"


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

	char sendData[1000] = "Hello World";

	while (true)
	{
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

		BYTE* buffer = sendBuffer->Buffer();
		reinterpret_cast<PacketHeader*>(buffer)->size = sizeof(PacketHeader) + sizeof(sendData);
		reinterpret_cast<PacketHeader*>(buffer)->id = 1;

		::memcpy(&buffer[4], sendData, sizeof(sendData));
		sendBuffer->Close(sizeof(PacketHeader) + sizeof(sendData));

		GSessionManager.Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();

	::WSACleanup();
}