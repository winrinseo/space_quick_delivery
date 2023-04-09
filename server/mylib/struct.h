#ifndef STRUCT_H
#define STRUCT_H
#include <mutex>
#include <WinSock2.h>
#include <mswsock.h>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <crtdefs.h>
#include <process.h>
#include <unordered_map>
#include <list>


#define LL long long
#define MAX_SOCKBUF 1024 // 패킷(현재는 버퍼)크기
#define MAX_CLIENT 100 // 최대 접속가능한 클라이언트 수
#define MAX_WORKERTHREAD 12 // 쓰레드 풀(CP객체)에 넣을 쓰레드 수
#define PORT_NUM 5555
using namespace std;

enum class IO_TYPE
{
	ACCEPT,
	IO_RECV,
	IO_SEND
};


// Overlapped 구조체를 확장하여 사용.
struct OverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;
	WSABUF		  m_wsaBuf;
	char		  m_dataBuffer[MAX_SOCKBUF];
	IO_TYPE		  m_ioType;
};

// 클라이언트(Session) 정보를 담기위한 구조체
struct ClientInfo
{
	ClientInfo() : m_socketClient(INVALID_SOCKET)
	{
		ZeroMemory(&m_overlapped, sizeof(OverlappedEx));
	}

	SOCKET			m_socketClient; 
	OverlappedEx	m_overlapped;

	//여기서 부터 클라이언트 방 참여 정보
	LL ID; //접속 시 부여된 ID

	int32_t userId; //방에 들어갔을 때 부여되는 id, 방에 속해있지 않을 땐 -1
	string roomName;
	string userName;
};

#endif