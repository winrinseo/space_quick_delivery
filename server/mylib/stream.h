#ifndef STREAM__H
#define STREAM__H
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm>

class OutputMemoryStream {
private:
    void ReallocBuffer(uint32_t inNewLength);
 
    char* mBuffer;
    uint32_t mHead;
    uint32_t mCapacity;

public:
    OutputMemoryStream() :
        mBuffer(nullptr), mHead(0), mCapacity(0)
    { ReallocBuffer(32); }
    // ~OutputMemoryStream() { std::free(mBuffer); }

    void clear(){
        mBuffer = nullptr; mHead = 0; mCapacity = 0;
        ReallocBuffer(32);
    }
 
    // 스트림의 데이터 시작 위치 포인터를 구함
    const char* GetBufferPtr() const { return mBuffer; }
    uint32_t GetLength() const { return mHead; }
 
    void Write(const void* inData, size_t inByteCount);
    void Write(uint32_t inData) { Write(&inData, sizeof(inData)); }
    void Write(int32_t inData) { Write(&inData, sizeof(inData)); }
    void Write(uint8_t inData) { Write(&inData, sizeof(inData)); }
    void Write(double inData) { Write(&inData, sizeof(inData)); }
    void Write(float inData) { Write(&inData, sizeof(inData)); }

    void Write(std::string inData);
};

void OutputMemoryStream::ReallocBuffer(uint32_t inNewLength)
{
    mBuffer = static_cast<char*>(std::realloc(mBuffer, inNewLength));
    // realloc 호출이 실패한 경우 처리
    // …
    mCapacity = inNewLength;
}
 
void OutputMemoryStream::Write(const void* inData, size_t inByteCount)
{
    // 공간을 일단 충분히 확보
    uint32_t resultHead = mHead + static_cast<uint32_t>(inByteCount);
    if (resultHead > mCapacity)
        ReallocBuffer(std::max(mCapacity * 2, resultHead));
 
    // 버퍼의 제일 앞에 복사
    std::memcpy(mBuffer + mHead, inData, inByteCount);
    // mHead를 전진시켜 다음 기록에 대비
    mHead = resultHead;
}

void OutputMemoryStream::Write(const std::string inData){
    const char* p = inData.c_str();
    uint32_t s = inData.size();

    // 공간을 일단 충분히 확보
    uint32_t resultHead = mHead + static_cast<uint32_t>(s+4);//1바이트 더 올림
    if (resultHead > mCapacity)
        ReallocBuffer(std::max(mCapacity * 2, resultHead));
    
    // 문자열의 크기 기록
    std::memcpy(mBuffer + mHead, &s, 4);
    // 버퍼의 제일 앞에 복사
    std::memcpy(mBuffer + mHead + 4, p, s);
    // mHead를 전진시켜 다음 기록에 대비
    mHead = resultHead;
}

class InputMemoryStream{
private:
    char* mBuffer;
    uint32_t mHead;
    uint32_t mCapacity;
public:
    InputMemoryStream(){
        mBuffer = nullptr;
        mHead = 0;
        mCapacity = 0;
    }
    InputMemoryStream(char* inBuffer,uint32_t inByteCount):mBuffer(inBuffer),mHead(0),mCapacity(inByteCount){}
    // ~InputMemoryStream(){std::free(mBuffer);}

    uint32_t GetRemainingDataSize() const {return mCapacity - mHead;}
    void initMsg(char* inBuffer,uint32_t inByteCount){
        mBuffer = inBuffer;
        mHead = 0;
        mCapacity = inByteCount;
    }

    void Read(void* outData,uint32_t inByteCount);
    void Read(uint32_t& outData){Read(&outData,sizeof(outData));}
    void Read(uint8_t& outData){Read(&outData,sizeof(outData));}
    void Read(int32_t& outData){Read(&outData,sizeof(outData));}
    void Read(double& outData){Read(&outData,sizeof(outData));}
    void Read(float& outData){Read(&outData,sizeof(outData));}

    void Read(std::string& outData);
};

void InputMemoryStream::Read(void* outData,uint32_t inByteCount ){
    std::memcpy(outData,mBuffer+mHead,inByteCount);
    mHead += inByteCount;
}
void InputMemoryStream::Read(std::string& outData){
    uint32_t s;
    //4바이트 먼저 읽어옴
    Read(s);
    outData.resize(s);
    for(int i = 0;i<(int)s;i++)
        outData[i] = *(mBuffer+mHead++);
}

#endif