#include "pch.h"
#include "DeadLockProfiler.h"


/*----------------------
	DeadLockProfiler
----------------------*/

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// ���̵� ã�ų� �߱�
	int32 lockId = 0;

	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else
	{
		lockId = findIt->second;
	}

	// ������ ���� ���� �ִ��� Ȯ��
	if (LLockStack.empty()  == false)
	{
		const int32 prevId = LLockStack.top();
		// ���� �ٸ� ���� ������ �Ѵٸ�
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			
			// ������ �湮���� ���� ���̽���� ����� ���θ� �ٽ� Ȯ��
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}
	LLockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (LLockStack.empty())
	{
		CRASH("MULTIPLE_UNLOCK");
	}

	int32 lockId = _nameToId[name];
	if (lockId != LLockStack.top())
	{
		CRASH("INVALID_UNLOCK");
	}

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discorveredCount = 0;
	_discorveredOrder = vector<int32>(lockCount, -1);
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
	{
		Dfs(lockId);
	}

	// ������ �������� ����
	_discorveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discorveredOrder[here] != -1)
	{
		return;
	}
	_discorveredOrder[here] = _discorveredCount++;

	// ������ ��� ����
	auto findIt = _lockHistory.find(here);

	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	// ������ ��� ���� ��ȸ
	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// �湮�� ���� ���ٸ� �湮
		if (_discorveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here�� there���� ���� �߰ߵǾ��ٸ� ������ ����
		if (_discorveredOrder[here] < _discorveredOrder[there])
		{
			continue;
		}

		// �������� �ƴϰ�, Dfs(there)�� ���� ������� �ʾҴٸ� ������ ����
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
				{
					break;
				}
			}

			CRASH("DEADLOCK_DETECTED");
		}
	}

	_finished[here] = true;
}