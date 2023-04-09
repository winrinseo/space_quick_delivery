#ifndef GAME__OBJECT__CPP
#define GAME__OBJECT__CPP
#include "gameObject.h"

RoomInfo::RoomInfo(){
    userCnt = 0;
    stage = 0;
	mobcnt = 0;
}

RoomManager::RoomManager(){

};
//�� ���� �� ȣ��Ʈ ����
void RoomManager::createRoom(std::string& roomName,std::string& userName,ClientInfo* pClient){
    RoomInfo * room = new RoomInfo;
    party[roomName] = room;
    int userNum = room->userNum++;
    room->userCnt++;
    pClient->userId = userNum;
    pClient->roomName = roomName;
    pClient->userName = userName;
    room->participate[userNum] = pClient;
    room->userPosition[userNum] = {roomName,userNum,0,0,0,0,userName};
    std::cout<<"�� �ο� : "<<room->userPosition.size()<<"\n";

}
//������ ���� �Ǿ� �ִ� �濡 ���� ����
bool RoomManager::participateRoom(std::string& roomName,std::string& userName,ClientInfo* pClient){
    std::cout<<roomName<<"\n";
    if(party.find(roomName) == party.end())
        return false;
    RoomInfo * room = party[roomName];
    if(room->userCnt == 4) return false;
    int userNum = room->userNum++;
    int userCnt = ++room->userCnt;
    cout<<"���� ���̵� : "<<userNum<<"\n";
    pClient->userId = userNum;
    pClient->roomName = roomName;
    pClient->userName = userName;
    room->participate[userNum] = pClient;
    room->userPosition[userNum] = {roomName,userNum,0,0,0,0,userName};

    //���� �� �濡 ������ �ڽ��� ������ �������� ��� �޼��� �۽�
    ParticipateRoom part = {1 , userCnt, room->userPosition};
    OutputMemoryStream outputStream;
    Write(&outputStream,(uint8_t*)&part,PacketType::PARTICIPATE_ROOM);
    sendRoom(roomName,outputStream.GetBufferPtr(),outputStream.GetLength());
    return true;
}

bool RoomManager::outRoom(ClientInfo* pClient){
    if(party.find(pClient->roomName) == party.end())
        return false;
    RoomInfo * room = getRoom(pClient->roomName);
    room->participate.erase(pClient->userId);
    room->userPosition.erase(pClient->userId);
    int userCnt = --room->userCnt;

    ParticipateRoom part = {1 , userCnt, room->userPosition};
    OutputMemoryStream outputStream;
    Write(&outputStream,(uint8_t*)&part,PacketType::PARTICIPATE_ROOM);
    sendRoom(pClient->roomName,outputStream.GetBufferPtr(),outputStream.GetLength());
    
    std::cout<<"���̸� : "<<pClient->roomName<<"\n";
    pClient->userId = -1;
    pClient->roomName = "";
    pClient->userName = "";
    return true;
}


void RoomManager::moveUser(Move* go){
    if(party.find(go->roomId) == party.end()){
        std::cout<<"�������� �ʴ� ���ȣ"<<"\n";
        return;
    }
    RoomInfo * room = party[go->roomId];
    room->userPosition[go->userNum].x = go->x;
    room->userPosition[go->userNum].y = go->y;
    room->userPosition[go->userNum].z = go->z;

    OutputMemoryStream outputStream;
    Write(&outputStream,(uint8_t*)go,PacketType::MOVE);
    sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
}
void RoomManager::startGame(string* roomName){
    OutputMemoryStream outputStream;
    Write(&outputStream,(uint8_t*)roomName,PacketType::START_GAME);
    sendRoom(*roomName,outputStream.GetBufferPtr(),outputStream.GetLength());
}

void RoomManager::createModule(CreateModule * go){
    RoomInfo * room = party[go->roomId];

    MODULE_CODE code = (MODULE_CODE)go->code;
    
    if(room->field[go->x][go->y] != MODULE_CODE::EMPTY){

    }else{
        room->field[go->x][go->y] = code;
        OutputMemoryStream outputStream;
        Write(&outputStream,(uint8_t*)go,PacketType::MODULE_CREATE);
        sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
    }
}

