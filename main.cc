/*
TODO
user should select name of custom gates
create user interface for adding and saving gates
create option to combine multiple gates to one, have more than one outputs (for creating multi bit gates like adders)
fix the bug where adding a new gate makes the lines of connections act unexpectedly
add error handling everywhere
handle edge cases and remove temp fixes (like setting the gate vector capacity to 100)
*/

#include <iterator>
#include <stack>
#include <iostream>
#include <raylib.h>
#include <vector>
#include <string>

#define WIDTH 500
#define HEIGHT 400
#define GATE_WIDTH 40
#define GATE_HEIGHT 20
#define PIN_RADIUS 5

using namespace std;

enum component {
	GATE, PIN, TOGGLE
};

struct gate;

typedef struct 
{
	bool val = 0;
	int x, y;
	struct gate *g = NULL;

	void init()
	{
		x = 50, y = 50;
	}

	void draw() 
	{
		if (val) DrawCircle(x, y, PIN_RADIUS, GREEN);
		else DrawCircle(x, y, PIN_RADIUS, RED);
	}
} o_pin;

typedef struct 
{
	o_pin *con = NULL;
	bool val = 0;
	int x, y;

	void init()
	{
		x = WIDTH-50, y = 50;
	}

	void draw() 
	{
		if (con != NULL) val = con->val;
		if (val) DrawCircle(x, y, PIN_RADIUS, GREEN);
		else DrawCircle(x, y, PIN_RADIUS, RED);
		if (con != NULL) {
			DrawLine(x, y, con->x, con->y, RED);
		}
	}
} i_pin;

typedef struct gate
{
	vector <i_pin> ip;
	o_pin op;
	int x, y;
	string name = "", logic = "";

	void init()
	{
		ip.resize(2);
		x = 100;
		y = 100;
		op.g = this;
		move();
		for (int i = 0 ; i < ip.size() ; i++) ip[i].con = NULL;
	}

	void move()
	{
		for (int i = 0 ; i < ip.size() ; i++) {
			ip[i].x = x;
			ip[i].y = y + GATE_HEIGHT*i/(ip.size()-1);
		}
		op.x = x + GATE_WIDTH;
		op.y = y + GATE_HEIGHT/2;
	}

	void tick()
	{
		evaluate_logic();
	}

	void draw() 
	{
		DrawRectangle(x, y, GATE_WIDTH, GATE_HEIGHT, WHITE);
		DrawText(name.c_str(), x, y, GATE_HEIGHT, BLACK);
		for (int i = 0 ; i < ip.size() ; i++) {
			ip[i].draw();
		}
		op.draw();
	}

	void evaluate_logic()
	{
		stack <bool> s;
		bool a, b;
		for (int i = 0 ; i < logic.size() ; i++) {
			if (isalpha(logic[i])) {
				if (ip[logic[i] - 'a'].con != NULL) s.push(ip[logic[i] - 'a'].con->val);
				else s.push(0);
			} else if (logic[i] == '!') {
				a = s.top();
				s.pop();
				s.push(!a);
			} else {
				a = s.top();
				s.pop();
				b = s.top();
				s.pop();
				if (logic[i] == '|') s.push(a|b);
				else if (logic[i] == '&') s.push(a&b);
			}
		}
		op.val = s.top();
	}

	void debug()
	{
		cout << "ADD : " << this << endl;
		cout << "ADD : " << this << endl;
		cout << "ADD : " << this << endl;
		cout << "ADD : " << this << endl;
		cout << "IP1 : " << this->ip[0].con << endl;
		cout << "IP2 : " << this->ip[1].con << endl;
		cout << "NAME: " << this->name << endl;
		cout << "LOGC: " << this->logic << endl;
		cout << endl;
	}
} gate;

typedef struct
{
	string name;
	string logic;
} gate_type;

vector <gate_type> types;

void load_gate_types()
{
	FILE *f = fopen("comp", "r");

	gate_type g;
	string s;
	char c;
	while ((c = fgetc(f)) != EOF) {
		if (c == ' ') {
			g.name = s;
			s = "";
		} else if (c == '\n') {
			g.logic = s;
			s = "";
			types.push_back(g);
		} else s += c;
	}
}

int cur = 0;

void draw(vector <auto> obj) 
{
	for (int i = 0 ; i < obj.size() ; i++) {
		obj[i].draw();
	}
}

bool mouse_in_o_pin(o_pin op)
{
	int x = GetMouseX(), y = GetMouseY();
	if (x >= op.x-PIN_RADIUS && x <= op.x+PIN_RADIUS && y >= op.y-PIN_RADIUS && y <= op.y+PIN_RADIUS) return 1;
	return 0;
}

