#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>

void display_phone_pad(void);
void control_phone_pad(void);
void draw_check02(int c, int r);
void gotoxy(int x, int y);

char alpha[][3]={{' '}, {'.', 'q', 'z'}, {'a', 'b', 'c'}, 
                    {'d', 'e', 'f'}, {'g', 'h', 'i'}, {'j','k','l'},
		    {'m', 'n', 'o'}, {'p', 'r', 's'}, {'t', 'u', 'v'}, 
                    {'w', 'x', 'y'}};
int main(void)
{
	display_phone_pad();
	control_phone_pad();	
    	return 0;
}

void display_phone_pad(void)
{
              //[함수 10.2.2]의 정의 부분 참고
	int y, x, z, cnt=0;
    	draw_check02(3, 4);
	for(y=2;y<=8;y+=3)
	{
		for(x=3;x<=15;x+=6)
		{
			gotoxy(x, y);
			printf("%d", ++cnt);
			gotoxy(x+1, y+1);
			for(z=0;z<=2;z+=1)
				printf("%c", alpha[cnt][z]);
		}
	}
	gotoxy(9, 11);
	printf("0");
	gotoxy(10, 12);
	printf("SP");
	gotoxy(1, 15);
}
void control_phone_pad(void)
{
       //[함수 10.2.1]의 정의 부분 참고
        char c1, c2, cnt;
        time_t start, end;
	printf("숫자키 입력>");
	c1=getch();
	start=time(NULL);
	do
	{
		cnt=0;
		if (c1==' ')
			printf("\b");
		else
			printf("%c", alpha[c1-'0'][cnt]);
		do
		{
			c2=getch();
			end=time(NULL);
			if ((c1==c2) && ((end-start)<=2))
			{
				if (cnt>1)
					cnt=0;
				else
					cnt++;
				c1=c2;
				printf("\b%c", alpha[c1-'0'][cnt]);
				start=time(NULL);
			}
			else
				break;
		}while(1);
		start=time(NULL);
		c1=c2;
	}while(1); 

}
void draw_check02(int c, int r)
{
	//[함수 3.5.4]를 확장하여 사용 
    int i, j;
    unsigned char a=0xa6;
    unsigned char b[12]; 
    for(i=1;i<12;i++)
		b[i]=0xa0+i;
    printf("%c%c",a, b[3]);
    for(i=0;i<c-1;i++)
    {
		printf("%c%c%c%c", a, b[1], a, b[1]);
		printf("%c%c", a, b[8]);
    }
    printf("%c%c%c%c", a, b[1], a, b[1]);
    printf("%c%c", a, b[4]);
    printf("\n");
    for(i=0;i<r-1;i++)
    {
		printf("%c%c", a, b[2]);
		for(j=0;j<c;j++)
		{
			printf("    ");
			printf("%c%c",a, b[2]);
		}
		printf("\n");
		//
		printf("%c%c", a, b[2]);
		for(j=0;j<c;j++)
		{
			printf("    ");
			printf("%c%c",a, b[2]);
		}
		printf("\n");
		//
		printf("%c%c", a, b[7]);
		for(j=0;j<c-1;j++)
		{
			printf("%c%c%c%c", a, b[1], a, b[1]);
			printf("%c%c", a, b[11]);
		}
		printf("%c%c%c%c", a, b[1], a, b[1]);
		printf("%c%c",a, b[9]);
		printf("\n");
    }
    printf("%c%c", a, b[2]);
    for(j=0;j<c;j++)
    {
		printf("    ");
		printf("%c%c",a, b[2]);
    }
    printf("\n");
	//
	printf("%c%c", a, b[2]);
	for(j=0;j<c;j++)
	{
		printf("    ");
		printf("%c%c",a, b[2]);
	}
	printf("\n");
	//
    printf("%c%c", a, b[6]);
    for(i=0;i<c-1;i++)
    {
		printf("%c%c%c%c", a, b[1], a, b[1], a, b[1]);
		printf("%c%c", a, b[10]);
    }
	printf("%c%c%c%c", a, b[1], a, b[1], a, b[1]);
    printf("%c%c", a, b[5]);
    printf("\n");
}

void gotoxy(int x, int y)
{
   COORD Pos = {x - 1, y - 1};
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}
