#ifndef MESSAGE__H
#define MESSAGE__H

//client - server간 통신할 메세지 포맷
#include "stream.h"
#include <unordered_map>
#include <vector>
#define OffsetOf(c,mv) ((size_t)& static_cast<c*>(nullptr)->mv)

/*-직렬화 일반화 START-*/
/*직렬화 일반화를 위한 열거자*/
enum EPrimitiveType{
    EPT_Int,
    EPT_UInt8,
    EPT_UInt32,
    EPT_Float,
    EPT_String
};

class MemberVariable{
public:
    MemberVariable(const char* inName, EPrimitiveType inPrimitiveType, uint32_t inOffset) : 
        mName(inName) , mPrimitiveType(inPrimitiveType) , mOffset(inOffset) {}

    EPrimitiveType getPrimitiveType() const {return mPrimitiveType;}

    uint32_t getOffset() const {return mOffset;}

private:
    std::string mName;
    EPrimitiveType mPrimitiveType;
    uint32_t mOffset;
};

class DataType{
public:
    DataType(std::vector<MemberVariable> inMVs) : mMemberVariables(inMVs){
        // std::copy(mMemberVariables.begin(), mMemberVariables.end(), inMVs);
    }

    const std::vector<MemberVariable>& GetMemberVariables() const {return mMemberVariables;}
private:
    std::vector<MemberVariable> mMemberVariables;
};

/*-NOMALIZE END-*/


/*MESSAGE START*/
enum class PacketType{
    NONE,
    HELLO,
    BYE,
    CREATE_ROOM,
    PARTICIPATE_USER,
    OUT_USER,
    PARTICIPATE_ROOM,
    MOVE,
    START_GAME,

    REPLICATION,

    OBJECT_MOVE,
    OBJECT_CONTROL,
    MODULE_CONTROL,
    MODULE_STATUS,
    CURRENT_POSITION,
    ENEMY_MOVE,
    TURRET_STATUS,
    BASIC_TURRET,
    
    MODULE_CREATE,
    MODULE_INTERACTION,
    MODULE_REPAIR,
    MODULE_UPGRADE,


    RESOURCE_CREATE,
    RESOURCE_MOVE,

    FACTORY_INPUT,
    FACTORY_OUTPUT,
};

//방 생성 및 참여에 사용될 메세지 타입
class CreateRoom{
public:
    std::string name;
    std::string roomId;
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("name" , EPT_String , OffsetOf(CreateRoom,name)),
            MemberVariable("roomId" , EPT_String , OffsetOf(CreateRoom,roomId)),
        });
    }
};

class Move{
public:
    // uint32_t packetType;
    std::string roomId;
    int32_t userNum;
    float x,y,z,xx,yy,zz,w;//좌표와 회전 각도
    
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(Move,roomId)),
            MemberVariable("userNum" , EPT_Int , OffsetOf(Move,userNum)),
            MemberVariable("x" , EPT_Float , OffsetOf(Move,x)),
            MemberVariable("y" , EPT_Float , OffsetOf(Move,y)),
            MemberVariable("z" , EPT_Float , OffsetOf(Move,z)),
            MemberVariable("xx" , EPT_Float , OffsetOf(Move,xx)),
            MemberVariable("yy" , EPT_Float , OffsetOf(Move,yy)),
            MemberVariable("zz" , EPT_Float , OffsetOf(Move,zz)),
            MemberVariable("w" , EPT_Float , OffsetOf(Move,w)),
        });
    }
};

class UserPosition{
public:
    std::string roomId;
    int32_t userId;
    float x,y,z,r;//좌표와 회전 각도
    std::string userName;
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(UserPosition,roomId)),
            MemberVariable("userId" , EPT_Int , OffsetOf(UserPosition,userId)),
            MemberVariable("x" , EPT_Float , OffsetOf(UserPosition,x)),
            MemberVariable("y" , EPT_Float , OffsetOf(UserPosition,y)),
            MemberVariable("z" , EPT_Float , OffsetOf(UserPosition,z)),
            MemberVariable("r" , EPT_Float , OffsetOf(UserPosition,r)),
            MemberVariable("userName" , EPT_String , OffsetOf(UserPosition,userName))
        });
    }
};

