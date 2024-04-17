#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"


void Lock::WriteLock()
{
	// ������ �����尡 ���� ���̸� ������ ����
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (lockThreadId == LThreadId)
	{
		_writeCount++;
		return;
	}

	// �ƹ��� ���� �� �����ϰ� ���� ���� ��, ���� �� ������ ȹ��
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = (LThreadId << 16) & WRITE_THREAD_MASK;
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}
		this_thread::yield();
	}
}

void Lock::WriteUnlock()
{
	// ReadLock ���� Ǯ�� ������ WriteUnlock �Ұ���
	const uint32 readCount = _lockFlag.load() & READ_COUNT_MASK;
	if (readCount != 0)
	{
		CRASH("INVALID_UNLOCK_ORDER");
	}

	const uint32 lockCount = --_writeCount;
	if (_writeCount == 0)
	{
		_lockFlag.store(EMPTY_FLAG);
		return;
	}
}

void Lock::ReadLock()
{
	// ������ �����尡 ���� ���̸� ������ ����
	const uint32 lockThreadId = _lockFlag.load() & WRITE_THREAD_MASK;
	if (lockThreadId == LThreadId)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	// �ƹ��� �����ϰ� ���� ���� ��, ���� �� ���� ī��Ʈ ����
	while (true)
	{
		const int64 beginTick = ::GetTickCount64();
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = _lockFlag.load() & READ_COUNT_MASK;
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
			{
				return;
			}
		}
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
		{
			CRASH("LOCK_TIMEOUT");
		}
		this_thread::yield();
	}
}

void Lock::ReadUnlock()
{
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
	{
		CRASH("MULTIPLE_UNLOCK");
	}
}