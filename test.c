#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h> //소리재생
#pragma comment(lib, "winmm.lib")

// 색상 정의
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define WIDTH 40 //가로
#define HEIGHT 25 //세로

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다. 
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d
#define SPACE 0x20 //스페이스바 코드
#define ENTER 0x0D //엔터키
#define z 0x7A //컨트롤 키

void erase_bullet(int i, int k);

/*총알 구조체*/
struct {
	int exist;//총알 존재여부

	int bx;
	int by;
	int boldx;
	int boldy;
} Bullet[11]; //10개 선언

/*적 구조체*/
struct {
	int exist; //적 존재여부
	int hit_count; //총알 맞은 수

	int enewx;
	int enewy;
	int eoldx;
	int eoldy;
} Enemy[11]; //적 최대 10개

/*미사일 구조체*/
struct {
	int exist;

	int mx;
	int my;
	int moldx;
	int moldy;
} Missile[4];

int keep_moving = 1;
int oldx = 10, oldy = 10; //플레이어 예전 좌표
int newx = 10, newy = 10; //플레이어 최신 좌표
int score = 0; //점수 = 시간초 + 아이템 + 적 처치 수
int enemy_kill_count = 0; //적 처치 수
int items[WIDTH][HEIGHT] = { 0 }; //1이면 아이템 있다는 뜻
int items_interval = 10; //아이템 표시 간격
int items_x; //아이템 x좌표 위치
int items_y; //아이템 y좌표 위치
int eat_items_count = 0; //아이템 먹은 횟수
int frame_count = 0;
int p1_frame_sync = 2; //플레이어 속도
int bullet_frame_sync = 1; //총알 속도
int enemy_frame_sync = 6; //적 속도
int player_hit_count = 0; //플레이어가 충돌 한 횟수

/*화면 지우기용*/
void cls(int text_color, int bg_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

/*텍스트 컬러*/
void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

/*커서 지우기*/
void removeCursor(void) // 커서를 안보이게 한다
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

/*커서 이동*/
void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API 함수입니다. 이건 알필요 없어요
}

/*플레이어 그리기*/
void putplayer(int x, int y)
{
	gotoxy(x, y);
	printf("<^>");
}

/*플레이어 지우기*/
void eraseplayer(int x, int y)
{
	gotoxy(x, y);
	printf("   ");
}

/*메뉴에서 커서이동*/
void draw_menu_cursor(int x, int y)
{
	gotoxy(x, y);
	printf("▶");
}
void erase_menu_cursor(int x, int y)
{
	gotoxy(x, y);
	printf("  ");
}

/*도움말 보여줌*/
void show_help() {
	cls(WHITE, BLACK);
	gotoxy(37, 1);
	printf("도움말\n");
	gotoxy(10, 5);
	printf("1. 방향키로 조작, SPACE : 공격, Z : 특수공격");
	gotoxy(10, 7);
	printf("2. 적을 처치하면 점수를 얻습니다.");
	gotoxy(10, 9);
	printf("3. 아이템을 먹으면 최대 5번 미사일을 사용할 수 있습니다.");
	gotoxy(10, 11);
	printf("4. 3번 충돌하면 게임이 종료됩니다.");
	gotoxy(10, 12);
	printf("5. 적의 속도가 점점 빨라집니다.");
	gotoxy(25, 15);
	printf("아무키나 누르면 메뉴로 돌아갑니다.");
	getch();
	cls(WHITE, BLACK);
}

/*게임 종료*/
void show_quit()
{
	cls(WHITE, BLACK);
	gotoxy(30, 15);
	printf("-게임종료-");
	Sleep(500);
}

