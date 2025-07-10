#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <string.h> 

typedef struct
{
	int i1, i2;
	int x, y;
	char *name;
	int typ;
} gate;

typedef struct
{
	int x, y;
	int in;
	bool value;
} bulb;

typedef struct
{
	int x, y;
	bool value;
} swtc;

gate g[8];
swtc s[2] = { {50, 100, 1}, {50, 300, 0} };

FILE *f;

bool mouse_collision(int x, int y, int w, int h)
{
	if(GetMouseX()>=x && GetMouseX()<=x+w && GetMouseY()>=y && GetMouseY()<=y+h) return 1;
	return 0;
}

int mouse_in(gate g)
{
	if(mouse_collision(g.x, g.y, 50, 24)) {
		return 0;
	}
	else if(mouse_collision(g.x+50, g.y+5, 12, 12)) {
		return 1;
	}
	if(g.name[0] == 'n') {
		if(mouse_collision(g.x-8, g.y+8, 8, 8)) return 2;
	} else {
		if(mouse_collision(g.x-8, g.y, 8, 8)) {
			return 2;
		}
		else if(mouse_collision(g.x-8, g.y+16, 8, 8)) {
			return 3;
		}
	}
	return -1;
}

void draw_gate(gate gt)
{
	DrawRectangle(gt.x, gt.y, 50, 24, WHITE);
	DrawRectangle(gt.x+50, gt.y+6, 12, 12, RED);
	if(gt.name[0]=='n') DrawRectangle(gt.x-8, gt.y+8, 8, 8, BLUE);
	else {
		DrawRectangle(gt.x-8, gt.y, 8, 8, BLUE);
		DrawRectangle(gt.x-8, gt.y+16, 8, 8, BLUE);
	}
	DrawText(gt.name, gt.x+5, gt.y+5, 15, BLACK);
	if(gt.name[0]=='n') {
		if(gt.i1>=0 && gt.i1<8) DrawLine(gt.x-4, gt.y+12, g[gt.i1].x+50+6, g[gt.i1].y+12, YELLOW);
		else if(gt.i1>=8 && gt.i1<16) DrawLine(gt.x-4, gt.y+12, s[gt.i1-8].x, s[gt.i1-8].y, BLUE);
	} else {
		if(gt.i1>=0 && gt.i1<8) DrawLine(gt.x-4, gt.y+4, g[gt.i1].x+50+6, g[gt.i1].y+12, YELLOW);
		else if(gt.i1>=8 && gt.i1<16) DrawLine(gt.x-4, gt.y+4, s[gt.i1-8].x, s[gt.i1-8].y, BLUE);
		if(gt.i2>=0 && gt.i2<8) DrawLine(gt.x-4, gt.y+20, g[gt.i2].x+50+6, g[gt.i2].y+12, YELLOW);
		else if(gt.i2>=8 && gt.i2<16)DrawLine(gt.x-4, gt.y+20, s[gt.i2-8].x, s[gt.i2-8].y, BLUE);
	}
}

void draw_swtc(swtc sw)
{
	if(sw.value == 0) DrawCircle(sw.x, sw.y, 20, GetColor(0x000066ff));
	else if(sw.value == 1) DrawCircle(sw.x, sw.y, 20, GetColor(0x0000ffff));
}

bool outp(int i)
{
	int j;
	if(i==-1) return 0;
	else if(i<8) {
		for(j=g[i].typ; j>0; j--) {
			while(fgetc(f) != '\n');
		}
//		while((j=fgetc(f)) != EOF) printf("%d : %c\n", j, j);


	} else if(i<16) {
		return s[i-8].value;
	}
}

void mkgt(int i)
{
	if(i==-1) return;
	else if(i<8) {
		if(g[i].name[0]=='a') {
			fprintf(f, "&");
			mkgt(g[i].i1);
			mkgt(g[i].i2);
		}
		else if(g[i].name[0]=='o') {
			fprintf(f, "|");
			mkgt(g[i].i1);
			mkgt(g[i].i2);
		}
		else if(g[i].name[0]=='n') {
			fprintf(f, "!");
			mkgt(g[i].i1);
		}
	} else if(i<16) {
		fprintf(f, "%c", i-8+'a');
	}
}


