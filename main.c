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

#include <time.h>

#include <logo.h>


char* progress_bar[] = {
    "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
};
const int progress_bar_size = sizeof(progress_bar) / sizeof(progress_bar[0]);
int progress_index = 0;
#define progressing() ({printf("%s\b", progress_bar[progress_index % progress_bar_size]); progress_index++; fflush(stdout);})

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
    raw_request_t* get_token = raw_init();
    raw_add_line(get_token, "GET /login HTTP/1.1");
    raw_add_line(get_token, "Host: app/login");
    raw_add_line(get_token, "");
    int fd_token = request_create("47.121.28.18", 15444);
    request_send_raw(fd_token, get_token);
    request_recv_all(fd_token, recv_msg.content, 8192);
    request_close(fd_token);
    raw_free(get_token);
    // 解析token
    char* body = getBody(recv_msg.content);
    if(!body){
        body = "Error: Unable to retrieve token from server.\n";
        exit(1);
    }
    char* token = malloc(32);
    sprintf(token, "Auth: %s", body);

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
        raw_request_t* post_chat = raw_init();
        raw_add_line(post_chat, "POST /chat HTTP/1.1");
        raw_add_line(post_chat, "Host: app/chat");
        raw_add_line(post_chat, token);
        raw_add_line(post_chat, "Content-Type: text/plain");
        raw_add_line(post_chat, content_length);
        raw_add_line(post_chat, "");
        raw_add_line(post_chat, shared_msg->content);
        smunlock(shared_msg);

        // 发送请求并获取响应
        int fd = request_create("47.121.28.18", 15444);
        request_send_raw(fd, post_chat);
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

const char* get_current_time_str() {
    static char time_str[6]; // HH:MM\0
    time_t t = time(NULL);
    struct tm tm_info;
    localtime_r(&t, &tm_info);  // 线程安全版本
    strftime(time_str, sizeof(time_str), "%H:%M", &tm_info);
    return time_str;
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
        printf(COLOR_BLUE "[You🐷 %s] >>> " COLOR_RESET, get_current_time_str());
        char input[8192];
        fgets(input, sizeof(input), stdin);
        printf(COLOR_GREEN "[Cmini🤖 %s]<<< " COLOR_RESET, get_current_time_str());
        fflush(stdout); // 立即输出到终端
        smlock(shared_msg);
        shared_msg->author_id = 1;  // terminal input
        strncpy(shared_msg->content, input, sizeof(shared_msg->content));
        smunlock(shared_msg);

        // 等待服务端回复，并流式输出
        int last_printed = 0;
        while(true){
            usleep(100000);  // 等待一点时间，避免占用 CPU
            smlock(shared_msg);
            if(shared_msg->author_id == 2){  // net client replied
                // 清理转圈圈残留
                printf("\b   \b"); // 回退 + 空格覆盖 + 再回退
                fflush(stdout);
                int len = strlen(shared_msg->content);
                while(last_printed < len){
                    // 每次打印一小段
                    putchar(shared_msg->content[last_printed]);
                    fflush(stdout); // 立即刷新输出
                    last_printed++;
                    usleep(30000);  // 打字机效果，可调
                }
                putchar('\n'); // 回复结束换行
                shared_msg->author_id = 0;  // reset
                smunlock(shared_msg);
                break;
            } else {
                // 正在等待，显示转圈圈
                progressing();
            }
            smunlock(shared_msg);
        }
    }
}