void RoomManager::interactionModule(ModuleInteraction * go){

        //���� �ʿ��ϸ� ���⼭

       if(go->activeNum == 1){
            RoomInfo * room = party[go->roomId];
            FactoryOutput * ret = new FactoryOutput;
            room->rOrder++;
            room->rOrder %= 4;

            int & ore = room->resource[0];
            int & fuel = room->resource[1];
            if(room->rOrder == 0 && ore >= 1 && fuel >= 1){
                ret->isMade = 1;
                ore -= 1; fuel -= 1;
                ret->type = 0;
            }else if( room->rOrder == 1 && ore >= 2 && fuel >= 1){
                ret->isMade = 1;
                ore -=2; fuel -= 1;
                ret->type = 1;
            }else if(room->rOrder == 2 &&ore >= 1 && fuel >= 2){
                ret->isMade = 1;
                ore -= 1; fuel -= 2;
                ret->type = 2;
            }else if(room->rOrder == 3 && ore >= 0 && fuel >= 3){
                ret->isMade = 1;
                ore -= 0; fuel -= 3;
                ret->type = 3;
            }else{//���� ���� ��� ���� x
                ret->isMade = 0;
            }
            OutputMemoryStream outputStream;
            Write(&outputStream,(uint8_t*)ret,PacketType::FACTORY_OUTPUT);
            sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
            delete ret;
       }
        OutputMemoryStream outputStream;
        Write(&outputStream,(uint8_t*)go,PacketType::MODULE_INTERACTION);
        sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
}

void RoomManager::repairModule(ModuleRepair * go){

        //���� �ʿ��ϸ� ���⼭

        OutputMemoryStream outputStream;
        Write(&outputStream,(uint8_t*)go,PacketType::MODULE_REPAIR);
        sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
}

void RoomManager::upgradeModule(ModuleUpgrade * go){

        //���� �ʿ��ϸ� ���⼭

        OutputMemoryStream outputStream;
        Write(&outputStream,(uint8_t*)go,PacketType::MODULE_UPGRADE);
        sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
}


void RoomManager::basicTurret(BasicTurret * go){

    OutputMemoryStream outputStream;
    Write(&outputStream,(uint8_t*)go,PacketType::BASIC_TURRET);
    sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
}

void RoomManager::factoryInput(FactoryInput * go){

    RoomInfo * room = party[go->roomId];

    FactoryOutput * ret = new FactoryOutput;

    //�ڿ� �ϳ� �÷���
    room->resource[go->resourceType]++;
    
    //���ǿ� ���� ���
    int & ore = room->resource[0];
    int & fuel = room->resource[1];
    if(room->rOrder == 0 && ore >= 1 && fuel >= 1){
        ret->isMade = 1;
        ore -= 1; fuel -= 1;
        ret->type = 0;
    }else if(room->rOrder == 1 && ore >= 2 && fuel >= 1){
        ret->isMade = 1;
        ore -=2; fuel -= 1;
        ret->type = 1;
    }else if(room->rOrder == 2 && ore >= 1 && fuel >= 2){
        ret->isMade = 1;
        ore -= 1; fuel -= 2;
        ret->type = 2;
    }else if(room->rOrder == 3 && ore >= 0 && fuel >= 3){
        ret->isMade = 1;
        ore -= 0; fuel -= 3;
        ret->type = 3;
    }else{//���� ���� ��� ���� x
        ret->isMade = 0;
    }
    ret->roomId = go->roomId;
    ret->userId = go->userId;
    ret->ore = ore;
    ret->fuel = fuel;

    OutputMemoryStream outputStream;
    Write(&outputStream,(uint8_t*)ret,PacketType::FACTORY_OUTPUT);
    sendRoom(go->roomId,outputStream.GetBufferPtr(),outputStream.GetLength());
    delete ret;
}

void RoomManager::sendRoom(std::string& roomName, const char* msg , uint32_t pLen){
    RoomInfo * room = party[roomName];
    
    for(auto user : room->participate){
        SendMsg(user.second,msg,pLen);
    }
}


bool RoomManager::SendMsg(ClientInfo * pClient,const char* pMsg, uint32_t pLen){
    DWORD recvNumBytes = 0;

    // Overlapped I/O?? ???? ????
    pClient->m_overlapped.m_wsaBuf.len = pLen;
    pClient->m_overlapped.m_wsaBuf.buf = (char*)pMsg;
    pClient->m_overlapped.m_ioType = IO_TYPE::IO_SEND;
    // OutputMemoryStream stream;

    int ret = WSASend(pClient->m_socketClient, &pClient->m_overlapped.m_wsaBuf,
        1, &recvNumBytes, 0, (LPWSAOVERLAPPED)&pClient->m_overlapped, nullptr);

    if (ret == SOCKET_ERROR && ::WSAGetLastError() != WSA_IO_PENDING)
    {
        return false;
    }

    return true;
}



#endif