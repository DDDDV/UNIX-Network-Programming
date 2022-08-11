#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
void error_die(const char *p){
    printf("%s\n", p);
    return ;
}

void accept_request(void *arg){
    int client = (intptr_t)arg;
    
}

/**
 * @brief 通过指定的端口创建httpd，使httpd处于listen状态。如果没有指定端口，则随机分配端口，并且通过&port传回。
 * 
 * @param port 
 * @return int 
 */
int startup(u_short *port){
    int httpd = 0;
    int on = 1;
    struct sockaddr_in name;

    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if(httpd == -1){
        error_die("httpd error");
    }
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY指明允许接收连接的地址为0.0.0.0，即任何地址均可连接到本socket
    if( (setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0 ){ // 通过setsockopt指定可以复用端口地址，避免因为人为或者意外原因导致端口没有断开，而导致的重新连接时端口已经再使用的错误
        error_die("reuse addr failed");
    }
    if((bind(httpd, (struct sockaddr *)&name, sizeof(name))) < 0){
        error_die("bind");
    }

    if(*port == 0){
        socklen_t namelen = sizeof(name);
        if(getsockname(httpd, (struct sockaddr *)&name, &namelen == -1)){
            error_die("getsockname");
        }
        *port = ntohs(name.sin_port);
    }
    if(listen(httpd, 5) < 0){
        error_die("listen");
    }
    return(httpd);
}


int main(){
    int server_sock = -1;
    u_short port = 4000;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);
    pthread_t newthread;

    server_sock =startup(&port);
    printf("server is running at %d port\n", port);

    while(1){
        client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
        if(client_sock == -1){
            error_die("accept");
        }
        if (pthread_create(&newthread, NULL, (void *)accept_request, (void *)(intptr_t)client_sock) != 0){
            perror("pthread_create");
        }
        close(server_sock);
    }
    return 0;


}
