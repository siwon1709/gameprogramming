#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define LEFT  75
#define RIGHT 77
#define UP    72
#define DOWN  80
#define ESC   27

void gotoxy(int x, int y) {
    COORD pos;
    pos.X = (SHORT)(x - 1);
    pos.Y = (SHORT)(y - 1);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void hide_cursor(void) {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = FALSE;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void draw_rectangle(int start_x, int start_y, int c, int r) {
    int i;
    unsigned char a = 0xa6;
    unsigned char h = 0xac;
    unsigned char v = 0xad;
    unsigned char lu = 0xae;
    unsigned char ru = 0xaf;
    unsigned char rd = 0xb0;
    unsigned char ld = 0xb1;

    int right_x = start_x + 2 + (c * 2);
    int bottom_y = start_y + r + 1;

    for (i = 0; i < c; i++) {
        gotoxy(start_x + 2 + (i * 2), start_y);
        printf("%c%c", a, h);
        gotoxy(start_x + 2 + (i * 2), bottom_y);
        printf("%c%c", a, h);
    }

    for (i = 1; i <= r; i++) {
        gotoxy(start_x, start_y + i);
        printf("%c%c", a, v);
        gotoxy(right_x, start_y + i);
        printf("%c%c", a, v);
    }

    gotoxy(start_x, start_y);
    printf("%c%c", a, lu);
    gotoxy(right_x, start_y);
    printf("%c%c", a, ru);
    gotoxy(start_x, bottom_y);
    printf("%c%c", a, ld);
    gotoxy(right_x, bottom_y);
    printf("%c%c", a, rd);
}

void move_arrow_key(int key, int *val, int min_val, int max_val) {
    if (key == LEFT || key == UP) {
        if (*val > min_val) (*val)--;
    } else if (key == RIGHT || key == DOWN) {
        if (*val < max_val) (*val)++;
    }
}

int main(void) {
    int h_slide = 4, v_slide = 3;
    int h_max = 20, v_max = 10;
    
    int h_startX = 10, h_startY = 10;
    int v_startX = 65, v_startY = 6;
    
    int old_h = -1, old_v = -1;
    int key;

    hide_cursor();
    system("cls");

    gotoxy(h_startX, h_startY - 3);
    printf("<< 수평 슬라이드 바 >>");
    gotoxy(v_startX - 6, v_startY - 2);
    printf("<< 수직 슬라이드 바 >>");

    draw_rectangle(h_startX, h_startY, h_max, 1);
    draw_rectangle(v_startX, v_startY, 1, v_max);

    while (1) {
        if (h_slide != old_h) {
            if (old_h != -1) {
                gotoxy(h_startX + 2 + (old_h - 1) * 2, h_startY + 1);
                printf("  "); 
            }
            gotoxy(h_startX + 2 + (h_slide - 1) * 2, h_startY + 1);
            printf("%c%c", 0xa1, 0xe1);

            gotoxy(h_startX + (h_max * 2), h_startY - 1);
            printf("%3d", h_slide);

            old_h = h_slide;
        }

        if (v_slide != old_v) {
            if (old_v != -1) {
                gotoxy(v_startX + 2, v_startY + old_v);
                printf("  ");
                gotoxy(v_startX + 7, v_startY + old_v);
                printf("  ");
            }
            gotoxy(v_startX + 2, v_startY + v_slide);
            printf("%c%c", 0xa1, 0xe1);

            gotoxy(v_startX + 7, v_startY + v_slide);
            printf("%2d", v_slide);

            old_v = v_slide;
        }

        key = getch();
        if (key == ESC) break;
        
        if (key == -32 || key == 224 || key == 0) {
            key = getch();
            if (key == LEFT || key == RIGHT) {
                move_arrow_key(key, &h_slide, 1, h_max);
            } else if (key == UP || key == DOWN) {
                move_arrow_key(key, &v_slide, 1, v_max);
            }
        }
    }

    system("cls");
    return 0;
}
