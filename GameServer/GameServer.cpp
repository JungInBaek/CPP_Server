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

using namespace std;


class Wraight : public RefCountable
{
public:
	int _hp = 150;
	int _posX = 0;
	int _posY = 0;
};

using WraightRef = TSharedPtr<Wraight>;

class Missile : public RefCountable
{
public:
	void SetTarget(WraightRef target)
	{
		_target = target;
	}

	bool Update()
	{
		if (_target == nullptr)
		{
			return true;
		}

		int posX = _target->_posX;
		int posY = _target->_posY;

		// TODO: 쫒아간다

		if (_target->_hp == 0)
		{
			_target = nullptr;
			return true;
		}

		return false;
	}

	WraightRef _target = nullptr;
};

using MissileRef = TSharedPtr<Missile>;


int main()
{
	WraightRef wraight(new Wraight());
	wraight->ReleaseRef();

	MissileRef missile(new Missile());
	missile->ReleaseRef();

	missile->SetTarget(wraight);

	wraight->_hp = 0;
	wraight = nullptr;

	while (true)
	{
		if (missile)
		{
			if (missile->Update())
			{
				missile = nullptr;
			}
		}
		else
		{
			break;
		}
	}
}