class CreateModule{
public:
    std::string roomId;
    int code;
    int userId;
    int x,y;
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(CreateModule,roomId)),
            MemberVariable("userId" , EPT_Int , OffsetOf(CreateModule,userId)),
            MemberVariable("code" , EPT_Int , OffsetOf(CreateModule,code)),
            MemberVariable("x" , EPT_Int , OffsetOf(CreateModule,x)),
            MemberVariable("y" , EPT_Int , OffsetOf(CreateModule,y)),
        });
    }
};

class ModuleInteraction{
public:
    std::string roomId;
    int userId;
    int activeNum;

    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(ModuleInteraction,roomId)),
            MemberVariable("userId" , EPT_Int , OffsetOf(ModuleInteraction,userId)),
            MemberVariable("activeNum" , EPT_Int , OffsetOf(ModuleInteraction,activeNum)),
        });
    }
};

class ModuleRepair{
public:
    std::string roomId;
    int userId;
    int moduletype;
    int x,z;

    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(ModuleRepair,roomId)),
            MemberVariable("userId" , EPT_String , OffsetOf(ModuleRepair,userId)),
            MemberVariable("moduletype" , EPT_Int , OffsetOf(ModuleRepair,moduletype)),
            MemberVariable("x" , EPT_Int , OffsetOf(ModuleRepair,x)),
            MemberVariable("z" , EPT_Int , OffsetOf(ModuleRepair,z)),
        });
    }
};

class ModuleUpgrade{
public:
    std::string roomId;
    int userId;
    int moduletype;
    int x,z;

    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(ModuleUpgrade,roomId)),
            MemberVariable("userId" , EPT_String , OffsetOf(ModuleUpgrade,userId)),
            MemberVariable("moduletype" , EPT_Int , OffsetOf(ModuleUpgrade,moduletype)),
            MemberVariable("x" , EPT_Int , OffsetOf(ModuleUpgrade,x)),
            MemberVariable("z" , EPT_Int , OffsetOf(ModuleUpgrade,z)),
        });
    }
};

class BasicTurret{
public:
    // uint32_t packetType;
    std::string roomId;
    int32_t userNum;
    float x,y,z,w,xx,yy,zz,ww;//좌표와 회전 각도
    
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(BasicTurret,roomId)),
            MemberVariable("userNum" , EPT_Int , OffsetOf(BasicTurret,userNum)),
            MemberVariable("x" , EPT_Float , OffsetOf(BasicTurret,x)),
            MemberVariable("y" , EPT_Float , OffsetOf(BasicTurret,y)),
            MemberVariable("z" , EPT_Float , OffsetOf(BasicTurret,z)),
            MemberVariable("w" , EPT_Float , OffsetOf(BasicTurret,w)),
            MemberVariable("xx" , EPT_Float , OffsetOf(BasicTurret,xx)),
            MemberVariable("yy" , EPT_Float , OffsetOf(BasicTurret,yy)),
            MemberVariable("zz" , EPT_Float , OffsetOf(BasicTurret,zz)),
            MemberVariable("ww" , EPT_Float , OffsetOf(BasicTurret,ww)),
        });
    }
};

class FactoryInput{
public:
    std::string roomId;
    int userId;
    int resourceType;
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(FactoryInput,roomId)),
            MemberVariable("userId" , EPT_Int , OffsetOf(FactoryInput,userId)),
            MemberVariable("resourceType" , EPT_Int , OffsetOf(FactoryInput,resourceType)),
        });
    }
};

class FactoryOutput{
public:
    std::string roomId;
    int userId;
    int ore;
    int fuel;
    uint8_t isMade;
    int type;
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_String , OffsetOf(FactoryOutput,roomId)),
            MemberVariable("userId" , EPT_Int , OffsetOf(FactoryOutput,userId)),
            MemberVariable("ore" , EPT_Int , OffsetOf(FactoryOutput,ore)),
            MemberVariable("fuel" , EPT_Int , OffsetOf(FactoryOutput,fuel)),
            MemberVariable("isMade" , EPT_UInt8 , OffsetOf(FactoryOutput,isMade)),
            MemberVariable("type" , EPT_Int , OffsetOf(FactoryOutput,type)),
        });
    }
};
 
// Object 생성 / 움직임
class ObjectMove { 
public:
    std::string roomId;
    int32_t objectNum;
    float x,y,z,r;
    
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_UInt32 , OffsetOf(ObjectMove,roomId)),
            MemberVariable("objectNum" , EPT_Int , OffsetOf(ObjectMove,objectNum)),
            MemberVariable("x" , EPT_Float , OffsetOf(ObjectMove,x)),
            MemberVariable("y" , EPT_Float , OffsetOf(ObjectMove,y)),
            MemberVariable("z" , EPT_Float , OffsetOf(ObjectMove,z)),
            MemberVariable("r" , EPT_Float , OffsetOf(ObjectMove,r))
        });
    }
};