int main()
{
	int selected = -1;
	bulb b = {500, 200, -1};
	int gtnum = 0;

	InitWindow(600, 400, "po");
	while(!WindowShouldClose())
	{
		if(gtnum<8) {
			if(IsKeyPressed(KEY_ONE)) g[gtnum++] = (gate){-1, -1, 200, 100, "and", 0};
			else if(IsKeyPressed(KEY_TWO)) g[gtnum++] = (gate){-1, -1, 200, 100, "or", 1};
			else if(IsKeyPressed(KEY_THREE)) g[gtnum++] = (gate){-1, -1, 200, 100, "not", 2};
		}

		if(IsKeyPressed(KEY_SPACE)) {
			f = fopen("gates", "a");
			mkgt(b.in);
			fprintf(f, "\n");
			fclose(f);
		}

		// drop
		if(selected != -1)
		{
			if(selected < 8) {
				g[selected].x = GetMouseX() - 25;
				g[selected].y = GetMouseY() - 12;
			} else if(selected < 16) {
				for(int i=0; i<gtnum; i++) {
					if(mouse_in(g[i])>1 && IsMouseButtonReleased(0)) {
						if(mouse_in(g[i])==2) g[i].i1 = selected-8;
						else if(mouse_in(g[i])==3) g[i].i2 = selected-8;
					}
				}
				if(CheckCollisionPointCircle(GetMousePosition(), (Vector2){500, 200}, 50) && IsMouseButtonReleased(0)) {
					b.in = selected-8;
				}
				else if(selected-8 == b.in) {
					b.in = -1;
				}
			} else if(selected>=32 && selected<40) {
				for(int i=0; i<gtnum; i++) {
					if(mouse_in(g[i])>1 && IsMouseButtonReleased(0)) {
						if(mouse_in(g[i])==2) g[i].i1 = selected-24;
						else if(mouse_in(g[i])==3) g[i].i2 = selected-24;
					}
				}
			}
		}

		// drag
		if(IsMouseButtonDown(0)) {
			for(int i=0; i<gtnum; i++) {
				int c;
				if(selected == -1 && (c = mouse_in(g[i])) != -1) { 
					selected = 8*c+i;
					break;
				}
			}
			for(int i=0; i<2; i++) {
				if(selected == -1 && CheckCollisionPointCircle(GetMousePosition(), (Vector2){s[i].x, s[i].y}, 20) && IsMouseButtonDown(0)) {
					selected = 32+i;
				}
			}
		} 
		else selected = -1;

		if(IsMouseButtonPressed(1)) {
			for(int i=0; i<2; i++) {
				if(CheckCollisionPointCircle(GetMousePosition(), (Vector2){s[i].x, s[i].y}, 20)) {
					s[i].value = !s[i].value;
				}
			}
		}

		if(b.in!=-1) {
			f = fopen("gates", "r");
			b.value = outp(b.in);
			fclose(f);
		}
		BeginDrawing();
		ClearBackground(BLACK);
		for(int i=0; i<gtnum; i++) {
			draw_gate(g[i]);
		}
		for(int i=0; i<2; i++) {
			draw_swtc(s[i]);
		}

		if(selected >= 8 && selected < 16) DrawLine(g[selected-8].x+50+6, g[selected-8].y+12, GetMouseX(), GetMouseY(), RED);
		else if(selected >= 32 && selected < 40) DrawLine(s[selected-32].x, s[selected-32].y, GetMouseX(), GetMouseY(), RED);

		if(b.value == 0) DrawCircle(500, 200, 50, GetColor(0x800000ff));
		else DrawCircle(500, 200, 50, GetColor(0xff0000ff));

		if(b.in!=-1) {
			DrawLine(500, 200, g[b.in].x+50, g[b.in].y+12, RED);
		}
		EndDrawing();
	}
	CloseWindow();

	return 0;
}
