#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>
#include "CorePch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"
#include "Container.h"
#include "MemoryPool.h"

using namespace std;


class Knight
{
public:
	int32 _hp = rand() % 1000;
};

int main()
{
	srand(static_cast<uint64>(time(nullptr)));
	for (int i = 0; i < 2; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Knight* knight = xnew<Knight>();
					cout << knight->_hp << endl;
					this_thread::sleep_for(1s);
					xdelete(knight);
				}
			});
	}

	GThreadManager->Join();
}