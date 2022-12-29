#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h> //�Ҹ����
#pragma comment(lib, "winmm.lib")

// ���� ����
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

#define WIDTH 40 //����
#define HEIGHT 25 //����

#define SPECIAL1 0xe0 // Ư��Ű�� 0xe0 + key ������ �����ȴ�.
#define SPECIAL2 0x00 // keypad ��� 0x00 + key �� �����ȴ�.

#define UP  0x48 // Up key�� 0xe0 + 0x48 �ΰ��� ���� ���´�. 
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d
#define SPACE 0x20 //�����̽��� �ڵ�
#define ENTER 0x0D //����Ű
#define z 0x7A //��Ʈ�� Ű

void erase_bullet(int i, int k);

/*�Ѿ� ����ü*/
struct {
	int exist;//�Ѿ� ���翩��

	int bx;
	int by;
	int boldx;
	int boldy;
} Bullet[11]; //10�� ����

/*�� ����ü*/
struct {
	int exist; //�� ���翩��
	int hit_count; //�Ѿ� ���� ��

	int enewx;
	int enewy;
	int eoldx;
	int eoldy;
} Enemy[11]; //�� �ִ� 10��

/*�̻��� ����ü*/
struct {
	int exist;

	int mx;
	int my;
	int moldx;
	int moldy;
} Missile[4];

int keep_moving = 1;
int oldx = 10, oldy = 10; //�÷��̾� ���� ��ǥ
int newx = 10, newy = 10; //�÷��̾� �ֽ� ��ǥ
int score = 0; //���� = �ð��� + ������ + �� óġ ��
int enemy_kill_count = 0; //�� óġ ��
int items[WIDTH][HEIGHT] = { 0 }; //1�̸� ������ �ִٴ� ��
int items_interval = 10; //������ ǥ�� ����
int items_x; //������ x��ǥ ��ġ
int items_y; //������ y��ǥ ��ġ
int eat_items_count = 0; //������ ���� Ƚ��
int frame_count = 0;
int p1_frame_sync = 2; //�÷��̾� �ӵ�
int bullet_frame_sync = 1; //�Ѿ� �ӵ�
int enemy_frame_sync = 6; //�� �ӵ�
int player_hit_count = 0; //�÷��̾ �浹 �� Ƚ��

/*ȭ�� ������*/
void cls(int text_color, int bg_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);
}

/*�ؽ�Ʈ �÷�*/
void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

/*Ŀ�� �����*/
void removeCursor(void) // Ŀ���� �Ⱥ��̰� �Ѵ�
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

/*Ŀ�� �̵�*/
void gotoxy(int x, int y) //���� ���ϴ� ��ġ�� Ŀ�� �̵�
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);// WIN32API �Լ��Դϴ�. �̰� ���ʿ� �����
}

/*�÷��̾� �׸���*/
void putplayer(int x, int y)
{
	gotoxy(x, y);
	printf("<^>");
}

/*�÷��̾� �����*/
void eraseplayer(int x, int y)
{
	gotoxy(x, y);
	printf("   ");
}

/*�޴����� Ŀ���̵�*/
void draw_menu_cursor(int x, int y)
{
	gotoxy(x, y);
	printf("��");
}
void erase_menu_cursor(int x, int y)
{
	gotoxy(x, y);
	printf("  ");
}

/*���� ������*/
void show_help() {
	cls(WHITE, BLACK);
	gotoxy(37, 1);
	printf("����\n");
	gotoxy(10, 5);
	printf("1. ����Ű�� ����, SPACE : ����, Z : Ư������");
	gotoxy(10, 7);
	printf("2. ���� óġ�ϸ� ������ ����ϴ�.");
	gotoxy(10, 9);
	printf("3. �������� ������ �ִ� 5�� �̻����� ����� �� �ֽ��ϴ�.");
	gotoxy(10, 11);
	printf("4. 3�� �浹�ϸ� ������ ����˴ϴ�.");
	gotoxy(10, 12);
	printf("5. ���� �ӵ��� ���� �������ϴ�.");
	gotoxy(25, 15);
	printf("�ƹ�Ű�� ������ �޴��� ���ư��ϴ�.");
	getch();
	cls(WHITE, BLACK);
}

/*���� ����*/
void show_quit()
{
	cls(WHITE, BLACK);
	gotoxy(30, 15);
	printf("-��������-");
	Sleep(500);
}

