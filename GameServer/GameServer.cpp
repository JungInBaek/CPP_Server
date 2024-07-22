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
#include "DBBind.h"


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
				[gold] INT NULL,							\
				[name] NVARCHAR(50) NULL,					\
				[createDate] DATETIME NULL					\
			);";
		ASSERT_CRASH(connection->Execute(query));

		GDBConnectionPool->Push(connection);
	}

	// Add Data
	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* connection = GDBConnectionPool->Pop();

		auto query = L"INSERT INTO [dbo].[Gold] ([gold], [name], [createDate]) VALUES (?, ?, ?);";
		DBBind<3, 0> dbBind(*connection, query);

		int32 gold = 100;
		WCHAR name[100] = L"홍길동";
		TIMESTAMP_STRUCT createDate = { 2024, 7, 19 };

		dbBind.BindParam(0, gold);
		dbBind.BindParam(1, name);
		dbBind.BindParam(2, createDate);

		ASSERT_CRASH(dbBind.Execute());

		/*
		// 기존 바인딩 된 정보 정리
		connection->Unbind();

		// 정보
		int32 gold = 100;
		SQLLEN len = 0;

		WCHAR name[100] = L"홍길동";
		SQLLEN nameLen = 0;

		TIMESTAMP_STRUCT createDate = {};
		createDate.year = 2024;
		createDate.month = 7;
		createDate.day = 19;
		SQLLEN createDateLen = 0;

		// 정보 바인딩
		ASSERT_CRASH(connection->BindParam(1,&gold, &len));
		ASSERT_CRASH(connection->BindParam(2,name, &nameLen));
		ASSERT_CRASH(connection->BindParam(3,&createDate, &createDateLen));

		// SQL 실행
		auto query = L"INSERT INTO [dbo].[Gold] ([gold], [name], [createDate]) VALUES (?, ?, ?);";
		ASSERT_CRASH(connection->Execute(query));
		*/

		GDBConnectionPool->Push(connection);
	}
	
	// Read
	{
		DBConnection* connection = GDBConnectionPool->Pop();

		auto query = L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?);";
		DBBind<1, 4> dbBind(*connection, query);

		int32 gold = 100;
		dbBind.BindParam(0, gold);

		int32 outId = 0;
		int32 outGold = 0;
		WCHAR outName[100];
		TIMESTAMP_STRUCT outCreateDate = {};

		dbBind.BindCol(0, outId);
		dbBind.BindCol(1, outGold);
		dbBind.BindCol(2, outName);
		dbBind.BindCol(3, outCreateDate);

		ASSERT_CRASH(dbBind.Execute());

		/*
		// 기존 바인딩 된 정보 정리
		connection->Unbind();

		// 정보
		int32 gold = 100;
		SQLLEN len = 0;

		// 정보 바인딩
		ASSERT_CRASH(connection->BindParam(1, &gold, &len));

		int32 outId = 0;
		SQLLEN outIdLen = 0;
		ASSERT_CRASH(connection->BindCol(1, &outId, &outIdLen));

		int32 outGold = 0;
		SQLLEN outGoldLen = 0;
		ASSERT_CRASH(connection->BindCol(2, &outGold, &outGoldLen));

		WCHAR outName[100];
		SQLLEN outNameLen = 0;
		ASSERT_CRASH(connection->BindCol(3, outName, len32(outName), &outNameLen));

		TIMESTAMP_STRUCT outCreateDate;
		SQLLEN outCreateDateLen = 0;
		ASSERT_CRASH(connection->BindCol(4, &outCreateDate, &outCreateDateLen));

		// SQL 실행
		auto query = L"SELECT * FROM [dbo].[Gold] WHERE gold = (?);";
		ASSERT_CRASH(connection->Execute(query));
		*/

		//std::locale::global(std::locale("kor"));
		setlocale(LC_ALL, "");
		while (connection->Fetch())
		{
			wcout << "Id: " << outId << ", Gold: " << outGold << ", Name: " << outName << endl;
			wcout << "Date: " << outCreateDate.year << "/" << outCreateDate.month << "/" << outCreateDate.day << endl;
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