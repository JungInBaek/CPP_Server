#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include "AccountManager.h"
#include "UserManager.h"
#include <future>
#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"
#include "ThreadManager.h"

using namespace std;


CoreGlobal Core;

void ThreadMain()
{
	while (true)
	{
		cout << "Hello! I am thread... " << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadMain);
	}
	GThreadManager->Join();
}