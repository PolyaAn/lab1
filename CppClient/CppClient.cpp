// CppClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include <mutex>
#include "CppClient.h"
#include "../MsgServer/Msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::mutex MLock;

void ProcessMessages()
{
    while (true)
    {
        Message m = Message::Send(M_BROKER, M_GETDATA);
        switch (m.m_Header.m_Type)
        {
        case M_DATA:
        {
            MLock.lock();
            cout << m.m_Data << endl;
            MLock.unlock();
            break;
        }
        case M_EXIT:
        {
            cout << "Вы были отключены";
            Message::m_ClientID = 0;
            break;
        }
        default:
        {
            Sleep(100);
            break;
        }
        }
    }
}

// The one and only application object
void Client()
{
    setlocale(LC_ALL, "Russian");
    AfxSocketInit();
    thread t(ProcessMessages);
    t.detach();
    Message m = Message::Send(M_BROKER, M_INIT);
    cout << "Ваш id " << m.m_Header.m_To << endl;

    while (true)
    {
        cout << "1. Отправить сообщение пользователю\n" << "2. Отправить всем\n" << "3. Выход\n";
        int n;
        cin >> n;
        switch (n) {
        case 1: {
            MLock.lock();
            cout << "Введите id\n";
            int id;
            cin >> id;
            cout << "Введите текст\n";
            string s;
            cin.ignore(256, '\n');
            getline(cin, s, '\n');
            MLock.unlock();
            Message::Send(id, M_DATA, s);
            break;
        }
        case 2: {
            MLock.lock();
            cout << "Введите текст\n";
            string s;
            cin.ignore(256, '\n');
            getline(cin, s, '\n');
            MLock.unlock();
            Message::Send(M_ALL, M_DATA, s);
            break;
        }
        case 3: {
            Message::Send(M_BROKER, M_EXIT);
            exit(0);
        }
        default: {
        }
        }
    }

    Message::Send(M_BROKER, M_EXIT);
}

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            Client();
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