class ObjectControl {
public:
    std::string roomId;
    int32_t userNum; // 누가( 오브젝트를 들었을 때 위치는 항상 캐릭터에서 고정된 위치 )
    int32_t objectNum; // 어떤 자원을
    int32_t activeNum; // 들고 있는가 / 내렸는가

    static DataType* sDataType;
    static void InitDataType() {
        sDataType = new DataType({
            MemberVariable("roomId", EPT_UInt32, OffsetOf(ObjectControl, roomId)),
            MemberVariable("userNum", EPT_Int, OffsetOf(ObjectControl, userNum)),
            MemberVariable("objectNum", EPT_Int, OffsetOf(ObjectControl, objectNum)),
            MemberVariable("activeNum", EPT_Int, OffsetOf(ObjectControl, activeNum))
        });
    };  
};

// 어떤 유저가 어떤 모듈에 어떤 작업을 했는지
class ModuleControl { 
public:
    std::string roomId;
    int32_t userNum;
    int32_t moduleNum;
    int32_t activeNum;

    static DataType* sDataType;
    static void InitDataType() {
        sDataType = new DataType({
            MemberVariable("roomId", EPT_UInt32, OffsetOf(ModuleControl, roomId)), 
            MemberVariable("userNum", EPT_Int, OffsetOf(ModuleControl, userNum)), 
            MemberVariable("moduleNum", EPT_Int, OffsetOf(ModuleControl, moduleNum)),
            MemberVariable("activeNum", EPT_Int, OffsetOf(ModuleControl, activeNum))
        });
    };  
};

// 모듈 상태(체력게이지, 산소게이지, 엔진 연료, 파괴) 
class ModuleStatus { 
public:
    std::string roomId;
    int32_t moduleNum;
    int32_t modulelHp;
    int32_t fuel;

    static DataType* sDataType;
    static void InitDataType() {
        sDataType = new DataType({
            MemberVariable("roomId", EPT_UInt32, OffsetOf(ModuleStatus, roomId)), 
            MemberVariable("moduleNum", EPT_Int, OffsetOf(ModuleStatus, moduleNum)),
            MemberVariable("modulelHp", EPT_Int, OffsetOf(ModuleStatus, modulelHp)),
            MemberVariable("fuel", EPT_Int, OffsetOf(ModuleStatus, fuel))
        });
    };  
};

// 얼마나 왔는지
class CurrentPosition { 
public:
    std::string roomId;
    int32_t currentPoint;

    static DataType* sDataType;
    static void InitDataType() {
        sDataType = new DataType({
            MemberVariable("roomId", EPT_UInt32, OffsetOf(CurrentPosition, roomId)), 
            MemberVariable("currentPoint", EPT_Int, OffsetOf(CurrentPosition, currentPoint))
        });
    };  
};


// 적 생성, 이동, 상태(체력) - 생성만 해 주면 화면 밖에서 특정 타겟 모듈을 향해서 알아서 이동 후 공격.
// 이에 맞춰서 타워도 가장 가까운 적을 향해서 조준 후 사격
// 하지만 생성-파괴(체력0이하)는 호스트 쪽의 데이터대로 이루어져야 한다.
class EnemyMove{
public:
    std::string roomId;
    int32_t enemyNum;
    int32_t enemyHp;
    float x,y,z,r; //좌표와 회전 각도
    
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_UInt32 , OffsetOf(EnemyMove,roomId)),
            MemberVariable("enemyNum" , EPT_Int , OffsetOf(EnemyMove,enemyNum)),
            MemberVariable("enemyHp" , EPT_Int , OffsetOf(EnemyMove,enemyHp)),
            MemberVariable("x" , EPT_Float , OffsetOf(EnemyMove,x)),
            MemberVariable("y" , EPT_Float , OffsetOf(EnemyMove,y)),
            MemberVariable("z" , EPT_Float , OffsetOf(EnemyMove,z)),
            MemberVariable("r" , EPT_Float , OffsetOf(EnemyMove,r))
        });
    }
};

