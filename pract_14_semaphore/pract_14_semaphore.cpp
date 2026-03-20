#include <iostream>
#include <windows.h>
#include <random>


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
    LONG currentVisitors = 0;
    LONG maxVisitors = 0;
    LONG servedCount = 0;
    LONG timeoutCount = 0;
};
ClubState clubst;
int currentclients = 0;
bool all;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(2000, 5000);
HANDLE CountSemaphore;
DWORD WINAPI SuperLooker() {
    while (!all) {
        Sleep(500);
        std::cout << "Количество занятых мест: " << clubst.currentVisitors << std::endl;
        std::cout << "Количество Обслуженных мест: " << clubst.servedCount << std::endl;
        std::cout << "Количество поситителей, ушедших по таймауту: " << clubst.timeoutCount << std::endl;
    }
    Sleep(100);
    std::cout << "Количество занятых мест: " << clubst.currentVisitors << std::endl;
    std::cout << "Количество Обслуженных мест: " << clubst.servedCount << std::endl;
    std::cout << "Количество поситителей, ушедших по таймауту: " << clubst.timeoutCount << std::endl;
    std::cout << "Максимальное количество посителей " << clubst.maxVisitors << std::endl;
    return 0;
}

DWORD WINAPI GiperVisitor(LPVOID countes) {
    Sleep(100);
    int count = (int)countes;
    int currentick = GetTickCount64();
    clubst.clients[count].arriveTick = GetTickCount64();
    while (true) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
        DWORD res =WaitForSingleObject(CountSemaphore,0);

        if (res!=WAIT_TIMEOUT) {
            clubst.currentVisitors++;
            if (clubst.maxVisitors < clubst.currentVisitors)
                clubst.maxVisitors = clubst.currentVisitors;
            clubst.clients[count].startTick = GetTickCount64();
            int a = dis(gen);
            Sleep(a);
            clubst.currentVisitors= clubst.currentVisitors-1;
            clubst.clients[count].endTick = GetTickCount64();
            clubst.clients[count].served = true;
            clubst.servedCount++;
            ReleaseSemaphore(CountSemaphore,1,NULL);
            break;
        }
        currentick = GetTickCount64();
        if (currentick - clubst.clients[count].arriveTick >= 3000) {
            clubst.timeoutCount++;
            clubst.clients[count].endTick = GetTickCount64();
            clubst.clients[count].timeout = true;
            break;
        }
    }
    return 0;
}

int main()
{
    
    CountSemaphore = CreateSemaphore(NULL, CLUB_CAPACITY, CLUB_CAPACITY, NULL);
    if (CountSemaphore == NULL) {
        return GetLastError();
    }
    setlocale(0, "rus");
    HANDLE hThreadLooker;
    DWORD IDThreadLooker;
    HANDLE hThreadVisitor[MAX_CLIENTS];
    DWORD IDThreadVisitor[MAX_CLIENTS];


    hThreadLooker = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SuperLooker, NULL, HANDLE_FLAG_INHERIT, &IDThreadLooker);
    if (hThreadLooker == NULL) {
        return GetLastError();
    }
    SetPriorityClass(hThreadLooker, THREAD_PRIORITY_LOWEST);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        hThreadVisitor[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GiperVisitor, (void*)i, HANDLE_FLAG_INHERIT, &IDThreadVisitor[i]);
        if (hThreadVisitor[i] == NULL) {
            return GetLastError();
        }
        if (i < 7) {
            SetPriorityClass(hThreadVisitor[i], THREAD_PRIORITY_NORMAL);
        }
        else if (i < 16) {
            SetPriorityClass(hThreadVisitor[i], THREAD_PRIORITY_BELOW_NORMAL);
        }
        else {
            SetPriorityClass(hThreadVisitor[i], THREAD_PRIORITY_HIGHEST);
        }
        clubst.clients[i].threadId = IDThreadVisitor[i];
    }


    WaitForMultipleObjects(MAX_CLIENTS, hThreadVisitor, true, INFINITE);
    all = true;
    WaitForSingleObject(hThreadLooker, INFINITE);
    CloseHandle(hThreadLooker);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        CloseHandle(hThreadVisitor[i]);
    }






}