#ifndef IOCP__SERVER__CPP
#define IOCP__SERVER__CPP

#include "IocpServer.h"
Iocp::Iocp() :m_hSocketListen(INVALID_SOCKET),
	m_clientCount(0),m_hAccepterThread(INVALID_HANDLE_VALUE),m_hcp(INVALID_HANDLE_VALUE),
	m_bWorkerRun(true),m_bAccepterRun(true),clientNum(0)
{
	for (int i = 0; i < MAX_WORKERTHREAD; ++i)
		m_hWorkerThread[i] = INVALID_HANDLE_VALUE;
	ZeroMemory(m_socketBuff, sizeof(MAX_SOCKBUF));

    //쓰레드락 필요
    userPool = new objectPool<ClientInfo>(MAX_CLIENT);

    roomManager = new RoomManager;
}

bool Iocp::InitSocket(){
    WSADATA wsadata;

    if(WSAStartup(MAKEWORD(2,2),&wsadata) != 0){
        cout<<"할당 실패"<<"\n";
        return false;
    }
    m_pClientInfo[0] = userPool->getObject();
    m_pClientInfo[0]->m_socketClient = WSASocket(AF_INET,SOCK_STREAM,0,0,0,WSA_FLAG_OVERLAPPED);

    if(m_pClientInfo[0]->m_socketClient == INVALID_SOCKET){
        cout<<"할당 실패"<<"\n";
        return false;
    }

    return true;
}

bool Iocp::BindandListen(){
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT_NUM);

    bind(m_pClientInfo[0]->m_socketClient,(SOCKADDR * ) & addr , sizeof(addr));
    
    if(listen(m_pClientInfo[0]->m_socketClient,SOMAXCONN) == SOCKET_ERROR){
        return false;
    }

    return true;
}

bool Iocp::StartServer(){
    Move::InitDataType();
    CreateRoom::InitDataType();
    UserPosition::InitDataType();
    CreateModule::InitDataType();
    ModuleInteraction::InitDataType();
    ModuleRepair::InitDataType();
    ModuleUpgrade::InitDataType();
    BasicTurret::InitDataType();
    FactoryInput::InitDataType();
    FactoryOutput::InitDataType();
   m_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE,0,0,0);

   if(m_hcp == INVALID_HANDLE_VALUE){
        return false;
   }

   bool ret = CreateWorkerThread();
   if(!ret) return false;

   ret = CreateAccepterThread();
   if(!ret) return false;

   cout<<"Server Start"<<"\n";

   return false;
}

unsigned __stdcall CallWorkerThread(LPVOID p){
    Iocp* pEvent = (Iocp*)p;

    pEvent->WorkerThread();

    return 0;
}

bool Iocp::CreateWorkerThread(){
    UINT threadId = 0;
    for(int i = 0; i<MAX_WORKERTHREAD; ++i){
        m_hWorkerThread[i] = (HANDLE)_beginthreadex(NULL,0,CallWorkerThread,this,CREATE_SUSPENDED,&threadId);
        if(m_hWorkerThread[i] == INVALID_HANDLE_VALUE){
            cout<<"create thread failed!!"<<"\n";
            return false;
        }
        ResumeThread(m_hWorkerThread[i]);
    }
    cout<<"Worker Thread start"<<"\n";
    return true;
}

void Iocp::WorkerThread()
{
	//CompletionKey(??? ?)소켓 키를 반환받는다.
	ClientInfo* cpKey = nullptr;
	// 작업 할당 성공 여부
	bool bSuccess = true;
	//Overlapped I/O??????? ?????? ?????? ???
	DWORD transferred = 0;
	// Overlapped I/O??????? ????? Overlapped?????
	LPOVERLAPPED pOverlapped = nullptr;
	OverlappedEx* originOverlapped = nullptr;
	while (m_bWorkerRun)
	{
		//iocp queue에서 작업을 하나 꺼내 수행한다.
		
		bSuccess = GetQueuedCompletionStatus(m_hcp, &transferred, (ULONG_PTR*)&cpKey,
			&pOverlapped, INFINITE);
        
		// ?????? ?????? ?????? ??.
		if (transferred == 0 && bSuccess == FALSE){
            // if(cpKey->userId != -1){
            //     RoomInfo * room = roomManager->getRoom(cpKey->roomName);
            //     room->participate.erase(cpKey->userId);
            //     room->userCnt--;
            // }
			closesocket(cpKey->m_socketClient);
            m_pClientInfo.erase(cpKey->ID);
            userPool->putObject(cpKey);
			continue;
		}
        if (transferred == 0){
			closesocket(cpKey->m_socketClient);
            m_pClientInfo.erase(cpKey->ID);
            userPool->putObject(cpKey);
			continue;
		}
		// ????? ?????? ???? ????? ???
		if (bSuccess == TRUE && transferred == 0 && pOverlapped == nullptr)
		{
			m_bWorkerRun = false;
			continue;
		}
		if (pOverlapped == nullptr)
			continue;

		//??????
		originOverlapped = (OverlappedEx*)pOverlapped;


		bool bRet = IOProcess(cpKey,originOverlapped,transferred);
		if (bRet == false)
		{
			m_bWorkerRun = false;
			continue;
		}
	}
}