void connect_i_pin(gate *g, o_pin *selected)
{
	int x = GetMouseX(), y = GetMouseY();
	for (int i = 0 ; i < g->ip.size() ; i++) if (x >= g->ip[i].x-PIN_RADIUS && x <= g->ip[i].x+PIN_RADIUS && y >= g->ip[i].y-PIN_RADIUS && y <= g->ip[i].y+PIN_RADIUS) {
		g->ip[i].con = selected;
		return;
	}

}
void connect_i_pin(i_pin *ip, o_pin *selected)
{
	int x = GetMouseX(), y = GetMouseY();
	if (x >= ip->x-PIN_RADIUS && x <= ip->x+PIN_RADIUS && y >= ip->y-PIN_RADIUS && y <= ip->y+PIN_RADIUS) {
		ip->con = selected;
	}
}

bool mouse_in_gate(gate g)
{
	int x = GetMouseX(), y = GetMouseY();
	if (x >= g.x && x <= g.x+GATE_WIDTH && y >= g.y && y <= g.y+GATE_HEIGHT) return 1;
	return 0;
}

string compile(gate *g, o_pin *t)
{
	string logic = g->logic;
	vector <string> s(2);
	for (int i = 0 ; i < g->ip.size() ; i++) {
		if (g->ip[i].con == NULL) {
    		cout << "ERROR : open input " << i << " on gate " << g->name << endl;
			continue;
		} else if (g->ip[i].con->g == NULL) {
			s[i] = distance(g->ip[i].con, t) + 'a';
		} else {
			s[i] = compile(g->ip[i].con->g, t);
		}
	}

	for (int i = 0 ; i < logic.size() ; i++) {
		if (isalpha(logic[i])) {
				string r = s[logic[i] - 'a'];
				logic.replace(i, 1, r);
			i += r.size() - 1;
		}
	}
	return logic;
}
void add_gate(string s)
{
	FILE *f = fopen("comp", "a");
	fprintf(f, "\nCUS%d %s", types.size()+1, s.c_str());
	fclose(f);
}

int main()
{
	vector <gate> gates;
	vector <o_pin> toggles;
	vector <i_pin> bulbs;
	void *selected = NULL;
	component drag = GATE;
	gates.reserve(100);
	toggles.reserve(100);

//	temp
	toggles.push_back({});
	toggles[0].init();
	bulbs.push_back({});
	bulbs[0].init();

	load_gate_types();

	SetTargetFPS(60);
	InitWindow(WIDTH, HEIGHT, "lgc");

	while (!WindowShouldClose()) {
		for (int i = 0 ; i < gates.size() ; i++) gates[i].evaluate_logic();
		if (selected == NULL && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			for (int i = 0 ; i < gates.size() ; i++) {
				if (mouse_in_o_pin(gates[i].op)) {
					selected = &(gates[i].op);
					drag = PIN;
					break;
				} else if (mouse_in_gate(gates[i])) {
					selected = &(gates[i]);
					drag = GATE;
					break;
				}
			} 
		}
		if (selected == NULL) {
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				for (int i = 0 ; i < toggles.size() ; i++) if (mouse_in_o_pin(toggles[i])) {
					selected = &(toggles[i]);
					drag = PIN;
				}
			}
			if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
				for (int i = 0 ; i < toggles.size() ; i++) if (mouse_in_o_pin(toggles[i])) {
					toggles[i].val = !toggles[i].val;
					break;
				}
			}
		}

			
		if (IsKeyPressed(KEY_SPACE)) {
			add_gate(compile(bulbs[0].con->g, toggles.data()));
		}
		if (IsKeyPressed(KEY_RIGHT)) cur = (cur+1)%types.size();
		if (IsKeyPressed(KEY_ENTER)) {
			gates.push_back({});
			gates.rbegin() -> init();
			gates.rbegin() -> name = types[cur].name;
			gates.rbegin() -> logic = types[cur].logic;
		}

		if (selected != NULL) {
			int mx = GetMouseX();
			int my = GetMouseY();
			if (drag == GATE) {
				((gate *) selected)->x = mx;
				((gate *) selected)->y = my;
				((gate *) selected)->move();
				if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) selected = NULL;
			} else if (drag == PIN) {
				int x = ((o_pin *) selected)->x, y = ((o_pin *) selected)->y;
				DrawLine(x, y, mx, my, RED);
				if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
					for (int i = 0 ; i < gates.size() ; i++) {
						connect_i_pin(&(gates[i]), (o_pin *)selected);
					}
					for (int i = 0 ; i < bulbs.size() ; i++) {
						connect_i_pin(&bulbs[i], (o_pin *)selected);
					}
					selected = NULL;
				}
			}
		}	
	
		BeginDrawing();

		ClearBackground(BLACK);
		draw(gates);
		draw(toggles);
		draw(bulbs);
		DrawText(types[cur].logic.c_str(), 10, HEIGHT-30, 20, BLUE);

		EndDrawing();
	}

	CloseWindow();
}
