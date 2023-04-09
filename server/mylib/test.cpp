#include <string>
#include "message.h"
using namespace std;
DataType* Move::sDataType = nullptr; 
DataType* CreateRoom::sDataType = nullptr; 
int main(){
    OutputMemoryStream stream;
    
    // Move::InitDataType();
    // Move a;
    // a.roomId = 10;
    // a.userNum = 1;
    // a.x = 2;
    // a.y = 1;
    // a.z = 4;
    // a.r = 55;
    
    // a.write(stream);

    CreateRoom::InitDataType();
    CreateRoom a;
    cin>>a.name>>a.roomId;
   
    Write(&stream,(uint8_t*)&a,PacketType::CREATE_ROOM);

    for(int i = 0;i<(int)stream.GetLength();i++){
        cout<<(int)*(stream.GetBufferPtr()+i)<<" ";
    }
    CreateRoom b;
    // cout<<stream.GetBufferPtr()<<"\n";
    InputMemoryStream st((char*)stream.GetBufferPtr(),stream.GetLength());
    // Read(&st,CreateRoom::sDataType,(uint8_t*)&b);
    PacketType pt = (PacketType)0;
    void * ret = Read(&st,pt);
    if(pt == PacketType::CREATE_ROOM){
        CreateRoom * c = static_cast<CreateRoom*>(ret);
        
        std::cout<<(c->roomId)<<" "<<(c->name)<<" "<<"\n";
        delete c;
    }
    
    // while(1){}
}