/*로고 보여줌*/
void show_logo()
{
	int x1 = 9;
	int y1 = 3;
	int x2 = 69;
	int y2 = 11;

	gotoxy(9, 3);
	printf("%s", "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");

	/*둘째 줄*/
	gotoxy(9, 4);
	printf("%s", "■■");
	printf("\n");
	gotoxy(21, 4);
	printf("%s", "■");
	gotoxy(31, 4);
	printf("%s", "■■■");
	gotoxy(43, 4);
	printf("%s", "■■■");
	gotoxy(55, 4);
	printf("%s", "■■");
	gotoxy(69, 4);
	printf("%s", "■");

	/*셋째 줄*/
	gotoxy(9, 5);
	printf("%s", "■");
	gotoxy(13, 5);
	printf("%s", "■■■■■");
	gotoxy(25, 5);
	printf("%s", "■■■");
	gotoxy(33, 5);
	printf("%s", "■");
	gotoxy(37, 5);
	printf("%s", "■■■");
	gotoxy(45, 5);
	printf("%s", "■");
	gotoxy(49, 5);
	printf("%s", "■■■");
	gotoxy(57, 5);
	printf("%s", "■");
	gotoxy(61, 5);
	printf("%s", "■■■■■");

	/*4번째 줄*/
	gotoxy(9, 6);
	printf("%s", "■");
	gotoxy(13, 6);
	printf("%s", "■■■■■");
	gotoxy(25, 6);
	printf("%s", "■■■");
	gotoxy(33, 6);
	printf("%s", "■");
	gotoxy(37, 6);
	printf("%s", "■■■");
	gotoxy(45, 6);
	printf("%s", "■");
	gotoxy(49, 6);
	printf("%s", "■■■■■");
	gotoxy(61, 6);
	printf("%s", "■■■■■");

	/*5번째 줄*/
	gotoxy(9, 7);
	printf("%s", "■■");
	gotoxy(19, 7);
	printf("%s", "■■");
	gotoxy(31, 7);
	printf("%s", "■■");
	gotoxy(45, 7);
	printf("%s", "■");
	gotoxy(49, 7);
	printf("%s", "■■■■■");
	gotoxy(67, 7);
	printf("%s", "■■");

	/*6번째 줄*/
	gotoxy(9, 8);
	printf("%s", "■■■■■");
	gotoxy(21, 8);
	printf("%s", "■");
	gotoxy(25, 8);
	printf("%s", "■■■■■");
	gotoxy(37, 8);
	printf("%s", "■■■");
	gotoxy(45, 8);
	printf("%s", "■");
	gotoxy(49, 8);
	printf("%s", "■■■■■");
	gotoxy(61, 8);
	printf("%s", "■■■■■");

	/*7번째 줄*/
	gotoxy(9, 9);
	printf("%s", "■■■■■");
	gotoxy(21, 9);
	printf("%s", "■");
	gotoxy(25, 9);
	printf("%s", "■■■■■");
	gotoxy(37, 9);
	printf("%s", "■■■");
	gotoxy(45, 9);
	printf("%s", "■");
	gotoxy(49, 9);
	printf("%s", "■■■");
	gotoxy(57, 9);
	printf("%s", "■");
	gotoxy(61, 9);
	printf("%s", "■■■■■");

	/*8번째 줄*/
	gotoxy(9, 10);
	printf("%s", "■");
	gotoxy(19, 10);
	printf("%s", "■■");
	gotoxy(25, 10);
	printf("%s", "■■■■■");
	gotoxy(37, 10);
	printf("%s", "■■■");
	gotoxy(45, 10);
	printf("%s", "■■");
	gotoxy(55, 10);
	printf("%s", "■■");
	gotoxy(69, 10);
	printf("%s", "■");

	/*마지막 줄*/
	gotoxy(9, 11);
	printf("%s", "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
	gotoxy(36, 12);
	printf("--------");
	gotoxy(36, 13);
	printf("Invaders");
	gotoxy(36, 14);
	printf("--------");
}

/*메뉴 보여줌*/
void show_menu()
{
	gotoxy(36, 18);
	printf("1. START");
	gotoxy(36, 19);
	printf("2. HELP");
	gotoxy(36, 20);
	printf("3. QUIT");
}

/*메뉴 선택(커서이동)*/
int choice_menu()
{
	unsigned char ch;//방향키 받음
	unsigned char ent;//엔터 받음
	int menu_oldx = 34;
	int menu_oldy = 18;
	int menu_newx = 34;
	int menu_newy = 18;

	int menu_number = 0;

	draw_menu_cursor(menu_oldx, menu_oldy);

	while (1) {
		if (kbhit() == 1) {
			ch = getch();

			if (ch == SPECIAL1 || ch == SPECIAL2) {//입력한 키가 특수key라면?
				ch = getch();//키값 받음

				switch (ch) {
				case UP:
					if (menu_newy <= 18) {
						break;
					}
					menu_newy = menu_oldy - 1;
					break;
				case DOWN:
					if (menu_newy >= 20) {
						break;
					}
					menu_newy = menu_oldy + 1;
					break;
				}
			}

			if (ch == ENTER) {
				if (menu_newy == 18) {
					return 1;//
				}
				else if (menu_newy == 19) {
					return 2;//
				}
				else if (menu_newy == 20) {
					return 3;//
				}
			}

			erase_menu_cursor(menu_oldx, menu_oldy);
			draw_menu_cursor(menu_newx, menu_newy);
			menu_oldx = menu_newx;
			menu_oldy = menu_newy;
		}
	}
}

/*맵 그린다 궤선 이용*/
void draw_map(int x1, int y1, int x2, int y2)
{
	int x;
	int y;

	for (x = x1; x <= x2; x = x + 2) {//가로 0칸부터 40칸(근데 0부터니까 41칸 그려진다)
		gotoxy(x, y1);
		printf("%s", "─");
		gotoxy(x, y2);
		printf("%s", "─");
	}

	for (y = y1; y <= y2; y++) {//세로 0칸부터 25칸까지
		gotoxy(x1, y);
		printf("%s", "│");
		gotoxy(x2, y);
		printf("%s", "│");
	}

	gotoxy(x1, y1); printf("┌");
	gotoxy(x1, y2); printf("└");
	gotoxy(x2, y1); printf("┐");
	gotoxy(x2, y2); printf("┘");
}

/*적 지우기*/
void erase_enemy(int x, int y)
{
	gotoxy(x, y);
	printf("   ");
}

/*적 그리기*/
void draw_enemy(int x, int y)
{
	gotoxy(x, y);
	printf("<|>");
}

/*적 생성(최초위치)*/
void make_enemy(int i)
{
	int r = rand() % 36 + 2;//2에서 37까지
	int r2 = rand() % 36 + 2;

	Enemy[i].exist = 1;
	Enemy[i].hit_count = 0;

	Enemy[i].enewx = r;
	Enemy[i].eoldx = r;
	Enemy[i].enewy = 1;
	Enemy[i].eoldy = 1;
}

/*적 움직임(랜덤아님)*/
void move_enemy(int i)
{
	Enemy[i].enewy = Enemy[i].eoldy + 1;//적이 아래로 이동

	erase_enemy(Enemy[i].eoldx, Enemy[i].eoldy);
	draw_enemy(Enemy[i].enewx, Enemy[i].enewy);

	Enemy[i].eoldx = Enemy[i].enewx;
	Enemy[i].eoldy = Enemy[i].enewy;
}

/*적 랜덤하게 움직임 : 아래로 가되, 좌우로 랜덤하게 움직이면서감*/
void move_enemy_randomly(int i)
{
	int r_enewx;

	r_enewx = rand() % 3 - 1; //-1, 0, 1

	Enemy[i].enewx = Enemy[i].eoldx + r_enewx; //좌, 우, 중앙 셋중 하나 랜덤

	if (Enemy[i].enewx <= 2) {
		Enemy[i].enewx = Enemy[i].enewx + 1;
		Enemy[i].eoldx = Enemy[i].enewx;
	}
	if (Enemy[i].enewx >= 37) {
		Enemy[i].enewx = Enemy[i].enewx - 1;
		Enemy[i].eoldx = Enemy[i].enewx;
	}

	Enemy[i].enewy = Enemy[i].eoldy + 1;

	erase_enemy(Enemy[i].eoldx, Enemy[i].eoldy);
	draw_enemy(Enemy[i].enewx, Enemy[i].enewy);

	Enemy[i].eoldx = Enemy[i].enewx;
	Enemy[i].eoldy = Enemy[i].enewy;
}

/*총알과 적의 충돌 검사*/
void check_enemy_hit(int b, int e) //b는 총알, e는 적
{
	if ((Enemy[e].enewx == Bullet[b].bx || Enemy[e].enewx + 1 == Bullet[b].bx || Enemy[e].enewx + 2 == Bullet[b].bx)
		&& Enemy[e].enewy == Bullet[b].by) {
		++Enemy[e].hit_count; //충돌했다면 충돌 횟수 증가
		Bullet[b].exist = 0;
		erase_bullet(Bullet[b].bx, Bullet[b].by);
	}

	if (Enemy[e].hit_count == 3) { //총알이 적에 3번 맞으면
		PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\bullet.wav"), NULL, SND_FILENAME | SND_ASYNC);
		Enemy[e].exist = 0; //적 삭제
		erase_enemy(Enemy[e].enewx, Enemy[e].enewy);
		enemy_kill_count++; //적 처치 수 증가
	}
}

/*플레이어와 적의 충돌*/
void check_enemy_hit_player(int i)
{
	if ((newx - Enemy[i].enewx < 3 && newx - Enemy[i].enewx > -3) && newy == Enemy[i].enewy) { //만약 충돌하면
		Enemy[i].exist = 0; //충돌 한 적 지우고
		erase_enemy(Enemy[i].enewx, Enemy[i].enewy);
		player_hit_count++; //플레이어 hit_count증가
	}
	if (player_hit_count == 3) { //3번 충돌하면 게임종료
		cls(WHITE, BLACK);
		gotoxy(35, 10);
		printf("점수 : %d", score);
		gotoxy(35, 14);
		printf("게임 종료");
		gotoxy(24, 19);
		printf("아무키나 누르면 메뉴로 돌아갑니다");

		player_hit_count = 0; //히트카운트 초기화
		eat_items_count = 0; //아이템 먹은 횟수 초기화
		enemy_kill_count = 0; // 적 처치 수 초기화
		score = 0; //점수 초기화
		oldx = 10, oldy = 10;
		newx = 10, newy = 10;

		Sleep(1000);
		getch();
		getchar();//버퍼 땜시
		cls(WHITE, BLACK);
		main(); //아무키나 누르면 메인함수 다시 실행
	}
}

/*미사일과 적의 충돌*/
void check_enemy_hit_missile(int b, int e)
{
	if ((Enemy[e].enewx + 2 >= Missile[b].mx && Enemy[e].enewx <= Missile[b].mx + 2) && Enemy[e].enewy == Missile[b].my) {
		Enemy[e].hit_count += 3; //충돌하면 히트 카운트 3증가
	}

	if (Enemy[e].hit_count >= 3) {
		Enemy[e].exist = 0;
		erase_enemy(Enemy[e].enewx, Enemy[e].enewy);
		enemy_kill_count++;
	}
}

/*체력 보이기*/
void show_hp(int player_hit_count)
{
	if (player_hit_count == 0) {
		printf("HP  ♥♥♥");
	}
	else if (player_hit_count == 1) {
		printf("HP  ♥♥♡");
	}
	else if (player_hit_count == 2) {
		printf("HP  ♥♡♡");
	}
	else {
		printf("게임종료!!!!!");
	}
}

/*총알 그림*/
void draw_bullet(int x, int y)
{
	gotoxy(x, y);
	printf("!");
}

/*총알 지우기*/
void erase_bullet(int x, int y)
{
	gotoxy(x, y);
	printf(" ");
}

/*총알 생성(최초위치)*/
void making_bullet(int i)
{
	Bullet[i].exist = 1;

	/*생성된 총알의 최초위치*/
	Bullet[i].bx = newx + 1;
	Bullet[i].by = newy - 1;
	Bullet[i].boldx = newx + 1;
	Bullet[i].boldy = newy - 1;
}

/*총알 움직임*/
void moving_bullet(int i)
{
	Bullet[i].by = Bullet[i].boldy - 1;//총알 위로 이동

	erase_bullet(Bullet[i].boldx, Bullet[i].boldy);
	draw_bullet(Bullet[i].bx, Bullet[i].by);

	Bullet[i].boldx = Bullet[i].bx;
	Bullet[i].boldy = Bullet[i].by;//새로운 총알좌표를 이전 총알좌표라고 저장
}

/*아이템 보여줌*/
void show_items()
{
	int x, y;
	x = rand() % 37 + 2; //2부터 39까지
	y = rand() % 22 + 1; //1부터 23까지

	textcolor(BLACK, GREEN1);
	gotoxy(x, y);
	printf("P");
	items[x][y] = 1;
	items_x = x; //위치 기억
	items_y = y; //위치 기억
	textcolor(WHITE, BLACK);
}

/*아이템 지우기*/
void erase_items(int x, int y) //아이템이 있던 위치로 가서 지움
{
	gotoxy(x, y);
	printf(" ");
}

/*아이템 먹었는지 검사*/
int check_eat_items()
{
	if (items_x >= newx && items_x <= newx + 2 && items_y == newy) { //아이템 먹었다면
		return 1; //1반환
	}
	else 0; //안먹었다면 0반환
}

/*미사일 지우기*/
void erase_missile(int x, int y)
{
	gotoxy(x, y);
	printf("   ");
}

/*미사일 그리기*/
void draw_missile(int x, int y)
{
	gotoxy(x, y);
	printf("(!)");
}

/*미사일 생성(최초위치)*/
void make_missile(int i)
{
	Missile[i].exist = 1;

	/*생성된 미사일의 최초위치*/
	Missile[i].mx = newx;
	Missile[i].my = newy - 1;
	Missile[i].moldx = newx;
	Missile[i].moldy = newy - 1;
}

/*미사일 움직임*/
void move_missile(int i)
{
	Missile[i].my -= 1;

	erase_missile(Missile[i].moldx, Missile[i].moldy);
	draw_missile(Missile[i].mx, Missile[i].my);

	Missile[i].moldx = Missile[i].mx;
	Missile[i].moldy = Missile[i].my;//새로운 미사일좌표를 이전 미사일좌표라고 저장
}

/*플레이어 움직임*/
void moving_player(unsigned char ch)
{
	static unsigned char last_ch;

	if (keep_moving && ch == 0) {
		ch = last_ch;
	}
	last_ch = ch;

	switch (ch) {
	case UP:
		if (oldy > 1) {//위쪽 가로선이 부딪히기 전까지 이동
			newy = oldy - 1;//위로 이동
		}
		break;
	case DOWN:
		if (oldy < HEIGHT - 2) {
			newy = oldy + 1;//아래로 이동
		}
		break;
	case LEFT:
		if (oldx > 2) {
			newx = oldx - 1;//왼쪽 이동
		}
		break;
	case RIGHT:
		if (oldx < WIDTH - 3) {
			newx = oldx + 1;//오른쪽 이동
		}
		break;
	}
	eraseplayer(oldx, oldy);
	putplayer(newx, newy);
	oldx = newx;
	oldy = newy;
}

/*전체 게임 움직임 통제*/
void whole_game_movement()
{
	int run_time; //실행시간
	int start_time = time(NULL); //현재시각
	int enemy_time = 0; //마지막 적 생성시간 기억
	int enemy_speed_time = 0;
	int items_spawn_time = 0; //아이템 생성'시각' 기억
	int items_time = 0; //마지막 아이템 생성'시간' 기억
	int items_run_time = 0; //아이템이 생성된 후 얼마나 지났는지
	int items_time2 = 0;

	int t = 0; //아이템(0이면 아이템 없음, 1이면 존재)
	int i = 1; //총알
	int l = 1; //적
	int m = 0; //미사일
	unsigned char ch; //키 입력 값

	srand(time(NULL));

	while (1) {
		run_time = time(NULL) - start_time; //실행 시간 = 현재시각 - 시작시각
		score = run_time + 10 * enemy_kill_count; //점수 = 실행 시간 + 적 처지 수

		/*아이템 5초마다 생성*/
		if (t == 0) { //아이템이 없을 때
			if (run_time > items_time && (run_time % items_interval == 0)) { //10초에 한번씩 생성
				items_spawn_time = run_time; //아이템 생성시각 기록
				show_items();
				items_time = run_time;
				t = 1; //필드에 아이템이 있다고 표시
			}
		}

		items_run_time = time(NULL) - items_spawn_time; //아이템이 생성된지 얼마나 됐는지

		/*아이템*/
		if (t == 1) { //아이템이 존재할 때
			if (items_run_time > items_time2 && (items_run_time % 5 == 0)) { //생성된지 5초가 되면
				t = 0; //필드에있던 아이템 없다고 표시
				erase_items(items_x, items_y); //아이템 지우기
				items_time2 = items_run_time;
			}

			/*아이템 먹었는지 검사*/
			if (check_eat_items() == 1 && eat_items_count <= 5) { //만약 아이템을 먹었다면
				PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\eat-items.wav"), NULL, SND_FILENAME | SND_ASYNC);
				erase_items(items_x, items_y); //아이템지우고
				t = 0; //아이템 존재 안한다고 표시
				eat_items_count++; //아이템 먹은 횟수 증가
			}
		}

		/*적 1초마다 생성*/
		if (run_time > enemy_time && run_time % 1 == 0 && l < 11) { //1초마다
			make_enemy(l); //적 생성
			enemy_time = run_time; //나머지가 0인 시간동안 계속 반복되지 않도록 하기 위해서
			l++; //다음 적 생성위해서
		}

		/*적 속도가 10초마다 증가함*/
		if (run_time > enemy_speed_time && (run_time % 10) == 0 && enemy_frame_sync > 3) {
			enemy_frame_sync--;
			enemy_speed_time = run_time;
		}

		if (l % 10 == 0) {//적
			l = 1;
		}
		if (i % 10 == 0) {//총알
			i = 1;
		}
		if (m % 3 == 0) { //미사일
			m = 1;
		}

		if (kbhit() == 1) {//키보드가 눌렸다면?
			ch = getch();//눌린 키보드 값 받음

			if (ch == 27) {//ESC만나면? 중지
				gotoxy(55, 12);
				printf("-게임 종료-\n");
				Sleep(10000);
				exit(0);
			}

			if (ch == SPECIAL1 || ch == SPECIAL2) {//입력한 키가 특수key라면?
				ch = getch();//키값 받음

				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					moving_player(ch);
					break;
				default://방향키말고 다른거 눌렀을 때
					if (frame_count % p1_frame_sync == 0) {
						moving_player(0);
					}
				}
			}

			if (ch == SPACE && i < 11) { //스페이스바 누르면 총알 생성, 최대10개
				if (Bullet[i].exist == 0) {
					making_bullet(i);
					i++; //다음 총알 생성하기위해서
				}
			}

			if (ch == z && eat_items_count >= 1 && m < 4) {
				if (Missile[m].exist == 0) {
					PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\laser-gun.wav"), NULL, SND_FILENAME | SND_ASYNC);
					eat_items_count--; //아이템 카운트 깍음
					make_missile(m);
					m++;
				}
			}
		}

		else {//키보드 안눌렸으면 계속 이동

			/*총알 검사, 총알 이동*/
			for (int j = 1; j < 11; j++) {
				if (Bullet[j].exist == 1) {
					if (frame_count % bullet_frame_sync == 0) {
						moving_bullet(j);
					}
					if (Bullet[j].by == 1) { //총알이 천장에 닿으면
						Bullet[j].exist = 0; //총알 삭제
						gotoxy(Bullet[j].bx, Bullet[j].by);
						printf(" ");
					}
					/*총알하고 적하고 충돌 검사*/
					for (int k = 1; k < 11; k++) {
						if (Enemy[k].exist == 1) {
							check_enemy_hit(j, k); //j는 총알, k는 적
						}
					}
				}
			}

			/*미사일 검사, 이동*/
			for (int k = 1; k < 4; k++) {
				if (Missile[k].exist == 1) {
					if (frame_count % bullet_frame_sync == 0) {
						move_missile(k);
					}
					if (Missile[k].my == 1) {
						Missile[k].exist = 0;
						gotoxy(Missile[k].mx, Missile[k].my);
						printf("   ");
					}
					/*미사일과 적 충돌검사*/
					for (int j = 1; j < 11; j++) {
						if (Enemy[j].exist == 1) {
							check_enemy_hit_missile(k, j); //k는 미사일, j는 적
						}
					}
				}
			}

			/*적 검사, 적의 이동*/
			for (int k = 1; k < 11; k++) {
				if (Enemy[k].exist == 1) {
					if (frame_count % enemy_frame_sync == 0) {
						move_enemy_randomly(k);
					}
					if (Enemy[k].enewy == 23) { //적이 바닥에 닿으면
						Enemy[k].exist = 0; //적 삭제
						gotoxy(Enemy[k].enewx, Enemy[k].enewy);
						printf("   ");
					}
					check_enemy_hit_player(k); //적이랑 플레이어랑 충돌
				}
			}

			/*플레이어 이동*/
			if (frame_count % p1_frame_sync == 0) {
				moving_player(0);
			}
		}

		gotoxy(50, 11);
		printf("시간 : %d", run_time);

		gotoxy(50, 13);
		printf("점수 : %d", score);

		gotoxy(50, 15);
		show_hp(player_hit_count);

		gotoxy(50, 17);
		printf("적 처치 수 : %d", enemy_kill_count);

		gotoxy(50, 19);
		printf("아이템 수(최대 5개) : %d", eat_items_count);
		Sleep(10);
		frame_count++;
	}
}

/*메인 함수*/
int main(void)
{
	int menu = 0;
start:
	PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\opening-bgm.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	cls(WHITE, BLACK);
	show_logo();//게임 로고
	show_menu();//메뉴 보여줌
	menu = choice_menu();//메뉴 선택(커서 움직임)
	cls(WHITE, BLACK);//화면 다 지운다
	removeCursor();//커서 지우고
	draw_map(0, 0, WIDTH, HEIGHT - 1);//WIDTH=40, HEIGHT=25, 맵 그린다

	switch (menu) {
	case 1://게임시작
		PlaySound(0, 0, 0);
		whole_game_movement();
		break;
	case 2://도움말 보기
		PlaySound(0, 0, 0);
		show_help();
		goto start;//도움말 끝나면 다시 메뉴창으로
		break;
	case 3:
		show_quit();//게임종료
		break;
	}
	return 0;
}
