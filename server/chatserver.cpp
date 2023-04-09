#pragma once
#include <WinSock2.h>
#include <thread>
#include <iostream>
#include <string>
#include <vector>

#define MAX_SIZE 1024
#define MAX_CLIENT 6
using namespace std;

typedef struct socket_INFO{
    SOCKET sck;
    string user;
}SOCKET_INFO;

/*연결된 client의 list*/
vector<SOCKET_INFO> client_list;

/*server socket*/
SOCKET_INFO server_sck;

int client_count = 0;

/*기능 목록*/
void server_init();
void add_client();
void send_msg(const char * msg);
void recv_msg(int idx);
void del_client(int idx);

int main(){
    //윈도우 소켓 정보
    WSADATA wsa;
    
    /*
    winsock을 초기화 한 후 wsa구조체에 winsocket 정보를 담는다.
    winsock을 사용할 준비가 되어있다면 0을 반환한다.
    */
    int code = WSAStartup(MAKEWORD(2,2),&wsa);

    if(!code){
        server_init();

        char buf[MAX_SIZE] = {};

        string msg;

        /*병렬적으로 client를 추가 할 쓰레드를 실행한다.*/
        thread th1(add_client);


        /*서버가 보낼 메세지를 입력받는다.*/
        while(1){
            cin>>buf;
            msg = server_sck.user + " : " + string(buf);
            send_msg(msg.c_str());
        }

        th1.join();
        closesocket(server_sck.sck);
    }
    
    WSACleanup();

    return 0;
}


void server_init(){
    /*서버 소켓 할당.   ip자체/ AF_INET인 경우 IP 체계를 사용하겠단 의미 , 스트림 형식 , tcp 통신*/
    server_sck.sck = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    /*주소 체계를 저장하기 위한 구조체.*/
    SOCKADDR_IN server_addr = {};

    /*ip 체계 사용*/
    server_addr.sin_family = AF_INET;
    /*
    port number, host to network short (호스트에서 네트워크로 보내주기 위해 바이트 순서를 수정해주는 함수)
    일반적으로 네트워크에선 바이트 순서가 반대인 big endian방식을 따른다.
    */
    server_addr.sin_port = htons(7777);
    /*INADDR_ANY는 현재 장착된 랜카드중 사용 가능한 ip 주소를 할당시키는 매크로*/
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(server_sck.sck,(sockaddr * ) &server_addr,sizeof(server_addr));
    listen(server_sck.sck,SOMAXCONN);

    server_sck.user = "server";

    cout<<"Server On...\n";
}

void add_client(){
    while(client_count < MAX_CLIENT){
        SOCKADDR addr = {};
        int addrsize = sizeof(addr);
        char buf[MAX_SIZE] = {};

        ZeroMemory(&addr,addrsize);

        SOCKET_INFO new_client = {};
        new_client.sck = accept(server_sck.sck,(sockaddr*) &addr, &addrsize);
        recv(new_client.sck,buf,MAX_SIZE,0);
        new_client.user = string(buf);

        string msg = new_client.user + " 접속";
        cout<<msg<<"\n";
        
        client_list.push_back(new_client);
        thread(recv_msg,client_count).detach();
        client_count++;

        send_msg(msg.c_str());

    }

}

void send_msg(const char * msg){
    for(int i = 0;i<client_count;i++)
        send(client_list[i].sck,msg,MAX_SIZE,0);
}

void recv_msg(int idx){
    char buf[MAX_SIZE] = {};
    string msg;
    cout<<client_list[idx].user<<"\n";

    while(1){
        ZeroMemory(&buf,MAX_SIZE);
        if(recv(client_list[idx].sck,buf,MAX_SIZE,0) > 0){
            msg = client_list[idx].user + " : " + string(buf);
            cout<<msg<<"\n";
            send_msg(msg.c_str());
        }else{
            msg = client_list[idx].user + " is disconnected..." ;
            cout<<msg<<"\n";
            send_msg(msg.c_str());
            del_client(idx);
            return;
        }
    }
}

void del_client(int idx){
    closesocket(client_list[idx].sck);
    client_list.erase(client_list.begin() + idx);
    client_count--;
}