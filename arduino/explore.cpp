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

#define TASK_GO			0
#define TASK_FOLLOW_WALL	1
#define TASK_STUCK		2
#define TASK_LIGHT		3
#define TASK_EVADE_WALL		4
#define TASK_LDR		5

char ant_status[2];
int ldr0, ldr1, ldr0_max = 480, ldr0_stop = 860;
volatile int snr0;

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
	char id;
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
	tasks[tasks_i].state = t->state;
	tasks[tasks_i].id = t->id;

	tasks_i++;
}

void task_stop(char id)
{
	int i;

	for(i = 0; i <= tasks_i; i++)
	{
		if(tasks[i].id == id)
		{
			tasks[i].state = 0;
		}
	}
}

void task_reset()
{
	int i;

	for(i = 0; i <= tasks_i; i++)
	{
		tasks[i].state = 0;
	}
}

int last_task = -1;
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

	if((!tasks[j].state) && (tasks[j].f_start))
	{
		Serial.print("New task triggered ");
		Serial.print(j);
		tasks[j].state = 1;
		tasks[j].f_start();
	}
	else
	{
		if(!tasks[j].state)
		{
			Serial.print("New task triggered ");
			Serial.print(j);
			tasks[j].state = 1;
		}
		if(tasks[j].f_update) tasks[j].f_update();
	}
	if(last_task != j)
	{
		last_task = j;
		Serial.print("Task ");
		Serial.print(j);
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

#define ANG_LEFT	PI/2.0 * 1
#define ANG_RIGHT	-ANG_LEFT

#define WALL_INIT	0
#define WALL_CENTER	1
#define WALL_APPROACH	2
#define WALL_FOLLOW	3
#define WALL_FLYING	4

#define WALL_DISTANCE	75
#define MAX_FREE_WALL	200

/* Tiempo siguiendo una pared en ms */
#define MAX_FOLLOW_WALL	6000

char follow_wall_state = WALL_INIT;

/* Recorrido desde el último choque con la pared */
float free_wall_r = 0;

/* Recorrido durante la pared */
unsigned long follow_wall_t = 0;

void check_free_wall()
{
	if(robot.r - free_wall_r < MAX_FREE_WALL) return;

	Serial.print("Free wall.");
	
	// Girar hacia la pared
	if(wall_side == LEFT)
	{
		motors(0.5, 1);
	}
	else
	{
		motors(1, 0.5);
	}

	// Dejar de seguir la pared ?
	follow_wall_state = WALL_FLYING;
}

void check_follow_wall()
{
	if(millis() < follow_wall_t) return;

	Serial.print("Too long wall. STUCK? micros() = ");
	Serial.print(millis());
	Serial.print(", follow timeout = ");
	Serial.print(follow_wall_t);
	Serial.print(".");
	
	follow_wall_state = WALL_CENTER;
}

void check_corner()
{
	Serial.print("Corner detected.");
	
	follow_wall_state = WALL_CENTER;
}

void wall_flying_update()
{
	if(ant_status[LEFT] || ant_status[RIGHT])
	{
		follow_wall_state = WALL_CENTER;
	}
}

#define MIN_SNR0	250
/* La bombilla cerca con ldr0 = 860 */

void wall_follow_update()
{
	//TODO: Cuando se reestablece el estado?
	if(follow_wall_state != WALL_FOLLOW)
	{
		follow_wall_state = WALL_FOLLOW;
		free_wall_r = robot.r;
		follow_wall_t = millis() + MAX_FOLLOW_WALL;
	}
/*
	if((snr0 < MIN_SNR0) && (snr0 > 0))
	{
		Serial.print("sonar alert:");
		Serial.print(snr0);
		Serial.print(".");
		follow_wall_state = WALL_CENTER;
		return;
	}
*/
	if(wall_side == LEFT)
	{
		if(ant_status[wall_side])
		{
			motors(1, -0.5);
			//motors(-0.3, -1);
			check_follow_wall();
			free_wall_r = robot.r;
		}
		else
		{
			motors(0.8, 1);
			check_free_wall();
			follow_wall_t = millis() + MAX_FOLLOW_WALL;
		}
	}
	else
	{
		if(ant_status[wall_side])
		{
			motors(-0.5, 1);
			//motors(-1, -0.3);
			check_follow_wall();
			free_wall_r = robot.r;
		}
		else
		{
			motors(1, 0.8);
			check_free_wall();
			follow_wall_t = millis() + MAX_FOLLOW_WALL;
		}
	}

	// Esquina
	if(ant_status[INV(wall_side)])
	{
		follow_wall_state = WALL_CENTER;
		return;
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
	motion_straight(-WALL_DISTANCE, 0, wall_approach_rotate);
	//Actualizar motion straight
}

#define CENTERING_TIMEOUT	5000

unsigned long centering_timeout;

void check_centering_timeout()
{
	if(millis() < centering_timeout) return;

	// Evitar la esquina

	if(wall_side == LEFT)
	{
		motors(-1, 1);
	}
	else
	{
		motors(1, -1);
	}

}

void wall_approach_center()
{
	if(follow_wall_state != WALL_CENTER)
	{
		follow_wall_state = WALL_CENTER;
		centering_timeout = millis() + CENTERING_TIMEOUT;
	}

	if(ant_status[wall_side])
	{
		centering_timeout = millis() + CENTERING_TIMEOUT;
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
		check_centering_timeout();
	}
}


void set_forward()
{
	int i;
	dir = 1;
//	motors(0, 0);
//	delay(1000);
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

void update_snr()
{
	sonar_update(&snr0);
}

void update_ldr()
{
	ldr0 = analogRead(PIN_LDR0);
	ldr1 = analogRead(PIN_LDR1);
}

void task_go_forward()
{
	motion_straight(1, 1, NULL);
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
	else if(follow_wall_state == WALL_FLYING)
	{
		wall_flying_update();
	}
}

void task_stuck()
{
	float v = sqrt(pow(robot.vx, 2) + pow(robot.vy, 2));
	if(v > robot.vmax) robot.vmax = v;
	if((robot.mr != 0) && (robot.ml != 0))
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
	//task_stuck();
	//if(follow_wall_state != WALL_FLYING) return 0;

	//if(snr0 > 0 && snr0 < MIN_SNR0) return 1;
	return 0;
}

#define LIGHT_ANGLE	(45.0/360.0 * 2.0*PI)

#define LIGHT_INIT	0
#define LIGHT_STOP	1
#define LIGHT_360	2
#define LIGHT_360_MAX	3
#define LIGHT_GO	4
#define LIGHT_SWING_L	5
#define LIGHT_SWING_R	6
#define LIGHT_SWING_MAX	7
#define LIGHT_BLOCKED	10

int light_max;
float light_angle;

char light_state = LIGHT_INIT;

void light_end_360();
void light_end_centering();
void light_end_straight();
void light_end_swing_l();
void light_end_swing_r();

void light_end_360()
{
	light_state = LIGHT_360_MAX;
	if(light_angle < robot.theta - PI) light_angle += 2.0*PI;
	motion_rotate(light_angle, -1, 1, light_end_centering);
}

void light_end_centering()
{
	Serial.print("ldr0_max:");
	Serial.print(light_max);
	Serial.print(".");
	light_state = LIGHT_GO;
	motion_straight(100, 0, light_end_straight);
}

void light_end_straight()
{
	light_state = LIGHT_SWING_L;
	light_max = ldr1;
	light_angle = robot.theta;
	motion_rotate(robot.theta + LIGHT_ANGLE / 2.0, -1, 1, light_end_swing_l);
}

void light_end_swing_l()
{
	light_state = LIGHT_SWING_R;
	motion_rotate(robot.theta - LIGHT_ANGLE, -1, 1, light_end_swing_r);
}

void light_end_swing_r()
{
	light_state = LIGHT_SWING_MAX;
	motion_rotate(light_angle, -1, 1, light_end_centering);
}

void task_light_update()
{
	if(light_state == LIGHT_BLOCKED) return;

	if(light_state == LIGHT_INIT)
	{
		light_max = ldr1;
		light_angle = robot.theta;
		light_state = LIGHT_360;
		motion_rotate(robot.theta + 2*PI, -1, 1, light_end_360);
	}
	else if(light_state == LIGHT_360 || light_state == LIGHT_SWING_R)
	{
		if(light_max < ldr1)
		{
			light_max = ldr1;
			light_angle = robot.theta;
		}
	}
}

char task_light_trigger()
{
	if(follow_wall_state != WALL_INIT)
	{
		if(follow_wall_state != WALL_FLYING) return 0;
	}
	if(ldr0 > ldr0_max)
	{
		Serial.print("Light detected, ldr0:");
		Serial.print(ldr0);
		Serial.print(".");
		return 1;
		follow_wall_state = WALL_INIT;
		task_stop(TASK_FOLLOW_WALL);
	}
	return 0;
}

void unblock_follow_light()
{
	light_state = LIGHT_INIT;
	motors(0, 0);
	task_stop(TASK_EVADE_WALL);
}

char task_evade_wall_trigger()
{
	if(ldr0 <= ldr0_max) return 0;

	if(follow_wall_state != WALL_FOLLOW) return 0;

	Serial.print("Evading wall.");
	/* Separarse de la pared */
	task_stop(TASK_FOLLOW_WALL);
	task_stop(TASK_LIGHT);
	task_stop(TASK_STUCK);
	task_stop(TASK_GO);
	follow_wall_state = WALL_INIT;
	light_state = LIGHT_INIT;

	if(wall_side == LEFT)
	{
		motion_rotate(robot.theta - PI/2.0, 0, 1, unblock_follow_light);	
	}
	else
	{
		motion_rotate(robot.theta + PI/2.0, 0, 1, unblock_follow_light);
	}

	return 1;
}

#define SNR0_MIN_STOP	250

char task_ldr_trigger()
{
	if(ldr0 > ldr0_stop) return 1;
	if(ldr0 > ldr0_max)
	{
		if((snr0 > 0) && (snr0 < SNR0_MIN_STOP)) return 1;
		if(light_state == LIGHT_GO)
		{
			if(ant_status[LEFT] || ant_status[RIGHT]) return 1;
			return 0;
		}
	
	}

	return 0;
}
void task_ldr_start()
{
	Serial.print("Stopping ldr0:");
	Serial.print(ldr0);
	Serial.print(", snr0:");
	Serial.print(snr0);
	Serial.print(".");
	
	motors(0, 0);
	stop = 1;
}

void set_tasks()
{
	struct task_t t;

	/* Comenzar avanzando hacia delante */
	t.f_start = task_go_forward;
	t.f_trigger = NULL;
	t.f_update = NULL;
	t.prio = 1;
	t.state = 0;
	t.id = TASK_GO;
	task_add(&t);

	t.f_start = task_follow_wall;
	t.f_trigger = task_follow_wall_trigger;
	t.f_update = task_follow_wall_update;
	t.prio = 2;
	t.state = 0;
	t.id = TASK_FOLLOW_WALL;
	task_add(&t);

	t.f_start = task_stuck;
	t.f_trigger = task_stuck_trigger;
	t.f_update = task_stuck;
	t.prio = 10;
	t.state = 0;
	t.id = TASK_STUCK;
	task_add(&t);

	t.f_start = NULL;
	t.f_trigger = task_light_trigger;
	t.f_update = task_light_update;
	t.prio = 3;
	t.state = 0;
	t.id = TASK_LIGHT;
	task_add(&t);

	t.f_start = NULL;
	t.f_trigger = task_evade_wall_trigger;
	t.f_update = NULL;
	t.prio = 40;
	t.state = 0;
	t.id = TASK_EVADE_WALL;
	task_add(&t);

	t.f_start = task_ldr_start;
	t.f_trigger = task_ldr_trigger;
	t.f_update = NULL;
	t.prio = 20;
	t.state = 0;
	t.id = TASK_LDR;
	task_add(&t);

}

void config_ldr()
{
	do
	{
		update_ants();
		update_ldr();
		ldr0_max = ldr0;
	}
	while(!ant_status[LEFT]);

	Serial.print("ldr0_max:");
	Serial.print(ldr0_max);

	do
	{
		update_ants();
	}
	while(!ant_status[RIGHT]);

	do
	{
		update_ants();
		update_ldr();
		ldr0_stop = ldr0;
	}
	while(!ant_status[LEFT]);

	Serial.print("ldr0_stop:");
	Serial.print(ldr0_stop);

	do
	{
		update_ants();
	}
	while(!ant_status[RIGHT]);

	delay(50);
	do
	{
		update_ants();
	}
	while(ant_status[RIGHT]);
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
	pinMode(PIN_ANTL, INPUT);
	pinMode(PIN_ANTR, INPUT);

	Serial.begin(115200);
	Serial.print("########### Starting arduino ###########\n");

	odometry_init();
	sonar_init();

	do
	{
		update_ants();
	}
	while(!(ant_status[LEFT] || ant_status[RIGHT]));
	if(ant_status[LEFT]) config_ldr();

	delay(100);
	do
	{
		update_ants();
	}
	while((!ant_status[LEFT]) && (!ant_status[RIGHT]));
	delay(100);

	set_tasks();
	//task_go_forward();
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
	update_snr();
	update_ldr();
	motion_update();

	task_update();

	while(stop);
}
