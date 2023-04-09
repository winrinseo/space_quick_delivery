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

/*����� client�� list*/
vector<SOCKET_INFO> client_list;

/*server socket*/
SOCKET_INFO server_sck;

int client_count = 0;

/*��� ���*/
void server_init();
void add_client();
void send_msg(const char * msg);
void recv_msg(int idx);
void del_client(int idx);

int main(){
    //������ ���� ����
    WSADATA wsa;
    
    /*
    winsock�� �ʱ�ȭ �� �� wsa����ü�� winsocket ������ ��´�.
    winsock�� ����� �غ� �Ǿ��ִٸ� 0�� ��ȯ�Ѵ�.
    */
    int code = WSAStartup(MAKEWORD(2,2),&wsa);

    if(!code){
        server_init();

        char buf[MAX_SIZE] = {};

        string msg;

        /*���������� client�� �߰� �� �����带 �����Ѵ�.*/
        thread th1(add_client);


        /*������ ���� �޼����� �Է¹޴´�.*/
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
    /*���� ���� �Ҵ�.   ip��ü/ AF_INET�� ��� IP ü�踦 ����ϰڴ� �ǹ� , ��Ʈ�� ���� , tcp ���*/
    server_sck.sck = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    /*�ּ� ü�踦 �����ϱ� ���� ����ü.*/
    SOCKADDR_IN server_addr = {};

    /*ip ü�� ���*/
    server_addr.sin_family = AF_INET;
    /*
    port number, host to network short (ȣ��Ʈ���� ��Ʈ��ũ�� �����ֱ� ���� ����Ʈ ������ �������ִ� �Լ�)
    �Ϲ������� ��Ʈ��ũ���� ����Ʈ ������ �ݴ��� big endian����� ������.
    */
    server_addr.sin_port = htons(7777);
    /*INADDR_ANY�� ���� ������ ��ī���� ��� ������ ip �ּҸ� �Ҵ��Ű�� ��ũ��*/
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

        string msg = new_client.user + " ����";
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