/*
// 적이 공격
class EnemyAttack{
public:
    std::string roomId;
    int32_t enemyNum;
    float x,y,z;// 타겟은 모듈.
    
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_UInt32 , OffsetOf(EnemyAttack,roomId)),
            MemberVariable("enemyNum" , EPT_Int , OffsetOf(EnemyAttack,enemyNum)),
            MemberVariable("x" , EPT_Float , OffsetOf(EnemyAttack,x)),
            MemberVariable("y" , EPT_Float , OffsetOf(EnemyAttack,y)),
            MemberVariable("z" , EPT_Float , OffsetOf(EnemyAttack,z))
        });
    }
};
*/

// 포탑 생성, 조준, 상태(체력),
// 공격도 해야하나(이펙트)???????????? -> 적 위치를 주면 알아서 가장 근처의 적을 찾아서 알아서 쏜다.
class TurretStatus{
public:
    std::string roomId;
    int32_t turretNum;
    int32_t moduleHp;
    
    static DataType* sDataType;
    static void InitDataType(){
        sDataType = new DataType({
            MemberVariable("roomId" , EPT_UInt32 , OffsetOf(TurretStatus,roomId)),
            MemberVariable("turretNum" , EPT_Int , OffsetOf(TurretStatus,turretNum)),
            MemberVariable("moduleHp" , EPT_Int , OffsetOf(TurretStatus,moduleHp))
        });
    }
};

// // 기본 포탑
// class BasicTurret{
// public:
//     std::string roomId;
//     int32_t moduleHp;
//     float r; // 머리 돌리기
    
//     static DataType* sDataType;
//     static void InitDataType(){
//         sDataType = new DataType({
//             MemberVariable("roomId" , EPT_UInt32 , OffsetOf(BasicTurret,roomId)),
//             MemberVariable("moduleHp" , EPT_Int , OffsetOf(BasicTurret,moduleHp)),
//             MemberVariable("r" , EPT_Int , OffsetOf(BasicTurret,r))
//         });
//     }
// };

//참여 성공 시 방 전체 유저에게 반환할 메세지 타입
class ParticipateRoom{
public:
    uint8_t isSuccess;
    int32_t userCnt;
    std::unordered_map<int32_t,UserPosition> userPosition;
};

/*MESSAGE END*/

/*SERIALIZE START*/

//POD객체 하나를 직렬화하는 일반화 함수
void Write(OutputMemoryStream* inMemoryStream , const DataType * inDataType, uint8_t* inData){
    for(auto& mv : inDataType->GetMemberVariables()){
        //저장해둔 변수의 오프셋을 더해 멤버 변수의 실제 위치를 얻는다.
        void* mvData = inData + mv.getOffset();

        switch(mv.getPrimitiveType()){
            case EPT_String:
                inMemoryStream->Write(*(std::string*) mvData);
                break;
            case EPT_UInt32:
                inMemoryStream->Write(*(uint32_t*) mvData);
                break;
            case EPT_Int:
                inMemoryStream->Write(*(int32_t*) mvData);
                break;
            case EPT_Float:
                inMemoryStream->Write(*(float*) mvData);
                break;
            default:
                break;
        }
    }
}

//직렬화 초입 (객체를 여러번 기록해야 하는 경우를 대비한 인터페이스)
void Write(OutputMemoryStream* inMemoryStream,uint8_t* inData,PacketType pt){
    //패킷 타입 먼저 입력
    inMemoryStream->Write((uint8_t)pt);
    //패킷 타입에 따라 다른 직렬화 동작 적용
    switch(pt){
        case PacketType::CREATE_ROOM: //문자열 2개 직렬화
            {
                Write(inMemoryStream,CreateRoom::sDataType,inData);
            }
            break;
        case PacketType::PARTICIPATE_USER:
            {
                Write(inMemoryStream,CreateRoom::sDataType,inData);
            }
            break;
        case PacketType::PARTICIPATE_ROOM: //성공여부와 유저의 수 , 유저들의 위치 직렬화
            {
                ParticipateRoom* room = (ParticipateRoom*)inData;
                inMemoryStream->Write((uint8_t)room->isSuccess);
                inMemoryStream->Write((int32_t)room->userCnt);

                for(int i = 0;i<(int)room->userCnt;i++){
                    Write(inMemoryStream,UserPosition::sDataType,(uint8_t*)&room->userPosition[i]);
                }
            }
            break;
        case PacketType::MOVE: //Move객체 직렬화
            Write(inMemoryStream,Move::sDataType,inData);
            break;
        case PacketType::REPLICATION: //객체 개수 , (네트워크 식별자, 클래스 식별자, 객체) 개수만큼 반복해 직렬화 
            break;
        case PacketType::START_GAME:
            {
                std::string * s = (std::string*) inData;
                inMemoryStream->Write(*s);
            }
            break;
        case PacketType::MODULE_CREATE:
            {
                Write(inMemoryStream,CreateModule::sDataType,inData);
            }
            break;
        case PacketType::MODULE_INTERACTION:
            {
                Write(inMemoryStream,ModuleInteraction::sDataType,inData);
            }
            break;
        
        case PacketType::MODULE_REPAIR:
            {
                Write(inMemoryStream,ModuleRepair::sDataType,inData);
            }
            break;
        
        case PacketType::MODULE_UPGRADE:
            {
                Write(inMemoryStream,ModuleUpgrade::sDataType,inData);
            }
            break;
        case PacketType::FACTORY_OUTPUT:
            {
                Write(inMemoryStream,FactoryOutput::sDataType,inData);
            }
            break;
        default:
            break;
    }
}



