#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define MAX_REQ_LEN 1000
#define FBLOCK_SIZE 4000

// Responses
#define COD_OK0_GET         "OK-0 method GET OK\n"
#define COD_OK1_FILE         "OK-1 File open OK\n"
#define COD_ERROR_0_METHOD  "ERROR-0 Method not supported\n"
#define COD_ERROR_1_FILE "ERROR-1 File could not be open\n"
#define COD_ERROR_2_NOT_FOUND "ERROR-2 File could not be found\n"

// Allowed Methods
#define REQ_GET "GET"
#define REQ_CREATE "CREATE"
#define REQ_REMOVE "REMOVE"
#define REQ_APPEND "APPEND"


struct req_t {
    char method[128];
    char filename[256];
};
typedef struct req_t req_t;

void get_request(req_t * resquest, char *rstr){
    bzero(resquest, sizeof(req_t));
    sscanf(rstr, "%s %s", resquest->method, resquest->filename);
}

void send_response_file(int sockfd, req_t request){
    int fd, nr;
    unsigned char fbuff[FBLOCK_SIZE];
    fd = open(request.filename, O_RDONLY, S_IRUSR);
    if(fd == -1){
        perror("open");
        send(sockfd, COD_ERROR_1_FILE, strlen(COD_ERROR_1_FILE), 0);
        return;
    }
    send(sockfd, COD_OK1_FILE, strlen(COD_OK1_FILE), 0);
    do{
        bzero(fbuff, FBLOCK_SIZE);
        nr = read(fd, fbuff, FBLOCK_SIZE);
        if (nr > 0){
            send(sockfd, fbuff, nr, 0);
        }
    } while (nr > 0);
    close(fd);
    return;
    
}

void proc_get_request(int sockfd, req_t request){
    send(sockfd, COD_OK0_GET, strlen(COD_OK0_GET), 0);
    send_response_file(sockfd, request);
}

void proc_create_request(int sockfd, req_t request){

}

void proc_remove_request(int sockfd, req_t request){
    if (access(request.filename, F_OK) == 0) {
        remove(request.filename);
    }
    else {
        perror("create not found");
        send(sockfd, COD_ERROR_2_NOT_FOUND, strlen(COD_ERROR_2_NOT_FOUND), 0);
    }
}

void proc_append_request(){

}

int main(int argc, char const *argv[])
{
    if (argc != 2){
        printf("Uso: %s <porta>\n", argv[0]);
        return 0;
    }
    int sl; // descritor de arquivo
    sl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sl == -1){
        perror("socket");
        return 0;
    }
    struct sockaddr_in saddr; 
    saddr.sin_port = htons(atoi(argv[1])); // o valor da porta esta em argv[1], e necessario converter para int, e para a ordem do byte correto
    saddr.sin_family = AF_INET; // ipv4
    saddr.sin_addr.s_addr = INADDR_ANY; // endereço IP vinculado a porta, faz o bind em todos os endereços IPs existentes na maq
    if(bind(sl, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) == -1){
        perror("bind");
        return 0;
    }
    // 1000 clientes podem estar aguardando conexão com o servidor
    if (listen(sl, 1000) == -1){
        perror("listen");
        return 0;
    }
    
    struct sockaddr_in caddr;
    int addr_len;
    int sc, nr; // socket cliente
    char resquest[MAX_REQ_LEN];
    req_t req;

    while (1){
        addr_len = sizeof(struct sockaddr_in);
        sc = accept(sl, (struct sockaddr*)&caddr, (socklen_t *)&addr_len);
        if(sc == -1){
            perror("accept");
            continue;
        }

        printf("Conectado com o cliente %s:%d\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));

        bzero(resquest, MAX_REQ_LEN);
        nr = recv(sc, resquest, MAX_REQ_LEN, 0);
        if(nr > 0){
            get_request(&req, resquest);
            printf("method: %s\nfilename: %s\n", req.method, req.filename);
            if (strcmp(req.method, REQ_GET) == 0) {
                proc_get_request(sc, req);
            }
            else if (strcmp(req.method, REQ_CREATE) == 0) {
                proc_create_request(sc, req);
            }
            else if (strcmp(req.method, REQ_REMOVE) == 0) {
                proc_remove_request(sc, req);
            }
            else if (strcmp(req.method, REQ_APPEND) == 0) {
                proc_append_request();
            }
            else {
                send(sc, COD_ERROR_0_METHOD, strlen(COD_ERROR_0_METHOD), 0);
            }
        }
        //send
        close(sc);
    }
    close(sl);
    return 0;
}
