#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"


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

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	Protocol::S_TEST pkt;

	ASSERT_CRASH(pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)));

	cout << pkt.id() << " " << pkt.hp() << " " << pkt.att() << " " << endl;
	cout << "BUFFSIZE: " << pkt.buffs_size() << endl;
	for (auto& buff : pkt.buffs())
	{
		cout << "BUFFINFO: " << buff.buffid() << " " << buff.remaintime() << endl;

		cout << "VICTIMS: " << buff.victims_size() << endl;
		for (auto& vic : buff.victims())
		{
			cout << vic << " ";
		}
		cout << endl;
	}
}