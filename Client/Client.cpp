// https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <string> 
#include <ws2tcpip.h>
using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define RAND (int)(rand() % 10)
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define PAUSE 1000

int main(int argc, char** argv) // имя сервера при желании можно будет указать через параметры командной строки
{
    // я попытался максимально упростить запуск клиентского приложения, поэтому количество параметров командной строки не проверяется!
    // Validate the parameters
    //if (argc != 2) {
    //    printf("usage: %s server-name\n", argv[0]);
    //    return 10;
    //}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    setlocale(0, "");
    system("title CLIENT SIDE (exit-0)");
    cout << "процесс клиента запущен!\n";
    srand(time(NULL));
    Sleep(PAUSE);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << "\n";
        return 11;
    }
    else {
        cout << "подключение Winsock.dll прошло успешно!\n";
        Sleep(PAUSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    const char* ip = "localhost"; // по умолчанию, оба приложения, и клиент, и сервер, запускаются на одной и той же машине

    //iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result); // раскомментировать, если нужно будет читать имя сервера из командной строки
    addrinfo* result = NULL;
    iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);

    if (iResult != 0) {
        cout << "getaddrinfo failed with error: " << iResult << "\n";
        WSACleanup();
        return 12;
    }
    else {
        cout << "получение адреса и порта клиента прошло успешно!\n";
        Sleep(PAUSE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Attempt to connect to an address until one succeeds
    SOCKET ConnectSocket = INVALID_SOCKET;

    for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) { // серверов может быть несколько, поэтому не помешает цикл

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            cout << "socket failed with error: " << WSAGetLastError() << "\n";
            WSACleanup();
            return 13;
        }

        // Connect to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        cout << "создание сокета на клиенте прошло успешно!\n";
        Sleep(PAUSE);

        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "невозможно подключиться к серверу. убедитесь, что процесс сервера запущен!\n";
        WSACleanup();
        return 14;
    }
    else {
        cout << "подключение к серверу прошло успешно!\n";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    const char* message;
    string tmp;
    char answer[DEFAULT_BUFLEN];
    for (int i = 0; i < 3; i++)
    {
        cout << "Меню:\n 0 - Яйца Пашот\t 1 - Том-Ям\t 2 - Омлет\t 3 - Фо-Бо\t 4 - Роллы\t 5 - Удон\t 6 - Креветки Темпура\t 7 - Цезарь\t 8 - Котлеты\t 9 - Борщ\n>>";
        cout << "Автоматическая отправка серверу: "; tmp = to_string(RAND);
        message = tmp.c_str();
        iResult = send(ConnectSocket, message, (int)strlen(message), 0);
        if (iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return 15;
        }
        else {
            cout << "данные успешно отправлены на сервер: " << message << "\n";
            cout << "байтов с клиента отправлено: " << iResult << "\n";
        }

        iResult = recv(ConnectSocket, answer, DEFAULT_BUFLEN, 0);
        answer[iResult] = '\0';

        if (iResult > 0) {
            cout << "процесс сервера прислал ответное сообщение: " << answer << "\n";
            cout << "байтов получено: " << iResult << "\n";
        }
        else if (iResult == 0)
            cout << "соединение с сервером закрыто.\n";
        else
            cout << "recv failed with error: " << WSAGetLastError() << "\n";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    do {
        cout << "Меню:\n 0 - Яйца Пашот\t 1 - Том-Ям\t 2 - Омлет\t 3 - Фо-Бо\t 4 - Роллы\t 5 - Удон\t 6 - Креветки Темпура\t 7 - Цезарь\t 8 - Котлеты\t 9 - Борщ\n>>"; 
        cin >> tmp;
        message = tmp.c_str();
        iResult = send(ConnectSocket, message, (int)strlen(message), 0);
        if (iResult == SOCKET_ERROR) {
            cout << "send failed with error: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return 15;
        }
        else {
            cout << "данные успешно отправлены на сервер: " << message << "\n";
            cout << "байтов с клиента отправлено: " << iResult << "\n";
        }

        iResult = recv(ConnectSocket, answer, DEFAULT_BUFLEN, 0);
        answer[iResult] = '\0';

        if (iResult > 0) {
            cout << "процесс сервера прислал ответное сообщение: " << answer << "\n";
            cout << "байтов получено: " << iResult << "\n";
        }
        else if (iResult == 0)
            cout << "соединение с сервером закрыто.\n";
        else
            cout << "recv failed with error: " << WSAGetLastError() << "\n";

    } while (message[0] != '0');


    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
        closesocket(ConnectSocket);
        WSACleanup();
        return 16;
    }
    else {
        cout << "процесс клиента инициирует закрытие соединения с сервером.\n";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    cout << "процесс клиента прекращает свою работу!\n";

    return 0;
}