#pragma once


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