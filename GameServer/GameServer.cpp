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
#include "DBConnectionPool.h"


enum
{
	WORKER_TICK = 64
};


void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;
		service->GetIocpCore()->Dispatch(10);

		// 예약된 JobQueue
		ThreadManager::DistributeReservedJobs();

		// 글로벌 JobQueue
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	//

	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\ProjectModels;Database=serverDB;Trusted_Connection=Yes;"));

	// Create Table
	{
		DBConnection* connection = GDBConnectionPool->Pop();

		auto query = L"										\
			DROP TABLE IF EXISTS [dbo].[Gold];				\
			CREATE TABLE [dbo].[Gold]						\
			(												\
				[id] INT NOT NULL PRIMARY KEY IDENTITY,		\
				[gold] INT NULL								\
			);";
		ASSERT_CRASH(connection->Execute(query));

		GDBConnectionPool->Push(connection);
	}

	// Add Data
	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* connection = GDBConnectionPool->Pop();

		// 기존 바인딩 된 정보 정리
		connection->Unbind();

		// 정보
		int32 gold = 100;
		SQLLEN len = 0;

		// 정보 바인딩
		ASSERT_CRASH(connection->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

		// SQL 실행
		auto query = L"INSERT INTO [dbo].[Gold] (gold) VALUES (?);";
		ASSERT_CRASH(connection->Execute(query));

		GDBConnectionPool->Push(connection);
	}
	
	// Read
	{
		DBConnection* connection = GDBConnectionPool->Pop();

		// 기존 바인딩 된 정보 정리
		connection->Unbind();

		// 정보
		int32 gold = 100;
		SQLLEN len = 0;

		// 정보 바인딩
		ASSERT_CRASH(connection->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

		int32 outId = 0;
		SQLLEN outIdLen = 0;
		ASSERT_CRASH(connection->BindCol(1, SQL_C_LONG, OUT &outId, sizeof(outId), &outIdLen));

		int32 outGold = 0;
		SQLLEN outGoldLen = 0;
		ASSERT_CRASH(connection->BindCol(2, SQL_C_LONG, OUT &outGold, sizeof(outGold), &outGoldLen));

		// SQL 실행
		auto query = L"SELECT id, gold FROM [dbo].[Gold] WHERE gold = (?);";
		ASSERT_CRASH(connection->Execute(query));

		while (connection->Fetch())
		{
			wcout << "Id: " << outId << ", Gold: " << outGold << endl;
		}

		GDBConnectionPool->Push(connection);
	}

	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(NetAddress(L"127.0.0.1", 7777), MakeShared<IocpCore>(), MakeShared<GameSession>, 100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(
			[&service]()
			{
				DoWorkerJob(service);
			}
		);
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}