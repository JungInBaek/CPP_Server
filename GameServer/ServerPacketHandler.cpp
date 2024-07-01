#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"


void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	case S_TEST:
		break;
	}
}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 att, Vector<BuffData> buffs, wstring name)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());
	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << id << hp << att;

	// 가변 데이터
	bw << (uint16)buffs.size();
	bw.Write(buffs.data(), (uint32)buffs.size() * sizeof(BuffData));
	/*for (BuffData& buff : buffs)
	{
		bw << buff.buffId << buff.remainTime;
	}*/

	// 문자열 데이터, WCHAR = UTF-16
	bw << (uint16)name.size();
	bw.Write(name.data(), (uint32)name.size() * sizeof(WCHAR));

	header->size = bw.WriteSize();
	header->id = S_TEST;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}
