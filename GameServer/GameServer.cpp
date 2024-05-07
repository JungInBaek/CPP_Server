#include "pch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>
#include "ConcurrentStack.h"
#include "ConcurrentQueue.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "CorePch.h"
#include "RefCounting.h"
#include "Memory.h"

using namespace std;


class Knight
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight(hp)" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

public:
	int32 _hp = 100;
	int32 _mp = 10;
};

int main()
{
	Knight* knight = xnew<Knight>(50);

	xdelete(knight);
}