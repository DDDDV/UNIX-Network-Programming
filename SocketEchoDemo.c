#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

void error_die(const char * p){
    printf("%s\n", p);
    return ;
}

int StartUpServer(int *port){
    int httpd = -1;
    httpd = socket(AF_INET, SOCK_STREAM, 0);//SOCK_STREAM使用TCP连接，SOCK_DGRAM使用udp AF_INET使用ipv4，第三个参数设置为0即可
    if (httpd == -1){
        error_die("socket");
    }
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(*port);
    server.sin_addr.s_addr = (INADDR_ANY);
    int on = 1;
    if (getsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1){
        error_die("getsockopt");
    }
    if (bind(httpd, (struct sockaddr * )&server, sizeof(server)) == -1){
        error_die("bind");
    }
    if(*port == 0){
        socklen_t serverlen;
        if(getsockname(httpd, (struct sockaddr *)&server, &serverlen) == -1){
            error_die("getsockname");
        }
        port = ntohs(server.sin_port);
    }
    return httpd;

}

int main(){
    int port = 0;
    int client_sock = -1;
    int httpd = StartUpServer(&port);
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));

    socklen_t client_len = sizeof(client);
    client_sock = accept(httpd, (struct sockaddr *)&client, &client_len);
    if (client_sock == -1){
        error_die("accept");
    }
    char buf[1024];
}