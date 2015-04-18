#include <Arduino.h>
#include <serialmouse.h>
#include <odometry.h>
#include <motion.h>
#include <pinout.h>
#include <sonar.h>

char stop = 0; //End?
char dir = 1;

float fw_r = 400.0; // 40 cm
float fw_angle;
float fw_x;
float fw_start[2];

#define ERR_MAX	10
#define Kp 0.2 //200.0
#define Ki 0/ERR_MAX
#define KA -100
#define KX 5
#define M 1.0

#define FOLLOW_SLOW	0.5

#define LEFT	0
#define RIGHT	1
#define INV(x) (1-(x))

char ant_status[2];

float fw_error[ERR_MAX];
int fw_error_i;
float fw_int;
float fw_pro;
char _counter;

void set_backward();

struct task_t
{
	void (*f_start)(void);
	char (*f_trigger)(void);
	void (*f_update)(void);
	int prio;
	char state;
};

#define MAX_TASKS	10
struct task_t tasks[MAX_TASKS];
int tasks_i = 0;

char task_add(struct task_t *t)
{
	if(tasks_i + 1 >= MAX_TASKS) return -1;

	tasks[tasks_i].f_start = t->f_start;
	tasks[tasks_i].f_trigger = t->f_trigger;
	tasks[tasks_i].f_update = t->f_update;
	tasks[tasks_i].prio = t->prio;

	tasks_i++;
}

void task_update()
{
	int i, j = 0, max_prio = 0;

	for(i = 0; i <= tasks_i; i++)
	{
		if(tasks[i].state || (!tasks[i].f_trigger) || tasks[i].f_trigger())
		{
			if(tasks[i].prio > max_prio)
			{
				max_prio = tasks[i].prio;
				j = i;
			}
		}
	}

	if(!tasks[j].state)
	{
		Serial.print("New task triggered ");
		Serial.print(j);
		tasks[j].state = 1;
		tasks[j].f_start();
	}
	else
	{
		tasks[j].f_update();
	}
}

void set_follow_right()
{
	motors(1, FOLLOW_SLOW);
}
void set_follow_left()
{
	motors(0.5, 1);
}

char wall_side;

#define ANG_LEFT	PI/2.0 * 0.9
#define ANG_RIGHT	-ANG_LEFT

#define WALL_CENTER	0
#define WALL_APPROACH	1
#define WALL_FOLLOW	2
#define WALL_DISTANCE	75

char follow_wall_state = 0;

void wall_follow_update()
{
	//TODO: Cuando se reestablece el estado?
	follow_wall_state = WALL_FOLLOW;

	if(wall_side == LEFT)
	{
		if(ant_status[wall_side])
		{
			motors(1, 0.5);
		}
		else
		{
			motors(0.8, 1);
		}
	}
	else
	{
		if(ant_status[wall_side])
		{
			motors(0.5, 1);
		}
		else
		{
			motors(1, 0.8);
		}
	}
}
void wall_approach_rotate()
{
	if(wall_side == RIGHT)
		motion_rotate(robot.theta+ANG_LEFT, -1, 1, wall_follow_update);
	else
		motion_rotate(robot.theta+ANG_RIGHT, -1, 1, wall_follow_update);
}
void wall_approach_back()
{
	Serial.print("WB.");
	motion_straight(-WALL_DISTANCE, wall_approach_rotate);
	//Actualizar motion straight
}

void wall_approach_center()
{
	follow_wall_state = WALL_CENTER;
	if(ant_status[wall_side])
	{
		if(wall_side == LEFT)
		{
			motors(-1, 0);
		}
		else
		{
			motors(0, -1);
		}
	}
	else if(ant_status[INV(wall_side)])
	{
		follow_wall_state = WALL_APPROACH;
		wall_approach_back();
	}
	else
	{
		if(wall_side == LEFT)
		{
			motors(0.5, 1);
		}
		else
		{
			motors(1, 0.5);
		}
	}
}


void set_forward()
{
	int i;
	dir = 1;
	motors(0, 0);
	delay(1000);
	motors(M*1, M*1);
	fw_angle = 0;
	fw_x = 0;
	fw_start[0] = robot.rx;
	fw_start[1] = robot.ry;
	fw_error_i = 0;

	for(i = 0; i < ERR_MAX; i++)
	{
		fw_error[i] = 0.0;
	}
}

void update_error()
{
	int i;
	
	fw_pro = KX * (fw_x - robot.rx) + KA * dir * (fw_angle - robot.theta);

	fw_error[fw_error_i] = fw_pro;
	fw_error_i = (fw_error_i + 1) % ERR_MAX;
	fw_int = 0;

	for(i = 0; i < ERR_MAX; i++)
	{
		fw_int += fw_error[i];
	}
}

