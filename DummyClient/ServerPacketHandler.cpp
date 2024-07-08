#include "pch.h"
#include "ServerPacketHandler.h"


PacketHandlerFunc GPacketHandler[UINT16_MAX];


bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

    // TODO: Log

    return false;
}

bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt)
{
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
    return true;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
    return true;
}