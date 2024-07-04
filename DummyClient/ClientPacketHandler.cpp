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
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;

		uint16 victimsOffset;
		uint16 victimsCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
		{
			if (victimsOffset + (victimsCount * sizeof(uint64)) > packetSize)
			{
				return false;
			}

			size += victimsCount * sizeof(uint64);
			return true;
		}
	};

	uint16 packetSize;
	uint16 packetId;
	uint64 id;
	uint32 hp;
	uint16 att;

	// 가변 데이터
	uint16 buffsOffset;
	uint16 buffsCount;

	using BuffsVictimsList = PacketList<uint64>;
	using BuffsList = PacketList<BuffsListItem>;

	bool Validate()
	{
		uint32 size = 0;
		
		size += sizeof(PKT_S_TEST);
		if (packetSize < size)
		{
			return false;
		}

		if (buffsOffset + (buffsCount * sizeof(BuffsListItem)) > packetSize)
		{
			return false;
		}

		// 버프 가변 데이터 추가
		size += buffsCount * sizeof(BuffsListItem);

		BuffsList buffs = GetBuffsList();
		for (BuffsListItem& buff : buffs)
		{
			if (buff.Validate(reinterpret_cast<BYTE*>(this), packetSize, OUT size) == false)
			{
				return false;
			}
		}

		// 최종 크기 비교
		if (size != packetSize)
		{
			return false;
		}

		return true;
	}

	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<BuffsListItem*>(data), buffsCount);
	}

	BuffsVictimsList GetVictimsList(BuffsListItem* buffsItem)
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsItem->victimsOffset;
		return BuffsVictimsList(reinterpret_cast<uint64*>(data), buffsItem->victimsCount);
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

		PKT_S_TEST::BuffsVictimsList victims = pkt->GetVictimsList(&buffs[i]);
		cout << "VICTIMS_COUNT: " << victims.Count() << endl;

		for (uint32 j = 0; j < victims.Count(); j++)
		{
			cout << "VICTIM: " << victims[j] << endl;
		}
	}

	for (auto it = buffs.begin(); it != buffs.end(); ++it)
	{
		cout << "BUFF_INFO: { " << it->buffId << ", " << it->remainTime << " }" << endl;

		PKT_S_TEST::BuffsVictimsList victims = pkt->GetVictimsList(&(*it));
		cout << "VICTIMS_COUNT: " << victims.Count() << endl;

		for (auto vit = victims.begin(); vit != victims.end(); ++vit)
		{
			cout << "VICTIM: " << *vit << endl;
		}
	}

	for (auto& buff : buffs)
	{
		cout << "BUFF_INFO: { " << buff.buffId << ", " << buff.remainTime << " }" << endl;

		PKT_S_TEST::BuffsVictimsList victims = pkt->GetVictimsList(&buff);
		cout << "VICTIMS_COUNT: " << victims.Count() << endl;

		for (uint64& victim : victims)
		{
			cout << "VICTIM: " << victim << endl;
		}
	}
}