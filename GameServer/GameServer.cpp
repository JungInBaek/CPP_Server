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

using namespace std;


// 소수 구하기
// 1과 자기 자신으로만 나누어 떨어지는 1보다 큰 양의 정수


bool isPrime(int number)
{
	if (number <= 1)
	{
		return false;
	}
	if (number == 2 || number == 3)
	{
		return true;
	}

	for (int i = 2; i < number; i++)
	{
		if (number % i == 0)
		{
			return false;
		}
	}

	return true;
}

int CountPrime(int start, int end, int coreCount)
{
	int count = 0;
	for (int number = start; number <= end; number += coreCount)
	{
		if (isPrime(number))
		{
			count++;
		}
	}
	return count;
}

int main()
{
	const int MAX_NUMBER = 100'0000;

	vector<thread> threads;

	int coreCount = thread::hardware_concurrency();
	int jobCount = (MAX_NUMBER / coreCount) + 1;

	atomic<int32> count = 0;
	for (int i = 0; i < coreCount; i++)
	{
		//int start = (i * jobCount) + 1;
		int start = i + 1;
		//int end = min((i + 1) * jobCount, MAX_NUMBER);

		/*threads.push_back(thread([=, &count]()
			{
				count += CountPrime(start, end);
			})
		);*/
		threads.push_back(thread([=, &count]()
			{
				count += CountPrime(start, MAX_NUMBER, coreCount);
			})
		);
	}

	for (thread& t : threads)
	{
		t.join();
	}

	cout << count << endl;
}