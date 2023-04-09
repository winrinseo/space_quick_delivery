#ifndef IOCP__SERVER__H
#define IOCP__SERVER__H

#include "objectPool.cpp"
#include "struct.h"
#include "message.h"
#include "util.h"
#include "gameObject.cpp"
using namespace std;
DataType* Move::sDataType = nullptr;
DataType* CreateRoom::sDataType = nullptr;
DataType* UserPosition::sDataType = nullptr;
DataType* CreateModule::sDataType = nullptr;
DataType* ModuleInteraction::sDataType = nullptr;
DataType* ModuleRepair::sDataType = nullptr;
DataType* ModuleUpgrade::sDataType = nullptr;
DataType* BasicTurret::sDataType = nullptr;
DataType* FactoryInput::sDataType = nullptr;
DataType* FactoryOutput::sDataType = nullptr;
class Iocp
{
	friend unsigned int WINAPI CallWorkerThread(LPVOID p);
	friend unsigned int WINAPI CallAccepterThread(LPVOID p);
public:
	Iocp();
	// virtual ~Iocp();

public:
	//소켓을 초기화하는 함수
	bool InitSocket();
	//소켓의 연결을 종료 시킨다.
	void CloseSocket(ClientInfo* pClientInfo, bool bIsForce = false);

	//서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 그 소켓을 등록하는 함수
	bool BindandListen();
	//접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer();

	//생성되어있는 쓰레드를 파괴한다.
	void DestroyThread();
private:
	//Overlapped I/O작업을 위한 쓰레드를 생성
	bool CreateWorkerThread();
	//accept요청을 처리하는 쓰레드 생성
	bool CreateAccepterThread();

	// CP객체와 Completion Key(완료키)를 등록하는 함수.
	bool BindIOCompletionPort(ClientInfo* pClientInfo);

	//WSARecv Overlapped I/O 작업을 시킨다.
	bool BindRecv(ClientInfo* pClientInfo);

	//WSASend Overlapped I/O작업을 시킨다.
	bool SendMsg(ClientInfo* pClientInfo, char* pMsg, int nLen);

	bool IOProcess(ClientInfo* pClient,OverlappedEx* pOverlapped, DWORD& transferred);

	//Overlapped I/O작업에 대한 완료 통보를 받아 
	//그에 해당하는 처리를 하는 함수
	void WorkerThread();
	//사용자의 접속을 받는 쓰레드
	void AccepterThread();

	//유저 오브젝트 풀
	objectPool<ClientInfo> * userPool;

	//방 관리자
	RoomManager * roomManager;

private:
	
	//클라이언트 정보 구조체
	unordered_map<LL,ClientInfo* > m_pClientInfo;
	//리슨소켓
	SOCKET		m_hSocketListen;
	// 연결된 클라이언트 수
	int			m_clientCount;
	// 워커쓰레드 핸들 배열
	HANDLE		m_hWorkerThread[MAX_WORKERTHREAD];
	// 접속 쓰레드 핸들
	HANDLE		m_hAccepterThread;
	// CP객체
	HANDLE		m_hcp;
	// 작업 쓰레드 BOOL
	bool		m_bWorkerRun;
	// 접속 쓰레드 BOOL
	bool		m_bAccepterRun;
	// 소켓 버퍼
	char		m_socketBuff[MAX_SOCKBUF];

	//접속한 클라이언트 ID (접속 할 때마다 달라짐)
	LL clientNum;

};

#endif