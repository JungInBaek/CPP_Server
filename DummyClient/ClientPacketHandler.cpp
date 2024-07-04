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
		if (packetSize < size)
		{
			return false;
		}

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

	using BuffsList = PacketList<BuffListItem>;

	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<BuffListItem*>(data), buffsCount);
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

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == false)
	{
		return;
	}

	cout << "ID: " << pkt->id << ", HP: " << pkt->hp << ", ATT: " << pkt->att << endl;


	// 가변 데이터
	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();
	cout << "BUFF_COUNT: " << buffs.Count() << endl;

	for (uint32 i = 0; i < buffs.Count(); i++)
	{
		cout << "BUFF_INFO: { " << buffs[i].buffId << ", " << buffs[i].remainTime << " }" << endl;
	}

	for (auto it = buffs.begin(); it != buffs.end(); ++it)
	{
		cout << "BUFF_INFO: { " << it->buffId << ", " << it->remainTime << " }" << endl;
	}

	for (auto& buff : buffs)
	{
		cout << "BUFF_INFO: { " << buff.buffId << ", " << buff.remainTime << " }" << endl;
	}
}