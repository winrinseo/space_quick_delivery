#ifndef GAME__OBJECT__H
#define GAME__OBJECT__H

#include "struct.h"
#include "message.h"
//module code
enum class MODULE_CODE
{
	EMPTY,
	PRIM,
	RASER
};

//보스의 스킬 목록
enum class BOSS_SKILL{

};

struct Mob{
	int32_t hp;
};

struct BossMob : public Mob{
	int useSkill;
	BOSS_SKILL skill;

	void usingSkill();
};

//방의 상태 정보를 담은 구조체
struct RoomInfo{
	int stage;
	int mobcnt;
	int userCnt;
	int userNum;
	int32_t resource[2];
	int32_t rOrder;
	std::unordered_map<int32_t,ClientInfo*> participate;
	std::unordered_map<int32_t,UserPosition> userPosition;
    //우주선 상태 중앙이 (10 ,10)
	MODULE_CODE field[21][21];

	RoomInfo();
};

class RoomManager{
private:
	//roomId
	unordered_map<string,RoomInfo *> party;

public:
	RoomManager();
	void createRoom(std::string& roomName,std::string& userName,ClientInfo * pClient);
	bool participateRoom(std::string& roomName,std::string& userName,ClientInfo * pClient);
	bool outRoom(ClientInfo* pClient);
	void moveUser(Move* go);
	void startGame(string* roomName);

	void createModule(CreateModule * go);
	void interactionModule(ModuleInteraction * go);
	void repairModule(ModuleRepair * go);
	void upgradeModule(ModuleUpgrade * go);

	void basicTurret(BasicTurret * go);

	void factoryInput(FactoryInput * go);

	//해당 방에 접속해 있는 유저에게 메세지 전송
	void sendRoom(std::string& roomName , const char * msg, uint32_t pLen);
	bool SendMsg(ClientInfo * pClient,const char* pMsg, uint32_t pLen);
	
	bool existRoom(std::string roomName) { return party.find(roomName) != party.end();}
	RoomInfo* getRoom(std::string roomName) { return party[roomName]; }
};

#endif