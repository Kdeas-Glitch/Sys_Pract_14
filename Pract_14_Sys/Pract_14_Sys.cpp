#include <iostream>
#include <windows.h>


#define MAX_CLIENTS 20
#define CLUB_CAPACITY 4

struct ClientRecord {
    DWORD threadId;
    DWORD arriveTick;
    DWORD startTick;
    DWORD endTick;
    BOOL served;
    BOOL timeout;
};

struct ClubState {
    ClientRecord clients[MAX_CLIENTS];
    LONG currentVisitors;
    LONG maxVisitors;
    LONG servedCount;
    LONG timeoutCount;
};
ClubState clubst;

DWORD WINAPI SuperLooker() {
    Sleep(500);
    while (true) {
        std::cout<<"Количество занятых мест: "<<clubst.currentVisitors << std::endl;
        std::cout<<"Количество Обслуженных мест: " << clubst.servedCount << std::endl;
        std::cout << "Количество поситителей, ушедших по таймауту: " << clubst.timeoutCount << std::endl;
    }
}

int main()
{
    STARTUPINFO si;
    HANDLE hThreadLooker;
    DWORD IDThreadLooker;

    hThreadLooker = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SuperLooker, NULL, HANDLE_FLAG_INHERIT, &IDThreadLooker);
    if (hThreadLooker == NULL) {
        return GetLastError();
    }


}