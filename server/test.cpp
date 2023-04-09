
#include "mylib/IocpServer.cpp"


int main(){
    Iocp iocp;
    iocp.InitSocket();
    iocp.BindandListen();
    iocp.StartServer();
    
    while(1){}
}