bool Iocp::IOProcess(ClientInfo * pClient , OverlappedEx* pOverlapped,DWORD& transferred){
    switch(pOverlapped->m_ioType){
        case IO_TYPE::ACCEPT:
            cout<<"유저 접속"<<"\n";
            break;
        case IO_TYPE::IO_RECV:
            pOverlapped->m_dataBuffer[transferred] = '\0';
            // 수신한 메세지의 타입을 확인 후 각기 다른 연산 수행
            {
                InputMemoryStream inputStream(pOverlapped->m_dataBuffer,static_cast<uint32_t>(transferred));
                PacketType pt = PacketType::NONE;
                void* ret = Read(&inputStream,pt);
                std::cout<<"수신 "<<(int)pt<<"\n";
                switch(pt){
                    case PacketType::CREATE_ROOM:
                        {
                            std::cout<<"방 생성"<<"\n";
                            CreateRoom * c = static_cast<CreateRoom*>(ret);
                            //방 코드 생성
                            std::string code = randCode();
                            while(roomManager->existRoom(code)){code = randCode();}
                            //방 등록
                            roomManager->createRoom(code,c->name, pClient);
                            //방 코드를 객체에 추가해 해당 유저에게 전송
                            c->roomId = code;
                            OutputMemoryStream outputStream;
                            Write(&outputStream,(uint8_t*)c , PacketType::CREATE_ROOM);
                            SendMsg(pClient,(char*)outputStream.GetBufferPtr(),outputStream.GetLength());
                            std::cout<<(c->roomId)<<" "<<(c->name)<<" "<<"\n";
                            delete c;
                        }
                        break;
                    case PacketType::PARTICIPATE_USER:
                        {
                            std::cout<<"방 참여"<<"\n";
                            CreateRoom * c = static_cast<CreateRoom*>(ret);
                            if(roomManager->participateRoom(c->roomId,c->name,pClient)){
                                
                            }
                            delete c;
                        }
                        break;
                    case PacketType::MOVE:
                        {
                            Move * c = static_cast<Move*>(ret);
                            //방 안의 유저를 옮기고 그대로 에코한다.
                            std::cout<<c->roomId<<" "<<c->userNum<<" "<<c->x<<" "<<c->y<<" 이동"<<"\n";
                            roomManager->moveUser(c);
                            delete c;
                        }
                        break;
                    case PacketType::OUT_USER:
                        {
                            roomManager->outRoom(pClient);
                            char buf[1] = {5};
                            SendMsg(pClient,buf,1);
                        }
                        break;
                    case PacketType::START_GAME:
                        {
                            string * c = static_cast<string*>(ret);
                            roomManager->startGame(c);
                            delete c;
                        }
                        break;

                    case PacketType::MODULE_CREATE:
                        {
                            CreateModule * c = static_cast<CreateModule*>(ret);
                            roomManager->createModule(c);
                            delete c;
                        }
                        break;

                    case PacketType::MODULE_INTERACTION:
                        {
                            ModuleInteraction * c = static_cast<ModuleInteraction*>(ret);
                            roomManager->interactionModule(c);
                            delete c;
                        }
                        break;

                    case PacketType::MODULE_REPAIR:
                        {
                            ModuleRepair * c = static_cast<ModuleRepair*>(ret);
                            roomManager->repairModule(c);
                            delete c;
                        }
                        break;

                    case PacketType::MODULE_UPGRADE:
                        {
                            ModuleUpgrade * c = static_cast<ModuleUpgrade*>(ret);
                            roomManager->upgradeModule(c);
                            delete c;
                        }
                        break;
                    case PacketType::BASIC_TURRET:
                        {
                            BasicTurret * c = static_cast<BasicTurret*>(ret);
                            roomManager->basicTurret(c);
                            delete c;
                        }
                        break;

                    case PacketType::FACTORY_INPUT:
                        {
                            FactoryInput * c = static_cast<FactoryInput*>(ret);
                            roomManager->factoryInput(c);
                            delete c;
                        }
                        break;

                    default:
                        std::cout<<"알수없는 타입"<<"\n";
                        
                        break;
                }
            }
            break;
        case IO_TYPE::IO_SEND:
            BindRecv(pClient);
            break;
        
        default:
            return false;
    }
    return true;
}

