#include <request.h>
#include <signal.h>
#include <stdio.h>

#include <stdbool.h>
#include <stdlib.h>
#include <subprocess.h>

#include <terminst.h>
#include <sharemem.h>
#include <ui.h>

#include <string.h>
#include <unistd.h>

#define logo "          _____              _____ _           _   \n" \
             "    /\\   |_   _|            / ____| |         | |  \n" \
             "   /  \\    | |             | |    | |__   __ _| |_ \n" \
             "  / /\\ \\   | |             | |    | '_ \\ / _` | __|\n" \
             " / ____ \\ _| |_            | |____| | | | (_| | |_ \n" \
             "/_/    \\_\\_____|            \\_____|_| |_|\\__,_|\\__|"

char* progress_bar[] = {
    // 使用盲文模拟一个转圈
    "⠋⠉⠉", "⠉⠉⠙", "⠈⠉⠹", " ⠉⠽", " ⠨⠽", " ⠤⠽", "⠠⠤⠼",
    "⠤⠤⠴", "⠦⠤⠤", "⠧⠤⠄", "⠯⠤ ", "⠯⠅ ", "⠯⠉ ", "⠏⠉⠁",
};
const int progress_bar_size = 14;
int progress_index = 0;
#define progressing() ({printf("%s\b\b\b", progress_bar[progress_index % progress_bar_size]); progress_index++; fflush(stdout);})

struct msg_t {
    int author_id;      // 1 is terminal, 2 is net client for posting msg 
    char content[8192];
};
int server_pid;
struct msg_t* shared_msg;
struct msg_t recv_msg;

void server_quit(){
    exit(0);
}

void client_quit(){
    post_signal(server_pid, SIGINT);
    printf("\n");
    exit(0);
}


static char* message_handler(int op, const char* msg){
    static char response[1024 * 16] = {0};
    static int len = 0;

    if(op == 1){  // clear
        len = 0;
        return NULL;
    }
    if(op == 2){  // add line
        int msg_len = strlen(msg);
        if(len + msg_len + 2 >= 1024 * 16){
            return NULL;  // overflow
        }
        strcpy(response + len, msg);
        len += msg_len;
        response[len++] = '\r';
        response[len++] = '\n';
        response[len] = 0;
        return NULL;
    }
    if(op == 3){  // get
        return response;
    }
    if(op == 4){  // size
        int* ptr = (int*)msg;
        *ptr = len;
        return NULL;
    }
}
#define msg_clear()         message_handler(1, NULL)
#define msg_add_line(str)   message_handler(2, str)
#define msg_get()           message_handler(3, NULL)
#define msg_size(ptr)       message_handler(4, (const char*)ptr)


char* getBody(char* httpResponse) {
    char* body = strstr(httpResponse, "\r\n\r\n");
    if (body) {
        return body + 4;  // 跳过分隔符
    }
    return NULL;  // 未找到正文
}


fork_func(server){
    signal(SIGINT, server_quit);  // ignore SIGINT in server process

    // 初始获取token
    msg_clear();
    msg_add_line("GET /login HTTP/1.1");
    msg_add_line("Host: app/login");
    msg_add_line("");
    int fd = request_create("47.121.28.18", 15444);
    int msg_len = 0;
    msg_size(&msg_len);
    request_send(fd, msg_get(), msg_len + 1);
    request_recv_all(fd, recv_msg.content, 8192);
    request_close(fd);
    // 解析token
    char* body = getBody(recv_msg.content);
    if(!body){
        body = "Error: Unable to retrieve token from server.\n";
        exit(1);
    }
    char* token = malloc(32);

    while(true){
        usleep(100000);  // wait for msg
        smlock(shared_msg);
        if(shared_msg->author_id != 1){  // new msg from terminal
            smunlock(shared_msg);
            continue;
        }

        int  length = strlen(shared_msg->content);
        char content_length[128] = {};
        sprintf(content_length, "Content-Length: %d", length);
        
        // 拼接成标准的HTTP请求
        msg_clear();
        msg_add_line("POST /chat HTTP/1.1");
        msg_add_line("Host: app/chat");
        msg_add_line(token);
        msg_add_line("Content-Type: text/plain");
        msg_add_line(content_length);
        msg_add_line("");
        msg_add_line(shared_msg->content);
        smunlock(shared_msg);

        // 发送请求并获取响应
        int fd = request_create("47.121.28.18", 15444);
        int msg_len = 0;
        msg_size(&msg_len);
        request_send(fd, msg_get(), msg_len + 1);
        request_recv_all(fd, recv_msg.content, 8192);
        request_close(fd);

        // 分析响应体，获取聊天回复
        char* body = getBody(recv_msg.content);
        if(!body){
            body = "Error: Invalid response from server.\n";
        }
        int body_len = strlen(body);
        if(body[body_len - 1] != '\n'){
            body[body_len] = '\n';
            body[body_len + 1] = 0;
        }
        smlock(shared_msg);
        shared_msg->author_id = 2;  // net client replied
        strncpy(shared_msg->content, body, body_len + 2);
        smunlock(shared_msg);
    }
}

int main(){
    // register signal handler
    signal(SIGINT, client_quit);

    // make share memory
    shared_msg = smalloc(10230, sizeof(struct msg_t));
    smlock(shared_msg);
    shared_msg->author_id = 0;  // no msg
    smunlock(shared_msg);

    server_pid = pfork(server);

    // print logo
    printf(COLOR_CYAN logo COLOR_RESET "\n\n");

    while(true){
        printf(COLOR_BLUE ">>> " COLOR_RESET);
        char input[8192];
        fgets(input, sizeof(input), stdin);
        smlock(shared_msg);
        shared_msg->author_id = 1;  // terminal input
        strncpy(shared_msg->content, input, sizeof(shared_msg->content));
        smunlock(shared_msg);
        while(true){
            usleep(150000);  // wait for reply
            smlock(shared_msg);
            if(shared_msg->author_id == 2){  // net client replied
                printf(COLOR_GREEN "<<< " COLOR_RESET);
                printf(shared_msg->content);
                shared_msg->author_id = 0;  // reset
                smunlock(shared_msg);
                break;
            } else {
                progressing();
            }
            smunlock(shared_msg);
        }
    }
}