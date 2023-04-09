
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include "mylib/message.h"


#define MAX_SIZE 1024
using namespace std;

SOCKET client_sock;
int chat_recv();
DataType* Move::sDataType = nullptr;
DataType* CreateRoom::sDataType = nullptr;
DataType* UserPosition::sDataType = nullptr;
int main(){
    WSAData wsa;
    Move::InitDataType();
    CreateRoom::InitDataType();
    UserPosition::InitDataType();
    int code = WSAStartup(MAKEWORD(2,2), &wsa);

    string nick;
    if(!code){
        cout<<"????? ?????? : ";
        cin>>nick;

        client_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
        string server = "127.0.0.1";
        SOCKADDR_IN addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5555);
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        while(1){
            if(!connect(client_sock, (SOCKADDR*)&addr,sizeof(addr))){
                cout<<"Server Connect"<<"\n";
                // send(client_sock,nick.c_str(),MAX_SIZE,0);
                break;
            }
            cout<<"Connecting..."<<"\n";
        }

        thread th2(chat_recv);
        

        // char buf[MAX_SIZE] = {};

        while(1){
            int pt;cin>>pt;
            PacketType pk = (PacketType)pt;
            if(pk == PacketType::CREATE_ROOM){
                CreateRoom a;
                cin>>a.name;
                cin>>a.roomId;
                OutputMemoryStream stream;
                Write(&stream,(uint8_t*)&a,PacketType::CREATE_ROOM);
                send(client_sock,stream.GetBufferPtr(),stream.GetLength(),0);
            }else if(pk == PacketType::PARTICIPATE_USER){
                CreateRoom a;
                cin>>a.name;
                cin>>a.roomId;
                OutputMemoryStream stream;
                Write(&stream,(uint8_t*)&a,PacketType::PARTICIPATE_USER);
                send(client_sock,stream.GetBufferPtr(),stream.GetLength(),0);
            }else if(pk == PacketType::MOVE){
                Move a;
                cin>>a.roomId;
                cin>>a.userNum;
                cin>>a.x;
                cin>>a.y;
                cin>>a.z;
                cin>>a.xx;
                cin>>a.yy;
                cin>>a.zz;
                cin>>a.w;
                OutputMemoryStream stream;
                Write(&stream,(uint8_t*)&a,PacketType::MOVE);
                send(client_sock,stream.GetBufferPtr(),stream.GetLength(),0);
            }else if(pk == PacketType::START_GAME){
                string room;
                cin>>room;
                OutputMemoryStream stream;
                Write(&stream,(uint8_t*)&room,PacketType::START_GAME);
                send(client_sock,stream.GetBufferPtr(),stream.GetLength(),0);
            }else if(pk == PacketType::OUT_USER){
                OutputMemoryStream stream;
                Write(&stream,nullptr,PacketType::OUT_USER);
                send(client_sock,stream.GetBufferPtr(),stream.GetLength(),0);
            }
        }
        th2.join();
        closesocket(client_sock);
    }

    WSACleanup();
    return 0;
}

int chat_recv(){
    char buf[MAX_SIZE] = {};
    // string msg;
    while(1){
        ZeroMemory(&buf,MAX_SIZE);
        if(recv(client_sock,buf,MAX_SIZE,0) > 0){
            // msg = string(buf);
            // cout<<buf<<"\n";
            InputMemoryStream stream(buf,static_cast<uint32_t>(MAX_SIZE));
            PacketType pt = PacketType::HELLO;
            void * ret = Read(&stream,pt);
            
            if(pt == PacketType::MOVE){
                Move * c = static_cast<Move*>(ret);
                std::cout<<c->roomId<<" "<<c->userNum<<" "<<c->x<<" "<<c->y<<" "<<c->z<<" "<<c->w<<"\n";
                delete c;
            }else if(pt == PacketType::CREATE_ROOM){
                CreateRoom * c = static_cast<CreateRoom*>(ret);
                std::cout<<(c->roomId)<<" "<<(c->name)<<" "<<"\n";
                delete c;
            }else if(pt == PacketType::PARTICIPATE_ROOM){
                ParticipateRoom * c = static_cast<ParticipateRoom*>(ret);
                std::cout<<"참여자 목록 : "<<c->userCnt<<" "<<c->userPosition.size()<<"\n";
                for(auto& cc : c->userPosition)
                    cout<<cc.second.roomId<<" "<<cc.second.userId<<" "<<cc.second.x<<" "<<cc.second.y<<" "<<cc.second.z<<" "<<cc.second.r<<" "<<cc.second.userName<<"\n";
            }else if(pt == PacketType::START_GAME){
                string * c = static_cast<string*>(ret);
                cout<<*c<<"\n";
            }

        }else{
            cout<<"server OFF"<<"\n";
            return -1;
        }
    }
}