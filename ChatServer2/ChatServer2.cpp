#include <winsock2.h>
#include <stdio.h>
#include <map>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

SOCKET clients[64];
int numClients = 0;

SOCKET connected[64];
int numConnected = 0;

map<SOCKET, char[32]> accTable;

const char* syntaxError = "[ERROR] Wrong syntax\n";

bool CheckAccExist(char * accName) {
    for ( auto&it :accTable )
    {
        if (strcmp(accName, it.second)==0)
        {
            return true;
        }
    }
    return false;
}

void RemoveClient(SOCKET* clients, int* numClients, int i)
{
    if (i < *numClients - 1)
        clients[i] = clients[*numClients - 1];
    *numClients -= 1;
}

int SendAll(SOCKET client, char* data, int jump)
{
    int success = 0;
    for (int i = 0; i < numConnected; i++)
    {
        if (connected[i] != client)
        {
            success = send(connected[i], data + jump, strlen(data) - jump, 0);
        }
    }
    return success;
}

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);

    fd_set fdread;
    int ret;

    char buf[256];
    char cmd[32], id[32], tmp[32];

    while (1)
    {
        FD_ZERO(&fdread);
        FD_SET(listener, &fdread);
        for (int i = 0; i < numClients; i++)
            FD_SET(clients[i], &fdread);
        ret = select(0, &fdread, 0, 0, 0);
        if (ret > 0)
        {
            
            if (FD_ISSET(listener, &fdread))
            {
                SOCKET client = accept(listener, NULL, NULL);
                printf("New connection: %d\n",(int) client);
                const char* msg = "Hello client\n";
                send(client, msg, strlen(msg), 0);
                clients[numClients] = client;
                numClients++;
            }
            for (int i = 0; i < numClients; i++)
            {
                if (FD_ISSET(clients[i], &fdread))
                {
                    ret = recv(clients[i], buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        RemoveClient(clients, &numClients, i);
                        i--;
                        continue;
                    }
                    buf[ret] = 0;
                    printf("Du lieu nhan duoc tu %d: %s",(int) clients[i], buf);
                    
                    SOCKET client = clients[i];
                    int j = 0;
                    for (; j < numConnected; j++)
                        if (connected[j] == client) break;
                    if (j == numConnected)
                    {
                        
                        ret = sscanf(buf, "%s %s %s", cmd, id, tmp);

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
                                if (!CheckAccExist(id))
                                {
                                    char msgAll[32];
                                    const char* msg = "[CONNECT] OK\n";
                                    sprintf(msgAll, "[USER_CONNECT] %s - New user enters chat room!\n", id);

                                    send(client, msg, strlen(msg), 0);
                                    SendAll(client, msgAll, 0);

                                    connected[numConnected] = client;
                                    memcpy(accTable[client], id, strlen(id));
                                    numConnected++;
                                }
                                else
                                {
                                    const char* msg = "This id has existed. Please enter different id!\n";
                                    send(client, msg, strlen(msg), 0);
                                }
                            }
                        }
                    }
                    else
                    {
                        char code[32];
                        
                        ret = sscanf(buf, "%s %s", cmd, code);
                        int lengthCode = strlen(cmd) + strlen(code) + 2;

                        if (strcmp(cmd, "[DISCONNECT]") == 0) {
                            char msg[32];
                            sprintf(msg, "[USER_DISCONNECT] %s\n", accTable[connected[i]]);
                            SendAll(client, msg, 0);
                            RemoveClient(connected, &numConnected, i);
                            accTable.erase(client);
                            continue;
                        }

                        else if (strcmp(cmd, "[LIST]")==0)
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

                        else if (strcmp(cmd, "[SEND]")==0)
                        {
                            int success = 0;
                            if (strcmp(code, "ALL") == 0)
                            {
                                char msg[32];
                                sprintf(msg, "[MESSAGE_ALL] %s: %s", accTable[client], buf+lengthCode);
                                success = SendAll(client, msg, 0);
                            }
                            else
                            {
                                for (int i = 0; i < numConnected; i++)
                                {
                                    if (strcmp(code, accTable[connected[i]]) == 0)
                                    {
                                        char msg[32];
                                        sprintf(msg, "[MESSAGE] %s: %s", accTable[client], buf + lengthCode);
                                        success= send(connected[i], msg, strlen(msg), 0);
                                        break;
                                    }
                                }
                            }
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
                        else send(client, syntaxError, strlen(syntaxError), 0);
                    }
                }
            }
        }
    }
}