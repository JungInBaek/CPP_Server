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


using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

class Player
{
public:
	DECLARE_TL;

	Player()
	{
		INIT_TL(Player);
	}

	virtual ~Player() {}

};

class Mage : public Player
{
public:
	Mage()
	{
		INIT_TL(Mage);
	}
};

class Knight : public Player
{
public:
	Knight()
	{
		INIT_TL(Knight);
	}
};

class Archer : public Player
{
public:
	Archer()
	{
		INIT_TL(Archer);
	}
};

class Dog : public Player
{
public:
	Dog()
	{
		INIT_TL(Dog);
	}
};


int main()
{
	{
		TypeList<Mage, Knight>::Head whoami1;
		TypeList<Mage, Knight>::Tail whoami2;

		TypeList<Mage, Knight, Archer>::Head whoami3;
		TypeList<Mage, Knight, Archer>::Tail::Head whoami4;
		TypeList<Mage, Knight, Archer>::Tail::Tail whoami5;

		using TL = TypeList<Player, Mage, Knight, Archer>;

		int32 len1 = Length<TL>::value;

		TypeAt<TL, 0>::Result whoami6;
		TypeAt<TL, 1>::Result whoami7;
		TypeAt<TL, 2>::Result whoami8;

		int32 index1 = IndexOf<TL, Mage>::value;
		int32 index2 = IndexOf<TL, Knight>::value;
		int32 index3 = IndexOf<TL, Archer>::value;
		int32 index4 = IndexOf<TL, Player>::value;

		bool convert1 = Conversion<Player, Knight>::exists;
		bool convert2 = Conversion<Knight, Player>::exists;
		bool convert3 = Conversion<Knight, Mage>::exists;
		bool convert4 = Conversion<Mage, Knight>::exists;
		bool convert5 = Conversion<Knight, Player>::exists;
		bool convert6 = Conversion<Player, Knight>::exists;
	}

	{
		Player* player = new Player();

		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);

		delete player;
	}

	{
		Player* player = new Knight();

		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);

		delete player;
	}

	{
		Player* player = new Knight();

		bool canCast = CanCast<Archer*>(player);
		Archer* archer = TypeCast<Archer*>(player);

		delete player;
	}

	{
		Player* player = new Knight();

		bool canCast = CanCast<Dog*>(player);
		Dog* dog2 = TypeCast<Dog*>(player);

		delete player;
	}

	{
		Player* dog = new Dog();

		bool canCast = CanCast<Dog*>(dog);
		Dog* player = TypeCast<Dog*>(dog);

		delete dog;
	}

	{
		Dog* dog = new Dog();

		bool canCast = CanCast<Player*>(dog);
		Player* player = TypeCast<Player*>(dog);

		delete dog;
	}

	{
		Player* player = new Player();

		bool canCast = CanCast<Dog*>(player);
		Dog* dog = TypeCast<Dog*>(player);

		delete player;
	}

	{
		shared_ptr<Knight> knight = MakeShared<Knight>();

		shared_ptr<Player> player = TypeCast<Player>(knight);
		bool canCast = CanCast<Player>(knight);
	}

	{
		shared_ptr<Player> player = MakeShared<Knight>();

		shared_ptr<Archer> archer = TypeCast<Archer>(player);
		bool canCast = CanCast<Mage>(player);
	}
}