bool Iocp::SendMsg(ClientInfo * pClient,char* pMsg, int nLen)
{
    DWORD recvNumBytes = 0;

    // Overlapped I/O?? ???? ????
    pClient->m_overlapped.m_wsaBuf.len = nLen;
    pClient->m_overlapped.m_wsaBuf.buf = pMsg;
    pClient->m_overlapped.m_ioType = IO_TYPE::IO_SEND;
    // OutputMemoryStream stream;

    int ret = WSASend(pClient->m_socketClient, &pClient->m_overlapped.m_wsaBuf,
        1, &recvNumBytes, 0, (LPWSAOVERLAPPED)&pClient->m_overlapped, nullptr);

    // ???????? PENDING???? ????? ???. (?????? ?????? ???)
    if (ret == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
    {
        return false;
    }

    return true;
}


bool Iocp::BindRecv(ClientInfo * pClient)
{
    DWORD flag = 0;
    DWORD recvNumBytes = 0;

    //Overlapped I/O 
    pClient->m_overlapped.m_wsaBuf.len = MAX_SOCKBUF;
    pClient->m_overlapped.m_wsaBuf.buf = pClient->m_overlapped.m_dataBuffer;
    pClient->m_overlapped.m_ioType = IO_TYPE::IO_RECV;

    // ??? ????? ??? ????? ???(recvNumBytes?? ???? ?????? ????) ?????????(PENDING)
    int ret = WSARecv(pClient->m_socketClient, &pClient->m_overlapped.m_wsaBuf,
        1, &recvNumBytes, &flag, (LPWSAOVERLAPPED)&pClient->m_overlapped, nullptr);
    
    // ???????? PENDING???? ????? ???. (?????? ?????? ???)
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        return false;
    }
    // ??? ????????? ??? ????? ???? PENDNG????? ??????????????, ???????? ??? ??? false????.
    
    return true;
}

unsigned __stdcall CallAccepterThread(LPVOID p){
    Iocp* pEvent = (Iocp*) p;
    pEvent->AccepterThread();
    return 0;
}

bool Iocp::CreateAccepterThread(){
    UINT threadId = 0;
    m_hAccepterThread = (HANDLE) _beginthreadex(NULL,0,CallAccepterThread,this,CREATE_SUSPENDED,&threadId);

    if(m_hAccepterThread == INVALID_HANDLE_VALUE){
        cout<<"create accepter thread failed"<<"\n";
        return false;
    }
    ResumeThread(m_hAccepterThread);
    cout<<"Create Accepter Thread Start"<<"\n";
    return true;
}

void Iocp::AccepterThread(){
    SOCKADDR_IN addr;
    int addrlen = sizeof(addr);
    while(m_bAccepterRun){
        ClientInfo * pClient = userPool->getObject();
        if(pClient == nullptr){
            return;
        }
        
        pClient->m_socketClient = accept(m_pClientInfo[0]->m_socketClient,(SOCKADDR*) &addr,&addrlen);
        if(pClient->m_socketClient == INVALID_SOCKET){
            if(WSAGetLastError() == WSAEWOULDBLOCK)
                continue;
            break;
        }

        bool ret = BindIOCompletionPort(pClient);
        if(!ret) return;
        
    }
}

bool Iocp::BindIOCompletionPort(ClientInfo* pClientInfo)
{
	HANDLE compareCP;
	compareCP = CreateIoCompletionPort((HANDLE)pClientInfo->m_socketClient,
		m_hcp, reinterpret_cast<ULONG_PTR>(pClientInfo),
		0);
	if (compareCP == NULL || compareCP != m_hcp){
        cout<<"bnding failed!!"<<"\n";
		return false;
    } 

	bool check = BindRecv(pClientInfo); // 일단 Recv를 건다
	while(!check)
		check = BindRecv(pClientInfo);
    cout<<"bnding successed!!"<<"\n";


    int cnum = ++clientNum;
    m_pClientInfo[cnum] = pClientInfo;
    pClientInfo->ID = cnum;
    ++m_clientCount;
    cout<<"user accept!!"<<"\n";
	return true;
}
#endif