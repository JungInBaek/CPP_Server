#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"


void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
}

#pragma pack(1)
struct PKT_S_TEST
{
	struct BuffListItem
	{
		uint64 buffId;
		float remainTime;
	};

	uint16 packetSize;
	uint16 packetId;
	uint64 id;
	uint32 hp;
	uint16 att;

	// 가변 데이터
	uint16 buffsOffset;
	uint16 buffsCount;

	bool Validate()
	{
		uint32 size = 0;
		
		size += sizeof(PKT_S_TEST);
		size += buffsCount * sizeof(BuffListItem);

		if (size != packetSize)
		{
			return false;
		}

		if (buffsOffset + (buffsCount * sizeof(BuffListItem)) > packetSize)
		{
			return false;
		}

		return true;
	}

	// 가변 데이터
	// 1) 문자열 (ex. name) <- 인코딩 이슈
	// 2) 바이트 배열 (ex. image)
	// 3) 일반 리스트
	//vector<BuffData> buffs;

	//wstring name;
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	if (len < sizeof(PKT_S_TEST))
	{
		return;
	}

	PKT_S_TEST pkt;
	br >> pkt;

	if (pkt.Validate() == false)
	{
		return;
	}

	cout << "ID: " << pkt.id << ", HP: " << pkt.hp << ", ATT: " << pkt.att << endl;

	// 가변 데이터
	Vector<PKT_S_TEST::BuffListItem> buffs(pkt.buffsCount);
	cout << "SIZE: " << pkt.buffsCount << endl;

	for (uint32 i = 0; i < pkt.buffsCount; i++)
	{
		br >> buffs[i];
	}

	cout << "BUFFS:";
	for (uint32 i = 0; i < pkt.buffsCount; i++)
	{
		cout << " { " << buffs[i].buffId << ", " << buffs[i].remainTime << " }";

		if (i != pkt.buffsCount - 1)
		{
			cout << ", ";
			continue;
		}

		cout << endl;
	}
}