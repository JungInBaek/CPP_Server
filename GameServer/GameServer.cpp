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


class Player
{
public:
	Player() {}
	virtual ~Player() {}
};

class Knight : public Player
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
	Knight* knight = static_cast<Knight*>(xnew<Player>());
	knight->_hp = 150;
	xdelete(knight);
}