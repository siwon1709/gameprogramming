#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define LEFT  75
#define RIGHT 77
#define UP    72
#define DOWN  80
#define ESC   27

// 콘솔 커서 위치 이동 함수
void gotoxy(int x, int y)
{
    COORD pos = { (SHORT)(x - 1), (SHORT)(y - 1) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// 커서 숨기기 함수
void hide_cursor(void)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = FALSE;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 사각형(Bar 틀)을 그리는 함수
// c: 가로 칸 수 (전각 문자 단위), r: 세로 줄 수
void draw_rectangle(int start_x, int start_y, int c, int r)
{
    int i, j;
    // 확장 아스키 / 완성형(CP949) 선 그리기 문자
    unsigned char a = 0xa6;
    unsigned char b[7];
    for (i = 1; i < 7; i++)
        b[i] = 0xa0 + i;
    // b[1]: ─, b[2]: │, b[3]: ┌, b[4]: ┐, b[5]: ┘, b[6]: └

    // 상단 테두리: ┌───┐
    gotoxy(start_x, start_y);
    printf("%c%c", a, b[3]);
    for (i = 0; i < c; i++)
        printf("%c%c", a, b[1]);
    printf("%c%c", a, b[4]);

    // 중간 내부 영역: │   │
    for (i = 1; i <= r; i++) {
        gotoxy(start_x, start_y + i);
        printf("%c%c", a, b[2]);
        for (j = 0; j < c; j++)
            printf("  "); // 전각 문자 크기(2칸) 공백
        printf("%c%c", a, b[2]);
    }

    // 하단 테두리: └───┘
    gotoxy(start_x, start_y + r + 1);
    printf("%c%c", a, b[6]);
    for (i = 0; i < c; i++)
        printf("%c%c", a, b[1]);
    printf("%c%c", a, b[5]);
}

// 화살표 키 입력에 따라 슬라이드 값 및 위치 갱신
void move_arrow_key(char key, int *val, int min_val, int max_val)
{
    switch (key) {
    case LEFT:
    case UP:
        if (*val > min_val) (*val)--;
        break;
    case RIGHT:
    case DOWN:
        if (*val < max_val) (*val)++;
        break;
    }
}

// 1. 수평 슬라이드 바 실행 함수
void run_horizontal_slider(void)
{
    system("cls");
    int start_x = 10, start_y = 6;
    int length = 20; // 슬라이드 바 전체 길이
    int slide = 4;   // 초기 슬라이드 위치/크기 값
    char key;

    gotoxy(10, 3);
    printf("<< 수평 슬라이드 바 >> [←, → 이동 / ESC 종료]");

    // 사각형 Bar 출력 (가로 length칸, 세로 1칸)
    draw_rectangle(start_x, start_y, length, 1);

    while (1) {
        // 1. 현재 값(크기) 표시: 사각형 우측 상단에 2자리 정수로 출력
        gotoxy(start_x + (length * 2) - 1, start_y - 1);
        printf("%3d", slide);

        // 2. 사각형 내부 이전 슬라이드 지우기
        gotoxy(start_x + 2, start_y + 1);
        for (int i = 0; i < length; i++)
            printf("  ");

        // 3. 현재 위치에 검은 사각형(■) 슬라이드 표시
        // (x좌표는 좌측 테두리(2byte) + (slide - 1) * 2byte 칸)
        gotoxy(start_x + 2 + (slide - 1) * 2, start_y + 1);
        printf("%c%c", 0xa1, 0xe1); // 기호 ■ 출력

        // 키 입력 처리
        key = getch();
        if (key == ESC) break;
        if (key == -32 || key == 0) { // 방향키 접두 코드
            key = getch();
            if (key == LEFT || key == RIGHT) {
                move_arrow_key(key, &slide, 1, length);
            }
        }
    }
}

// 2. 수직 슬라이드 바 실행 함수
void run_vertical_slider(void)
{
    system("cls");
    int start_x = 20, start_y = 5;
    int height = 12; // 슬라이드 바 전체 높이
    int slide = 3;   // 초기 슬라이드 위치/크기 값
    char key;

    gotoxy(15, 3);
    printf("<< 수직 슬라이드 바 >> [↑, ↓ 이동 / ESC 종료]");

    // 사각형 Bar 출력 (가로 1칸, 세로 height칸)
    draw_rectangle(start_x, start_y, 1, height);

    while (1) {
        // 1. 현재 값(크기) 표시: 사각형 우측 하단에 출력
        gotoxy(start_x + 6, start_y + height + 1);
        printf("%2d", slide);

        // 2. 사각형 내부 이전 슬라이드 지우기
        for (int i = 1; i <= height; i++) {
            gotoxy(start_x + 2, start_y + i);
            printf("  ");
        }

        // 3. 현재 위치에 검은 사각형(■) 슬라이드 표시
        gotoxy(start_x + 2, start_y + slide);
        printf("%c%c", 0xa1, 0xe1); // 기호 ■ 출력

        // 키 입력 처리
        key = getch();
        if (key == ESC) break;
        if (key == -32 || key == 0) { // 방향키 접두 코드
            key = getch();
            if (key == UP || key == DOWN) {
                move_arrow_key(key, &slide, 1, height);
            }
        }
    }
}

int main(void)
{
    hide_cursor();
    char menu;

    while (1) {
        system("cls");
        printf("===============================\n");
        printf("       슬라이드 바 프로그램     \n");
        printf("===============================\n");
        printf(" 1. 수평 슬라이드 바 (←, →)\n");
        printf(" 2. 수직 슬라이드 바 (↑, ↓)\n");
        printf(" 3. 프로그램 종료\n");
        printf("===============================\n");
        printf("선택: ");
        menu = getch();

        if (menu == '1') {
            run_horizontal_slider();
        } else if (menu == '2') {
            run_vertical_slider();
        } else if (menu == '3' || menu == ESC) {
            break;
        }
    }

    system("cls");
    return 0;
}