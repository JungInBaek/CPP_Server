#pragma once
#include "JobSerializer.h"


class Room : public JobSerializer
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	virtual void FlushJob() override;

private:
	map<uint64, PlayerRef> _players;
};

extern shared_ptr<Room> GRoom;