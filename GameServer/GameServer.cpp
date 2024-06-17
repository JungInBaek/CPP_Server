#pragma once
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"
#include "SocketUtils.h"


const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT,
};

struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0;
};

void WorkerThreadMain(HANDLE iocpHandle)
{
	while (true)
	{
		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;

		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, (PULONG_PTR)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);
		if (ret == FALSE || bytesTransferred == 0)
		{
			continue;
		}

		ASSERT_CRASH(overlappedEx->type == READ);

		cout << "Recv Data IOCP = " << bytesTransferred << endl;

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
}


int main()
{
	SOCKET listenSocket = SocketUtils::CreateSocket();
	SocketUtils::BindAnyAddress(listenSocket, 7777);
	SocketUtils::Listen(listenSocket);
	
	vector<Session*> sessionManager;

	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			return 0;
		}

		Session* session = xnew<Session>();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "Client Connected!" << endl;

		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, (ULONG_PTR)session, 0);

		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);

		//Session* s = sessionManager.back();
		//sessionManager.pop_back();

		//::closesocket(s->socket);
		////::WSACloseEvent(wsaEvent);

		//xdelete(s);
	}

	GThreadManager->Join();

	::WSACleanup();
}