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


struct BuffData
{
	uint64 buffId;
	float remainTime;
};

struct S_TEST
{
	uint64 id;
	uint32 hp;
	uint16 att;

	// 가변 데이터
	// 1) 문자열 (ex. name) <- 인코딩 이슈
	// 2) 바이트 배열 (ex. image)
	// 3) 일반 리스트
	vector<BuffData> buffs;

	wstring name;
};

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	uint64 id;
	uint32 hp;
	uint16 att;
	br >> id >> hp >> att;
	cout << "ID: " << id << ", HP: " << hp << ", ATT: " << att << endl;

	// 가변 데이터
	uint16 size;
	br >> size;
	Vector<BuffData> buffs(size);
	cout << "SIZE: " << size << endl;

	for (uint16 i = 0; i < size; i++)
	{
		br >> buffs[i];
		//br >> buffs[i].buffId >> buffs[i].remainTime;
	}

	cout << "BUFFS:";
	for (uint16 i = 0; i < size; i++)
	{
		cout << " { " << buffs[i].buffId << ", " << buffs[i].remainTime << " }";

		if (i != size - 1)
		{
			cout << ", ";
			continue;
		}

		cout << endl;
	}

	// 문자열 데이터
	uint16 nameSize;
	br >> nameSize;
	cout << "NAME SIZE: " << nameSize << endl;

	wstring name;
	name.resize(nameSize);
	br.Read((void*)name.data(), nameSize * sizeof(WCHAR));
	wcout.imbue(std::locale("kor"));
	wcout << L"NAME: " << name << endl;
}