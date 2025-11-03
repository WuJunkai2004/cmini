#include <stdio.h>
#include <terminst.h>
#include <unistd.h>
#include <string.h>
#include <ui.h>

#define MAX_LINE 512


char* models[] = {
    "gemini",
    "qwen",
    NULL
};

int model_selector() {
    int selected = 0;
    int total = 0;
    int in_menu = 1;
    while (models[total]) total++;
    terminst_init();
    int rows, cols;
    terminst_size(&rows, &cols);
    if(rows < 13){
        terminst_throw("Terminal too small! Please resize to at least 10 rows.");
    }
    widget_t* widget = widget_create(0, 0, 18, 7 + total * 3);
    widget_border(widget);
    ui_text(widget, 0, 0, " Model Selector", COLOR_YELLOW);
    while(in_menu){
        for(int i = 0; i < total; i++){
            ui_button(widget, 1, 2 + i * 3, 14, models[i],
                      COLOR_GREEN, i == selected, 1);
        }
        ui_button(widget, 1, 2 + total * 3, 14, "exit",
                  COLOR_CYAN, selected == total, 1);
        terminst_flush();
        switch(terminst_wait_key()){
            case KEY_UP:
            case 'W':
                selected = (selected != 0) ? selected - 1 : total;
                break;
            case KEY_DOWN:
            case 'S':
                selected = (selected != total) ? selected + 1 : 0;
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