void Read(InputMemoryStream* inMemoryStream , const DataType * inDataType, uint8_t* inData){
    
    for(auto& mv : inDataType->GetMemberVariables()){
        //저장해둔 변수의 오프셋을 더해 멤버 변수의 실제 위치를 얻는다.
        void* mvData = inData + mv.getOffset();
        switch(mv.getPrimitiveType()){
            case EPT_String:
                inMemoryStream->Read(*(std::string*) mvData);
                break;
            case EPT_UInt32:
                inMemoryStream->Read(*(uint32_t*) mvData);
                break;
            case EPT_Int:
                inMemoryStream->Read(*(int32_t*) mvData);
                break;
            case EPT_Float:
                inMemoryStream->Read(*(float*) mvData);
                break;
            default:
                break;
        }
    }
}

void* Read(InputMemoryStream* inMemoryStream,PacketType& pt){
    //패킷 타입 먼저 읽음
    uint8_t prevPt;
    inMemoryStream->Read(prevPt);
    pt = (PacketType)prevPt;
    void* result = nullptr;
    switch(pt){
        case PacketType::CREATE_ROOM:
            {
                //오브젝트 풀로 변경 예정
                CreateRoom* ret = new CreateRoom;
                Read(inMemoryStream,CreateRoom::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
        case PacketType::PARTICIPATE_USER:
            {
                //오브젝트 풀로 변경 예정
                CreateRoom* ret = new CreateRoom;
                Read(inMemoryStream,CreateRoom::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
        case PacketType::MOVE:
            {
                //오브젝트 풀로 변경 예정
                Move* ret = new Move;
                Read(inMemoryStream,Move::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
            
        case PacketType::PARTICIPATE_ROOM:
            {
                ParticipateRoom* ret = new ParticipateRoom;
                inMemoryStream->Read(ret->isSuccess);
                inMemoryStream->Read(ret->userCnt);
                for(int i = 0;i<ret->userCnt;i++){
                    UserPosition m;
                    Read(inMemoryStream,UserPosition::sDataType,(uint8_t*)&m);
                    ret->userPosition[m.userId] = m;
                }
                result = ret;
            }
            break;
        
        case PacketType::REPLICATION:
            break;

        case PacketType::START_GAME:
            {
                std::string* ret = new std::string;
                inMemoryStream->Read(*ret);
                result = ret;
            }
            break;

        case PacketType::MODULE_CREATE:
            {
                CreateModule * ret = new CreateModule;
                Read(inMemoryStream,CreateModule::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
        case PacketType::MODULE_INTERACTION:
            {
                ModuleInteraction * ret = new ModuleInteraction;
                Read(inMemoryStream,ModuleInteraction::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
        
        case PacketType::MODULE_REPAIR:
            {
                ModuleRepair * ret = new ModuleRepair;
                Read(inMemoryStream,ModuleRepair::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;

        case PacketType::MODULE_UPGRADE:
            {
                ModuleUpgrade * ret = new ModuleUpgrade;
                Read(inMemoryStream,ModuleUpgrade::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
        case PacketType::BASIC_TURRET:
            {
                BasicTurret * ret = new BasicTurret;
                Read(inMemoryStream,BasicTurret::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
        case PacketType::FACTORY_INPUT:
            {
                FactoryInput * ret = new FactoryInput;
                Read(inMemoryStream,FactoryInput::sDataType,(uint8_t*)ret);
                result = ret;
            }
            break;
        default:
            break;
    }
    return result;
}
/*SERIALIZE END*/
#endif