#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <string>

#define BUF_SIZE 1024
#define Clients 10  // Підключення до 10 клієнтів

using namespace std;

struct MsgData
{
    char mess [BUF_SIZE];
    int fromClient;
};

MsgData MsgArr [Clients];
SOCKET Sockets[Clients] = {0,0,0,0,0,0,0,0,0,0};

void ClientHandler (int idx){

  char buffer [BUF_SIZE];

  while (1){

      if (Sockets[idx] == INVALID_SOCKET){
          break;
      }

      int result = recv(Sockets[idx],buffer,BUF_SIZE,0);

      if (result > 0){
        string msg(buffer);
        // format: "fromId=XX_toId=XX_msg:message"
        if (msg.find("fromId=") == 0 && msg.find("toId=") == 10){
           int fromId = atoi(msg.substr(7,2).c_str());
           int toId = atoi(msg.substr(15,2).c_str());


           if (fromId < Clients){
                // Запис повідомлення для клієнта c індексом toId
                int start_pos = msg.find_first_of(":")+1;
                string message = msg.substr(start_pos,msg.length()-start_pos);
                string storedMesssge (MsgArr[toId-1].mess);
                if (storedMesssge.length()){
                    storedMesssge+='\n';
                    message = storedMesssge + message;
                }
                strcpy(MsgArr[toId-1].mess, message.c_str());
                MsgArr[toId-1].fromClient = fromId;

                char outbuff [1024] = "\0";

                // Перевірка повідомлень для клієнта
                if (MsgArr[fromId-1].fromClient > 0){
                    // Є повідомлення для клієнта
                   sprintf(outbuff, "fromId=%02d_msg:%s", MsgArr[fromId-1].fromClient, MsgArr[fromId-1].mess);
                }

                 send (Sockets[idx],outbuff,sizeof(outbuff),0);

                 // Очистка переданих повідомлень
                 MsgArr[fromId-1].fromClient = 0;
                 MsgArr[fromId-1].mess[0] = '\0';

           }
        }
      }
      else
       break;

   }
   closesocket(Sockets[idx]);
   Sockets[idx] = 0;
   cout <<"Socket with index "<<idx<<" closed."<<endl;
   return;

}


int main (int argc,char* argv[])
{
    int index = 0;

    WSAData wsaData;
    WORD DllVersion = MAKEWORD (2, 1);
    if (WSAStartup (DllVersion, &wsaData)!=0)
    {
        cout<<"Error downloading lib"<<endl;
        exit (1);
    }
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons (1111);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket (AF_INET, SOCK_STREAM,IPPROTO_TCP);
    bind (sListen, (SOCKADDR*)&addr, sizeof (addr));
    listen (sListen, SOMAXCONN);
    if (listen (sListen, SOMAXCONN) == SOCKET_ERROR)
    {
        cout <<"Listening  failed with error: %d/n",WSAGetLastError();
        WSACleanup();
        return 1;
    }

    cout <<"Waiting for connection!"<<endl;

    while (1){

        while (Sockets[index]!=0){
            (index < Clients -1)?index++:index=0;
        }

        Sockets[index] = accept (sListen, (SOCKADDR*)&addr,&sizeofaddr);

        if (Sockets[index] == INVALID_SOCKET){
            cout <<"Connection Error!"<<endl;
            closesocket(Sockets[index]);
            WSACleanup();
            Sockets[index] = 0;
            continue;
        }

        cout <<"New client successfully connected"<<endl;

        CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID) index ,0,NULL);

    }

    system ("pause");
    return 0;
}
