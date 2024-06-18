#pragma once


class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	PreRecv,
	Recv,
	Send,
};

// 가상함수 사용 주의: 0번 offset이 OVERLAPPED여야 하는데 가상함수 테이블이 채워짐, 상속일 경우 0번 offset은 OVERLAPPED 유지됨
/*----------------------
		IocpEvent
----------------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return _type; }
	
protected:
	EventType _type;
};


/*-------------------------
		ConnectEvent
-------------------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

/*-------------------------
		AcceptEvent
-------------------------*/

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	void SetSession(Session* session)
	{
		_session = session;
	}

	Session* GetSession()
	{
		return _session;
	}

private:
	Session* _session = nullptr;
};


/*-------------------------
		RecvEvent
-------------------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};


/*-------------------------
		SendEvent
-------------------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}
};