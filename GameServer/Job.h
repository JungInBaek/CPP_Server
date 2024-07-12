#pragma once


class IJob
{
public:
	virtual void Execute() {}
};

class HealJob : public IJob
{
public:
	virtual void Execute() override
	{
		cout << _target << "에게 " << _healValue << "만큼 힐" << endl;
	}

public:
	uint64 _target = 0;
	uint32 _healValue = 0;
};


using JobRef = shared_ptr<IJob>;


class JobQueue
{
public:
	void Push(JobRef job)
	{
		WRITE_LOCK;
		_jobs.push(job);
	}

	JobRef Pop()
	{
		WRITE_LOCK;
		if (_jobs.empty())
		{
			return nullptr;
		}
		JobRef job = _jobs.back();
		_jobs.pop();
		return job;
	}

private:
	USE_LOCK;
	queue<JobRef> _jobs;
};