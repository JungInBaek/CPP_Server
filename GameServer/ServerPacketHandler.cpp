#include "pch.h"
#include "ServerPacketHandler.h"


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
