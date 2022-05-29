
#include <iostream>
#include <winsock2.h>
#include <map>
#include <string.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

SOCKET clients[64];
int numClients = 0;

SOCKET connected[64];
int numConnected = 0;

map<SOCKET, char*> accTable;

const char* successAuth = "[CONNECT] OK\n";
const char* signIn = "Moi ban nhap id cua minh!\n";
const char* syntaxError = "[ERROR] Wrong syntax\n";

int CheckAccExist(char* accName) {
    for (auto& it : accTable)
        if (strcmp(accName, it.second) == 0)
            return 1;
    return 0;
}

void RemoveClient(SOCKET client)
{
    int i = 0;
    for (; i < numConnected; i++)
        if (connected[i] == client) break;
    
    if (i < numConnected - 1)
        connected[i] = connected[numConnected - 1];
    numConnected--;
}



int SendAll(SOCKET client, char* data, int jump)
{ 
    int success = 0 ;
    for (int i = 0; i < numConnected; i++)
    {
        if (connected[i] != client)
        {
            success = send(connected[i], data + jump, strlen(data) - jump, 0);
        }
    }
    return success;
}

DWORD WINAPI ClientThread(LPVOID param) {
    
    SOCKET client = *(SOCKET *)param;

    int ret;

    char idInput[256];
    char cmd[32], id[32], tmp[32];

    send(client, signIn, strlen(signIn), 0);

    while (true)
    {
        ret = recv(client, idInput, sizeof(idInput), 0);
        idInput[ret] = 0;

        ret = sscanf(idInput, "%s %s %s", cmd, id, tmp);
        if (ret != 2)
        {
            send(client, syntaxError, strlen(syntaxError), 0);
        }

        else
        {
            if (strcmp(cmd, "[CONNECT]") != 0)
            {
                send(client, syntaxError, strlen(syntaxError), 0);
            }
            else
            {
                int found = CheckAccExist(id);

                if (found == 0)
                {
                    send(client, successAuth, strlen(successAuth), 0);
                    connected[numConnected] = client;
                    memcpy(accTable[client], id, strlen(id));
                    numConnected++;

                    char msg[30];
                    sprintf(msg, "[USER_CONNECT] %s - New user enters chat room!\n", accTable[client]);
                    SendAll(client, msg, 0);
                    break;
                }
                else
                {
                    const char* msg = "[CONNECT] ERROR - This user's id already exist!\n";
                    send(client, msg, strlen(msg), 0);
                }
            }
        }
    }

    char buff[256];

    while (true)
    {
        ret = recv(client, buff, sizeof(buff), 0);
        buff[ret] = 0;
        printf("%d: %s",(int)client, buff);

        ret = sscanf(buff, "%s %s", cmd, id);

        int lengthCode = strlen(cmd) + strlen(id) + 2;

        if (strcmp(cmd, "[SEND]")==0)
        {
            if (strcmp(id, "ALL") == 0)
            {
                
                int success = SendAll(client, buff, lengthCode);

                if (success)
                {
                    const char* msg = "[SEND] OK-Gui tin nhan thanh cong!\n";
                    send(client, msg, strlen(msg), 0);
                }

                else
                {
                    const char* msg = "[SEND] ERROR error_message - Gui tin nhan that bai!\n";
                    send(client, msg, strlen(msg), 0);
                }
            }
            else
            {
                for (int i = 0; i < numConnected; i++)
                {
                    if (strncmp(id, accTable[connected[i]], strlen(id))==0)
                    {
                        char msg[30];
                        sprintf(msg, "[MESSAGE] %s: ", accTable[client]);
                        send(connected[i], msg, strlen(msg), 0);
                        send(connected[i], buff + lengthCode, strlen(buff) - lengthCode, 0);
                    }
                    break;
                }
            }
        }

        else if (strcmp(cmd, "[LIST]") == 0)
        {
            char temp[256] = "[LIST] OK";

            for (int i = 0; i < numConnected; i++)
            {
                sprintf(temp + strlen(temp), " %s", accTable[connected[i]]);
            }

            temp[strlen(temp)] = '\n';

            int ret = send(client, temp, strlen(temp), 0);
            
            if (ret < 0)
            {
                const char* msg = "[LIST] ERROR - Lay danh sach that bai!";
                send(client, msg, strlen(msg), 0);
            }
        }

        else if (strcmp(cmd, "[DISCONNECT]") == 0) {
            char msg[30];
            sprintf(msg, "[USER_DISCONNECT] %s\n",accTable[client]);
            SendAll(client, msg, 0);
            RemoveClient(client);
            accTable.erase(client);
            break;
        }
    
        else
        {
            const char* msg = "[ERROR] Wrong syntax!\n";
            send(client, msg, strlen(msg), 0);
        }
        
    }

}

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);

    char buffer[1024];
    
    printf("Start Server!!\n");

    while (true)
    {
        clients[numClients] = accept(listener, NULL, NULL);
        printf("Client moi ket noi: %d\n",(int) clients[numClients]);
        CreateThread(0, 0, ClientThread, &clients[numClients], 0, 0);
        numClients++;
    }

}