/*�ΰ� ������*/
void show_logo()
{
	int x1 = 9;
	int y1 = 3;
	int x2 = 69;
	int y2 = 11;

	gotoxy(9, 3);
	printf("%s", "��������������������������������");

	/*��° ��*/
	gotoxy(9, 4);
	printf("%s", "���");
	printf("\n");
	gotoxy(21, 4);
	printf("%s", "��");
	gotoxy(31, 4);
	printf("%s", "����");
	gotoxy(43, 4);
	printf("%s", "����");
	gotoxy(55, 4);
	printf("%s", "���");
	gotoxy(69, 4);
	printf("%s", "��");

	/*��° ��*/
	gotoxy(9, 5);
	printf("%s", "��");
	gotoxy(13, 5);
	printf("%s", "������");
	gotoxy(25, 5);
	printf("%s", "����");
	gotoxy(33, 5);
	printf("%s", "��");
	gotoxy(37, 5);
	printf("%s", "����");
	gotoxy(45, 5);
	printf("%s", "��");
	gotoxy(49, 5);
	printf("%s", "����");
	gotoxy(57, 5);
	printf("%s", "��");
	gotoxy(61, 5);
	printf("%s", "������");

	/*4��° ��*/
	gotoxy(9, 6);
	printf("%s", "��");
	gotoxy(13, 6);
	printf("%s", "������");
	gotoxy(25, 6);
	printf("%s", "����");
	gotoxy(33, 6);
	printf("%s", "��");
	gotoxy(37, 6);
	printf("%s", "����");
	gotoxy(45, 6);
	printf("%s", "��");
	gotoxy(49, 6);
	printf("%s", "������");
	gotoxy(61, 6);
	printf("%s", "������");

	/*5��° ��*/
	gotoxy(9, 7);
	printf("%s", "���");
	gotoxy(19, 7);
	printf("%s", "���");
	gotoxy(31, 7);
	printf("%s", "���");
	gotoxy(45, 7);
	printf("%s", "��");
	gotoxy(49, 7);
	printf("%s", "������");
	gotoxy(67, 7);
	printf("%s", "���");

	/*6��° ��*/
	gotoxy(9, 8);
	printf("%s", "������");
	gotoxy(21, 8);
	printf("%s", "��");
	gotoxy(25, 8);
	printf("%s", "������");
	gotoxy(37, 8);
	printf("%s", "����");
	gotoxy(45, 8);
	printf("%s", "��");
	gotoxy(49, 8);
	printf("%s", "������");
	gotoxy(61, 8);
	printf("%s", "������");

	/*7��° ��*/
	gotoxy(9, 9);
	printf("%s", "������");
	gotoxy(21, 9);
	printf("%s", "��");
	gotoxy(25, 9);
	printf("%s", "������");
	gotoxy(37, 9);
	printf("%s", "����");
	gotoxy(45, 9);
	printf("%s", "��");
	gotoxy(49, 9);
	printf("%s", "����");
	gotoxy(57, 9);
	printf("%s", "��");
	gotoxy(61, 9);
	printf("%s", "������");

	/*8��° ��*/
	gotoxy(9, 10);
	printf("%s", "��");
	gotoxy(19, 10);
	printf("%s", "���");
	gotoxy(25, 10);
	printf("%s", "������");
	gotoxy(37, 10);
	printf("%s", "����");
	gotoxy(45, 10);
	printf("%s", "���");
	gotoxy(55, 10);
	printf("%s", "���");
	gotoxy(69, 10);
	printf("%s", "��");

	/*������ ��*/
	gotoxy(9, 11);
	printf("%s", "��������������������������������");
	gotoxy(36, 12);
	printf("--------");
	gotoxy(36, 13);
	printf("Invaders");
	gotoxy(36, 14);
	printf("--------");
}

/*�޴� ������*/
void show_menu()
{
	gotoxy(36, 18);
	printf("1. START");
	gotoxy(36, 19);
	printf("2. HELP");
	gotoxy(36, 20);
	printf("3. QUIT");
}

