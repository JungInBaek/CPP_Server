#pragma once
#include "Job.h"

extern class Room GRoom;


class Room
{
	friend class EnterJob;
	friend class LeaveJob;
	friend class BroadcastJob;

private:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	void PushJob(JobRef job) { _jobs.Push(job); }
	void FlushJob();

private:
	map<uint64, PlayerRef> _players;
	JobQueue _jobs;
};


class EnterJob : public IJob
{
public:
	EnterJob(Room& room, PlayerRef player)
		: _room(room), _player(player)
	{

	}

	virtual void Execute() override
	{
		_room.Enter(_player);
	}

private:
	Room& _room;
	PlayerRef _player;
};


class LeaveJob : public IJob
{
public:
	LeaveJob(Room& room, PlayerRef player)
		: _room(room), _player(player)
	{

	}

	virtual void Execute() override
	{
		_room.Leave(_player);
	}

private:
	Room& _room;
	PlayerRef _player;
};


class BroadcastJob : public IJob
{
public:
	BroadcastJob(Room& room, SendBufferRef sendBuffer)
		: _room(room), _sendBuffer(sendBuffer)
	{

	}

	virtual void Execute() override
	{
		_room.Broadcast(_sendBuffer);
	}

private:
	Room& _room;
	SendBufferRef _sendBuffer;
};