void update_forward()
{
	float ci, cp, c;
	if(robot.counter == _counter) return;

	// After odometry update

	// Check direction
	update_error();

	ci = fw_int * Ki;
	cp = fw_pro * Kp;
	c = ci + cp;

//	Serial.print("F.");
	/*
	Serial.print("F ci = ");
	Serial.print(ci*100.0);
	Serial.print("\tcp = ");
	Serial.print(cp*100.0);
	Serial.print("\tc = ");
	Serial.print(c*100.0);
//	Serial.print("\n");
*/

	if(c > 1) c = 1;
	if(c < -1) c = -1;

	if(c > 0)
		motors(M*(1), M*(1-c));
	else if(c < 0)
		motors(M*(1+c), M*(1));
}

void set_backward()
{
	int i;
	dir = -1;
	motors(0, 0);
	delay(1000);
	motors(M*(-1), M*(-1));
	fw_r = 400.0;
	fw_angle = 0;
	fw_x = 0;
	fw_start[0] = robot.rx;
	fw_start[1] = robot.ry;
	fw_error_i = 0;

	for(i = 0; i < ERR_MAX; i++)
	{
		fw_error[i] = 0.0;
	}
}

void update_backward()
{
	float c, ci, cp;
	if(robot.counter == _counter) return;

	// After odometry update

	// Check direction
	update_error();

	ci = fw_int * Ki;
	cp = fw_pro * Kp;
	c = ci + cp;

	Serial.print("B ci = ");
	Serial.print(ci*100.0);
	Serial.print("\tcp = ");
	Serial.print(cp*100.0);
	Serial.print("\tc = ");
	Serial.print(c*100.0);
//	Serial.print("\n");

	if(c > 1) c = 1;
	if(c < -1) c = -1;

	if(c > 0)
		motors(M*(-1), M*(-1+c));
	else if(c < 0)
		motors(M*(-1-c), M*(-1));


	// Check end
	if(robot.r > fw_r)
	{
		robot.r = 0;
		set_forward();
	}
}

void update_ants()
{
	ant_status[LEFT] = digitalRead(PIN_ANTL);
	ant_status[RIGHT] = digitalRead(PIN_ANTR);
}

void task_go_forward()
{
	set_forward();
}

void task_follow_wall()
{
	wall_approach_center();
}

char task_follow_wall_trigger()
{
	if(ant_status[LEFT])
	{
		wall_side = LEFT;
		return 1;
	}
	else if(ant_status[RIGHT])
	{
		wall_side = RIGHT;
		return 1;
	}
	return 0;
}

void task_follow_wall_update()
{
	if(follow_wall_state == WALL_FOLLOW)
	{
		wall_follow_update();
	}
	else if(follow_wall_state == WALL_CENTER)
	{
		wall_approach_center();
	}
}

void task_stuck()
{
	float v = sqrt(pow(robot.vx, 2) + pow(robot.vy, 2));
	if(v > robot.vmax) robot.vmax = v;
	if(robot.mr != 0 && robot.ml != 0)
	{
		if(v < 0.1 * robot.vmax)
		{
			Serial.print("STUCK!!!");
		}
		/*
		Serial.print("v=");
		Serial.print(v*1000.0);
		Serial.print(" vy=");
		Serial.print(robot.vy*1000.0);
		Serial.print(" vmax=");
		Serial.print(robot.vmax*1000.0);
		Serial.print("\n");*/
	}
}
char task_stuck_trigger()
{
	task_stuck();
	return 0;
}



void set_tasks()
{
	struct task_t t;

	/* Comenzar avanzando hacia delante */
	t.f_start = task_go_forward;
	t.f_trigger = NULL;
	t.f_update = update_forward;
	t.prio = 1;
	t.state = 1;
	task_add(&t);

	t.f_start = task_follow_wall;
	t.f_trigger = task_follow_wall_trigger;
	t.f_update = task_follow_wall_update;
	t.prio = 2;
	t.state = 0;
	task_add(&t);

	t.f_start = task_stuck;
	t.f_trigger = task_stuck_trigger;
	t.f_update = task_stuck;
	t.prio = 10;
	t.state = 0;
	task_add(&t);
}


////////////////////////////////////////////////////////////////////////////////

void setup()
{
	pinMode(PIN_ANTL, INPUT);
	pinMode(PIN_ANTR, INPUT);

	Serial.begin(115200);
	Serial.print("########### Starting arduino ###########\n");

	odometry_init();
//	sonar_init();

	set_tasks();
	task_go_forward();
}

void loop()
{
/*
	if(dir == 1)
		update_forward();
	else
		update_backward();
*/

/*
	if(robot.counter != _counter)
	{
		Serial.print("\trx ");
		Serial.print(robot.rx);
//		Serial.print("\try ");
//		Serial.print(robot.ry);
//		Serial.print("\tr ");
//		Serial.print(robot.r);
		Serial.print("\tvx ");
		Serial.print(robot.vx);
		Serial.print("\tvy ");
		Serial.print(robot.vy);
		Serial.print("\ttheta ");
		Serial.print(robot.theta);
		Serial.print("\n");
		_counter = robot.counter;
	}
*/

	update_ants();
	motion_update();

	task_update();

	while(stop);
}
