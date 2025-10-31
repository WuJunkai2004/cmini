#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define COLOR_RESET "\033[0m"
#define COLOR_CYAN "\033[36m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_GREEN "\033[32m"
#define COLOR_REVERSE "\033[7m" // 反显，用于高亮当前选择
#define CONFIG_FILE ".cmini_config"

#define MAX_LINE 512

char* models[] = {
    "gemini",
    "Qwen",
    NULL
};

int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);             // 读取终端设置
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);           // 关闭行缓冲与回显
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);    // 应用设置
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);    // 恢复原设置
    return ch;
}

void save_api_key(const char *model, const char *api_key) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), CONFIG_FILE);

    FILE *fp = fopen(path, "r");
    char lines[100][MAX_LINE];
    int count = 0;
    int found = 0;

    // 先读取已有内容
    if (fp) {
        while (fgets(lines[count], MAX_LINE, fp) && count < 100) {
            if (strncmp(lines[count], model, strlen(model)) == 0 && lines[count][strlen(model)] == '=') {
                snprintf(lines[count], MAX_LINE, "%s=%s\n", model, api_key);
                found = 1;
            }
            count++;
        }
        fclose(fp);
    }

    // 若没有找到该模型，则添加一行
    if (!found && count < 100) {
        snprintf(lines[count++], MAX_LINE, "%s=%s\n", model, api_key);
    }

    // 重新写回文件
    fp = fopen(path, "w");
    if (!fp) {
        perror("Failed to open config file");
        return;
    }
    for (int i = 0; i < count; i++) {
        fputs(lines[i], fp);
    }
    fclose(fp);

    printf("\033[0;32mAPI key for model '%s' saved successfully to %s\033[0m\n", model, path);
}

// 从配置文件加载 API Key
int load_api_key(const char *model, char *key_out, int size) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), CONFIG_FILE);

    FILE *fp = fopen(path, "r");
    if (!fp) return 0;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, model, strlen(model)) == 0 && line[strlen(model)] == '=') {
            strncpy(key_out, line + strlen(model) + 1, size);
            key_out[strcspn(key_out, "\n")] = 0; // 去掉换行符
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void deal_api_key(int selected) {
    // 输入 API Key
    char api_key[256];
    printf("Enter your API key: ");
    fgets(api_key, sizeof(api_key), stdin);
    api_key[strcspn(api_key, "\n")] = 0; // 去掉换行符
    // 保存api key
    save_api_key(models[selected], api_key);
    printf(COLOR_GREEN "API key saved for %s!\n" COLOR_RESET, models[selected]);
}

void model_selector() {
    int selected = 0;
    int total = 0;
    while (models[total]) total++;

    while (1) {
        printf("\033[H\033[J"); // 清屏
        printf(COLOR_YELLOW "Use ↑ ↓ to choose model, Enter to confirm.\n" COLOR_RESET);
        printf(COLOR_YELLOW "Available models:\n" COLOR_RESET);

        for (int i = 0; i < total; i++) {
            if (i == selected)
                printf(COLOR_REVERSE "  %s\n" COLOR_RESET, models[i]);
            else
                printf(COLOR_CYAN "  %s\n" COLOR_RESET, models[i]);
        }

        int c = getch();
        if (c == 27) { // 方向键前缀 ESC
            getch();    // 跳过 '['
            switch (getch()) {
                case 'A': // ↑
                    selected = (selected - 1 + total) % total;
                    break;
                case 'B': // ↓
                    selected = (selected + 1) % total;
                    break;
            }
        } else if (c == '\n') {
            printf(COLOR_GREEN "\nSelected model: %s\n" COLOR_RESET, models[selected]);
            break;
        }
    }

    deal_api_key(selected);
    
}