/*�޴� ����(Ŀ���̵�)*/
int choice_menu()
{
	unsigned char ch;//����Ű ����
	unsigned char ent;//���� ����
	int menu_oldx = 34;
	int menu_oldy = 18;
	int menu_newx = 34;
	int menu_newy = 18;

	int menu_number = 0;

	draw_menu_cursor(menu_oldx, menu_oldy);

	while (1) {
		if (kbhit() == 1) {
			ch = getch();

			if (ch == SPECIAL1 || ch == SPECIAL2) {//�Է��� Ű�� Ư��key���?
				ch = getch();//Ű�� ����

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

/*�� �׸��� �˼� �̿�*/
void draw_map(int x1, int y1, int x2, int y2)
{
	int x;
	int y;

	for (x = x1; x <= x2; x = x + 2) {//���� 0ĭ���� 40ĭ(�ٵ� 0���ʹϱ� 41ĭ �׷�����)
		gotoxy(x, y1);
		printf("%s", "��");
		gotoxy(x, y2);
		printf("%s", "��");
	}

	for (y = y1; y <= y2; y++) {//���� 0ĭ���� 25ĭ����
		gotoxy(x1, y);
		printf("%s", "��");
		gotoxy(x2, y);
		printf("%s", "��");
	}

	gotoxy(x1, y1); printf("��");
	gotoxy(x1, y2); printf("��");
	gotoxy(x2, y1); printf("��");
	gotoxy(x2, y2); printf("��");
}

/*�� �����*/
void erase_enemy(int x, int y)
{
	gotoxy(x, y);
	printf("   ");
}

/*�� �׸���*/
void draw_enemy(int x, int y)
{
	gotoxy(x, y);
	printf("<|>");
}

/*�� ����(������ġ)*/
void make_enemy(int i)
{
	int r = rand() % 36 + 2;//2���� 37����
	int r2 = rand() % 36 + 2;

	Enemy[i].exist = 1;
	Enemy[i].hit_count = 0;

	Enemy[i].enewx = r;
	Enemy[i].eoldx = r;
	Enemy[i].enewy = 1;
	Enemy[i].eoldy = 1;
}

/*�� ������(�����ƴ�)*/
void move_enemy(int i)
{
	Enemy[i].enewy = Enemy[i].eoldy + 1;//���� �Ʒ��� �̵�

	erase_enemy(Enemy[i].eoldx, Enemy[i].eoldy);
	draw_enemy(Enemy[i].enewx, Enemy[i].enewy);

	Enemy[i].eoldx = Enemy[i].enewx;
	Enemy[i].eoldy = Enemy[i].enewy;
}

/*�� �����ϰ� ������ : �Ʒ��� ����, �¿�� �����ϰ� �����̸鼭��*/
void move_enemy_randomly(int i)
{
	int r_enewx;

	r_enewx = rand() % 3 - 1; //-1, 0, 1

	Enemy[i].enewx = Enemy[i].eoldx + r_enewx; //��, ��, �߾� ���� �ϳ� ����

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

/*�Ѿ˰� ���� �浹 �˻�*/
void check_enemy_hit(int b, int e) //b�� �Ѿ�, e�� ��
{
	if ((Enemy[e].enewx == Bullet[b].bx || Enemy[e].enewx + 1 == Bullet[b].bx || Enemy[e].enewx + 2 == Bullet[b].bx)
		&& Enemy[e].enewy == Bullet[b].by) {
		++Enemy[e].hit_count; //�浹�ߴٸ� �浹 Ƚ�� ����
		Bullet[b].exist = 0;
		erase_bullet(Bullet[b].bx, Bullet[b].by);
	}

	if (Enemy[e].hit_count == 3) { //�Ѿ��� ���� 3�� ������
		PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\bullet.wav"), NULL, SND_FILENAME | SND_ASYNC);
		Enemy[e].exist = 0; //�� ����
		erase_enemy(Enemy[e].enewx, Enemy[e].enewy);
		enemy_kill_count++; //�� óġ �� ����
	}
}

/*�÷��̾�� ���� �浹*/
void check_enemy_hit_player(int i)
{
	if ((newx - Enemy[i].enewx < 3 && newx - Enemy[i].enewx > -3) && newy == Enemy[i].enewy) { //���� �浹�ϸ�
		Enemy[i].exist = 0; //�浹 �� �� �����
		erase_enemy(Enemy[i].enewx, Enemy[i].enewy);
		player_hit_count++; //�÷��̾� hit_count����
	}
	if (player_hit_count == 3) { //3�� �浹�ϸ� ��������
		cls(WHITE, BLACK);
		gotoxy(35, 10);
		printf("���� : %d", score);
		gotoxy(35, 14);
		printf("���� ����");
		gotoxy(24, 19);
		printf("�ƹ�Ű�� ������ �޴��� ���ư��ϴ�");

		player_hit_count = 0; //��Ʈī��Ʈ �ʱ�ȭ
		eat_items_count = 0; //������ ���� Ƚ�� �ʱ�ȭ
		enemy_kill_count = 0; // �� óġ �� �ʱ�ȭ
		score = 0; //���� �ʱ�ȭ
		oldx = 10, oldy = 10;
		newx = 10, newy = 10;

		Sleep(1000);
		getch();
		getchar();//���� ����
		cls(WHITE, BLACK);
		main(); //�ƹ�Ű�� ������ �����Լ� �ٽ� ����
	}
}

/*�̻��ϰ� ���� �浹*/
void check_enemy_hit_missile(int b, int e)
{
	if ((Enemy[e].enewx + 2 >= Missile[b].mx && Enemy[e].enewx <= Missile[b].mx + 2) && Enemy[e].enewy == Missile[b].my) {
		Enemy[e].hit_count += 3; //�浹�ϸ� ��Ʈ ī��Ʈ 3����
	}

	if (Enemy[e].hit_count >= 3) {
		Enemy[e].exist = 0;
		erase_enemy(Enemy[e].enewx, Enemy[e].enewy);
		enemy_kill_count++;
	}
}

/*ü�� ���̱�*/
void show_hp(int player_hit_count)
{
	if (player_hit_count == 0) {
		printf("HP  ������");
	}
	else if (player_hit_count == 1) {
		printf("HP  ������");
	}
	else if (player_hit_count == 2) {
		printf("HP  ������");
	}
	else {
		printf("��������!!!!!");
	}
}

/*�Ѿ� �׸�*/
void draw_bullet(int x, int y)
{
	gotoxy(x, y);
	printf("!");
}

/*�Ѿ� �����*/
void erase_bullet(int x, int y)
{
	gotoxy(x, y);
	printf(" ");
}

/*�Ѿ� ����(������ġ)*/
void making_bullet(int i)
{
	Bullet[i].exist = 1;

	/*������ �Ѿ��� ������ġ*/
	Bullet[i].bx = newx + 1;
	Bullet[i].by = newy - 1;
	Bullet[i].boldx = newx + 1;
	Bullet[i].boldy = newy - 1;
}

/*�Ѿ� ������*/
void moving_bullet(int i)
{
	Bullet[i].by = Bullet[i].boldy - 1;//�Ѿ� ���� �̵�

	erase_bullet(Bullet[i].boldx, Bullet[i].boldy);
	draw_bullet(Bullet[i].bx, Bullet[i].by);

	Bullet[i].boldx = Bullet[i].bx;
	Bullet[i].boldy = Bullet[i].by;//���ο� �Ѿ���ǥ�� ���� �Ѿ���ǥ��� ����
}

/*������ ������*/
void show_items()
{
	int x, y;
	x = rand() % 37 + 2; //2���� 39����
	y = rand() % 22 + 1; //1���� 23����

	textcolor(BLACK, GREEN1);
	gotoxy(x, y);
	printf("P");
	items[x][y] = 1;
	items_x = x; //��ġ ���
	items_y = y; //��ġ ���
	textcolor(WHITE, BLACK);
}

/*������ �����*/
void erase_items(int x, int y) //�������� �ִ� ��ġ�� ���� ����
{
	gotoxy(x, y);
	printf(" ");
}

/*������ �Ծ����� �˻�*/
int check_eat_items()
{
	if (items_x >= newx && items_x <= newx + 2 && items_y == newy) { //������ �Ծ��ٸ�
		return 1; //1��ȯ
	}
	else 0; //�ȸԾ��ٸ� 0��ȯ
}

/*�̻��� �����*/
void erase_missile(int x, int y)
{
	gotoxy(x, y);
	printf("   ");
}

/*�̻��� �׸���*/
void draw_missile(int x, int y)
{
	gotoxy(x, y);
	printf("(!)");
}

/*�̻��� ����(������ġ)*/
void make_missile(int i)
{
	Missile[i].exist = 1;

	/*������ �̻����� ������ġ*/
	Missile[i].mx = newx;
	Missile[i].my = newy - 1;
	Missile[i].moldx = newx;
	Missile[i].moldy = newy - 1;
}

/*�̻��� ������*/
void move_missile(int i)
{
	Missile[i].my -= 1;

	erase_missile(Missile[i].moldx, Missile[i].moldy);
	draw_missile(Missile[i].mx, Missile[i].my);

	Missile[i].moldx = Missile[i].mx;
	Missile[i].moldy = Missile[i].my;//���ο� �̻�����ǥ�� ���� �̻�����ǥ��� ����
}

/*�÷��̾� ������*/
void moving_player(unsigned char ch)
{
	static unsigned char last_ch;

	if (keep_moving && ch == 0) {
		ch = last_ch;
	}
	last_ch = ch;

	switch (ch) {
	case UP:
		if (oldy > 1) {//���� ���μ��� �ε����� ������ �̵�
			newy = oldy - 1;//���� �̵�
		}
		break;
	case DOWN:
		if (oldy < HEIGHT - 2) {
			newy = oldy + 1;//�Ʒ��� �̵�
		}
		break;
	case LEFT:
		if (oldx > 2) {
			newx = oldx - 1;//���� �̵�
		}
		break;
	case RIGHT:
		if (oldx < WIDTH - 3) {
			newx = oldx + 1;//������ �̵�
		}
		break;
	}
	eraseplayer(oldx, oldy);
	putplayer(newx, newy);
	oldx = newx;
	oldy = newy;
}

/*��ü ���� ������ ����*/
void whole_game_movement()
{
	int run_time; //����ð�
	int start_time = time(NULL); //����ð�
	int enemy_time = 0; //������ �� �����ð� ���
	int enemy_speed_time = 0;
	int items_spawn_time = 0; //������ ����'�ð�' ���
	int items_time = 0; //������ ������ ����'�ð�' ���
	int items_run_time = 0; //�������� ������ �� �󸶳� ��������
	int items_time2 = 0;

	int t = 0; //������(0�̸� ������ ����, 1�̸� ����)
	int i = 1; //�Ѿ�
	int l = 1; //��
	int m = 0; //�̻���
	unsigned char ch; //Ű �Է� ��

	srand(time(NULL));

	while (1) {
		run_time = time(NULL) - start_time; //���� �ð� = ����ð� - ���۽ð�
		score = run_time + 10 * enemy_kill_count; //���� = ���� �ð� + �� ó�� ��

		/*������ 5�ʸ��� ����*/
		if (t == 0) { //�������� ���� ��
			if (run_time > items_time && (run_time % items_interval == 0)) { //10�ʿ� �ѹ��� ����
				items_spawn_time = run_time; //������ �����ð� ���
				show_items();
				items_time = run_time;
				t = 1; //�ʵ忡 �������� �ִٰ� ǥ��
			}
		}

		items_run_time = time(NULL) - items_spawn_time; //�������� �������� �󸶳� �ƴ���

		/*������*/
		if (t == 1) { //�������� ������ ��
			if (items_run_time > items_time2 && (items_run_time % 5 == 0)) { //�������� 5�ʰ� �Ǹ�
				t = 0; //�ʵ忡�ִ� ������ ���ٰ� ǥ��
				erase_items(items_x, items_y); //������ �����
				items_time2 = items_run_time;
			}

			/*������ �Ծ����� �˻�*/
			if (check_eat_items() == 1 && eat_items_count <= 5) { //���� �������� �Ծ��ٸ�
				PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\eat-items.wav"), NULL, SND_FILENAME | SND_ASYNC);
				erase_items(items_x, items_y); //�����������
				t = 0; //������ ���� ���Ѵٰ� ǥ��
				eat_items_count++; //������ ���� Ƚ�� ����
			}
		}

		/*�� 1�ʸ��� ����*/
		if (run_time > enemy_time && run_time % 1 == 0 && l < 11) { //1�ʸ���
			make_enemy(l); //�� ����
			enemy_time = run_time; //�������� 0�� �ð����� ��� �ݺ����� �ʵ��� �ϱ� ���ؼ�
			l++; //���� �� �������ؼ�
		}

		/*�� �ӵ��� 10�ʸ��� ������*/
		if (run_time > enemy_speed_time && (run_time % 10) == 0 && enemy_frame_sync > 3) {
			enemy_frame_sync--;
			enemy_speed_time = run_time;
		}

		if (l % 10 == 0) {//��
			l = 1;
		}
		if (i % 10 == 0) {//�Ѿ�
			i = 1;
		}
		if (m % 3 == 0) { //�̻���
			m = 1;
		}

		if (kbhit() == 1) {//Ű���尡 ���ȴٸ�?
			ch = getch();//���� Ű���� �� ����

			if (ch == 27) {//ESC������? ����
				gotoxy(55, 12);
				printf("-���� ����-\n");
				Sleep(10000);
				exit(0);
			}

			if (ch == SPECIAL1 || ch == SPECIAL2) {//�Է��� Ű�� Ư��key���?
				ch = getch();//Ű�� ����

				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					moving_player(ch);
					break;
				default://����Ű���� �ٸ��� ������ ��
					if (frame_count % p1_frame_sync == 0) {
						moving_player(0);
					}
				}
			}

			if (ch == SPACE && i < 11) { //�����̽��� ������ �Ѿ� ����, �ִ�10��
				if (Bullet[i].exist == 0) {
					making_bullet(i);
					i++; //���� �Ѿ� �����ϱ����ؼ�
				}
			}

			if (ch == z && eat_items_count >= 1 && m < 4) {
				if (Missile[m].exist == 0) {
					PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\laser-gun.wav"), NULL, SND_FILENAME | SND_ASYNC);
					eat_items_count--; //������ ī��Ʈ ����
					make_missile(m);
					m++;
				}
			}
		}

		else {//Ű���� �ȴ������� ��� �̵�

			/*�Ѿ� �˻�, �Ѿ� �̵�*/
			for (int j = 1; j < 11; j++) {
				if (Bullet[j].exist == 1) {
					if (frame_count % bullet_frame_sync == 0) {
						moving_bullet(j);
					}
					if (Bullet[j].by == 1) { //�Ѿ��� õ�忡 ������
						Bullet[j].exist = 0; //�Ѿ� ����
						gotoxy(Bullet[j].bx, Bullet[j].by);
						printf(" ");
					}
					/*�Ѿ��ϰ� ���ϰ� �浹 �˻�*/
					for (int k = 1; k < 11; k++) {
						if (Enemy[k].exist == 1) {
							check_enemy_hit(j, k); //j�� �Ѿ�, k�� ��
						}
					}
				}
			}

			/*�̻��� �˻�, �̵�*/
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
					/*�̻��ϰ� �� �浹�˻�*/
					for (int j = 1; j < 11; j++) {
						if (Enemy[j].exist == 1) {
							check_enemy_hit_missile(k, j); //k�� �̻���, j�� ��
						}
					}
				}
			}

			/*�� �˻�, ���� �̵�*/
			for (int k = 1; k < 11; k++) {
				if (Enemy[k].exist == 1) {
					if (frame_count % enemy_frame_sync == 0) {
						move_enemy_randomly(k);
					}
					if (Enemy[k].enewy == 23) { //���� �ٴڿ� ������
						Enemy[k].exist = 0; //�� ����
						gotoxy(Enemy[k].enewx, Enemy[k].enewy);
						printf("   ");
					}
					check_enemy_hit_player(k); //���̶� �÷��̾�� �浹
				}
			}

			/*�÷��̾� �̵�*/
			if (frame_count % p1_frame_sync == 0) {
				moving_player(0);
			}
		}

		gotoxy(50, 11);
		printf("�ð� : %d", run_time);

		gotoxy(50, 13);
		printf("���� : %d", score);

		gotoxy(50, 15);
		show_hp(player_hit_count);

		gotoxy(50, 17);
		printf("�� óġ �� : %d", enemy_kill_count);

		gotoxy(50, 19);
		printf("������ ��(�ִ� 5��) : %d", eat_items_count);
		Sleep(10);
		frame_count++;
	}
}

/*���� �Լ�*/
int main(void)
{
	int menu = 0;
start:
	PlaySound(TEXT("C:\\Users\\s9430\\source\\repos\\Space_Invader(version 3.0)\\spaceinvaders_sf\\opening-bgm.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	cls(WHITE, BLACK);
	show_logo();//���� �ΰ�
	show_menu();//�޴� ������
	menu = choice_menu();//�޴� ����(Ŀ�� ������)
	cls(WHITE, BLACK);//ȭ�� �� �����
	removeCursor();//Ŀ�� �����
	draw_map(0, 0, WIDTH, HEIGHT - 1);//WIDTH=40, HEIGHT=25, �� �׸���

	switch (menu) {
	case 1://���ӽ���
		PlaySound(0, 0, 0);
		whole_game_movement();
		break;
	case 2://���� ����
		PlaySound(0, 0, 0);
		show_help();
		goto start;//���� ������ �ٽ� �޴�â����
		break;
	case 3:
		show_quit();//��������
		break;
	}
	return 0;
}
