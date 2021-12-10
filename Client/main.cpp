#include <stdio.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#define Clients 10
#define BUF_SIZE 1024

using namespace std;


struct MsgData
{
    char mess [BUF_SIZE];
    int toClient;
    int fromClient;
};

MsgData data;
SOCKET connection;

bool connectToServer (void){

    WSAData wsaData;
    WORD DllVersion = MAKEWORD (2,2);
    if (WSAStartup (DllVersion,&wsaData)!=0)
    {
        cout << "Error downloading lib" << endl;
        exit (1);
    }
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons (1111);
    addr.sin_family = AF_INET;

    connection = socket (AF_INET, SOCK_STREAM,IPPROTO_TCP);

    if (connect(connection, (SOCKADDR*)&addr, sizeof(addr))!=0){
        cout<<"Error!Failed to connect to server"<<endl;
        return false;
    } else {

        cout<<"Successfuly connected"<<endl;
        return true;
    }

}

void disconnectFromServer (void){
    closesocket(connection);
    WSACleanup();
}

int main (int argc,char* argv[])
{
    cout<<"Enter your ID: ";
    cin>>data.fromClient;
    cout<<endl;

    if (connectToServer()){

        while (true){

            string line, text;
            cout<<"Enter your message: ";
            cin>>text; // Читаємо перше слово до пробілу

            getline(cin, line); // Читаеємо залишившусь частину строки після пробілу
            line = text+line;   // Додавання строк
            cout<<endl;

            strcpy(data.mess,line.c_str());

            if (strcmp(data.mess,"exit") == 0){
                break;
            }

            cout<<"Send to (index): ";
            cin>>data.toClient;
            cout<<endl;

            string str(data.mess);

            if (connection == INVALID_SOCKET)
                break;

            char outbuf[BUF_SIZE];
            sprintf(outbuf, "fromId=%02d_toId=%02d_msg:%s", data.fromClient, data.toClient, data.mess);
            send(connection,outbuf,sizeof(outbuf),0);

            char inbuf[BUF_SIZE];
            int result = recv(connection,inbuf,BUF_SIZE,0);
            if (result > 0){
                string msg(inbuf);
                if (msg.find("fromId=") == 0){
                    int fromId = atoi(msg.substr(7,2).c_str());
                    int start_pos = msg.find_first_of(":")+1;
                    string context = msg.substr(start_pos,msg.length()-start_pos);
                    cout <<"New message from [" << fromId << "]: " << context << endl;

                }
            }
        }

        disconnectFromServer();
    }

    system ("pause");
    return 0;

}
