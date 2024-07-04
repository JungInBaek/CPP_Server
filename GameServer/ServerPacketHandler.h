#pragma once
#include "BufferReader.h"
#include "BufferWriter.h"


enum
{
	S_TEST = 1,
};

template<typename T, typename C>
class PacketIterator
{
public:
	PacketIterator(C& container, uint16 index) : _container(container), _index(index) {}

	bool operator!=(const PacketIterator& other) const { return _index != other._index; }
	const T& operator*() const { return _container[_index]; }
	T& operator*() { return _container[_index]; }
	T* operator->() { return &_container[_index]; }
	PacketIterator& operator++() { _index++; return *this; }
	PacketIterator operator++(int32) { PacketIterator ret = *this; ++_index; return ret; }

private:
	C& _container;
	uint16 _index;
};

template<typename T>
class PacketList
{
public:
	PacketList() : _data(nullptr), _count(0) {}
	PacketList(T* data, uint16 count) : _data(data), _count(count) {}

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);
		return _data[index];
	}

	uint16 Count() { return _count; }

	// range-base for 지원
	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

private:
	T* _data;
	uint16 _count;
};


class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len);
};


#pragma pack(1)
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;

		// Victim List
		uint16 victimsOffset;
		uint16 victimsCount;
	};

	uint16 packetSize;
	uint16 packetId;
	uint64 id;
	uint32 hp;
	uint16 att;

	// 가변 데이터
	uint16 buffsOffset;
	uint16 buffsCount;
};

class PKT_S_TEST_WRITE
{
public:
	using BuffsListItem = PKT_S_TEST::BuffsListItem;
	using BuffsList = PacketList<BuffsListItem>;
	using BuffsVictimsList = PacketList<uint64>;


	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 att)
	{
		_sendBuffer = GSendBufferManager->Open(4096);
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		_pkt = _bw.Reserve<PKT_S_TEST>();
		_pkt->packetSize = 0;
		_pkt->packetId = S_TEST;
		_pkt->id = id;
		_pkt->hp = hp;
		_pkt->att = att;
		_pkt->buffsOffset = 0;
		_pkt->buffsCount = 0;
	}

	BuffsList ReserveBuffsList(uint16 buffCount)
	{
		_pkt->buffsOffset = _bw.WriteSize();
		_pkt->buffsCount = buffCount;
		BuffsListItem* buffs = _bw.Reserve<BuffsListItem>(buffCount);
		return BuffsList(buffs, buffCount);
	}

	BuffsVictimsList ReserveBuffsVictimsList(BuffsListItem* buffsItem, uint16 victimsCount)
	{
		buffsItem->victimsOffset = _bw.WriteSize();
		buffsItem->victimsCount = victimsCount;
		uint64* victims = _bw.Reserve<uint64>(victimsCount);
		return BuffsVictimsList(victims, victimsCount);
	}

	SendBufferRef CloseAndReturn()
	{
		_pkt->packetSize = _bw.WriteSize();
		_sendBuffer->Close(_bw.WriteSize());
		return _sendBuffer;
	}


private:
	PKT_S_TEST* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bw;
};

#pragma pack()