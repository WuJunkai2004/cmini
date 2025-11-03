#include <stdio.h>
#include <terminst.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 512


char* models[] = {
    "gemini",
    "qwen",
    NULL
};
const int model_amount = sizeof(models) / sizeof(models[0]) - 1; // exclude NULL

int model_selector() {
    int selected = 0;
    int in_menu = 1;
    terminst_init();
    int rows, cols;
    terminst_size(&rows, &cols);
    if(rows < model_amount + 3){
        terminst_throw("Terminal too small! Please resize to at least 10 rows.");
    }
    while(in_menu){
        terminst_clear();
        printf(COLOR_GRAY "Use Up/Down or W/S to navigate, Enter/Space to select.\n" COLOR_RESET);
        printf(COLOR_YELLOW "Available models:\n" COLOR_RESET);
        for(int i = 0; i < model_amount; i++){
            if(i == selected){
                printf(COLOR_GREEN "> %s\n" COLOR_RESET, models[i]);
            } else {
                printf("  %s\n", models[i]);
            }
        }
        if(selected == model_amount){
            printf(COLOR_RED "> exit\n" COLOR_RESET);
        } else {
            printf("  exit\n");
        }
        terminst_flush();
        switch(terminst_wait_key()){
            case KEY_UP:
            case 'W':
                selected = (selected != 0) ? selected - 1 : model_amount;
                break;
            case KEY_DOWN:
            case 'S':
                selected = (selected != model_amount) ? selected + 1 : 0;
                break;
            case KEY_ENTER:
            case KEY_SPACE:
                in_menu = 0;
                break;
        }
    }
    terminst_quit();
    return selected;
}