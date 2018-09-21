
/**
 *  @ Author Dat Nguyen
 *  @version 5/24/18
 */

#include <LedCube.h>
#include <EEPROM.h>


 /*Function prototype*/
void(*resetFunc) (void) = 0;


#define SWITCH_BUTTON A0 
#define NIGHT_BUTTON A1
const int STATE_ADDRESS = 0;
#define MAX_STATE 7
#define SIZE 3
#define COLS (SIZE*SIZE)
#define PWM 5
byte levelPins[SIZE] = { 11,12,13 };
byte colPins[COLS] = { 2,3,4,5,6,7,8,9,10 };
byte pwmPin[PWM] = { 3,5,6,9,10 };
#define MAX_PWM 255

LedCube cube(SIZE, levelPins, colPins);

//#define DEBUG
#ifdef DEBUG
#include <memdebug.h>
void showmem(const char label[] = "")
{
	char buffer[100];

	sprintf(buffer, "%s: %04u %04u %04u : used/free",
		label,
		getMemoryUsed(),
		//getFreeMemory()
		);

	Serial.println(buffer);
}
#endif

void setup()
{
	Serial.begin(9600);
	pinMode(SWITCH_BUTTON, INPUT_PULLUP);  // set pull-up on analog pin 0
	// check button state
	if (EEPROM.read(STATE_ADDRESS) > MAX_STATE)
		EEPROM.write(STATE_ADDRESS, 1);
	// configure state 

	int state = EEPROM.read(STATE_ADDRESS);
	Serial.print("current state: "); Serial.println(state);
	switch (state)
	{
	case 1:
		mainState();
		break;
	case 2:
		steady1();
		break;
	case 3:
		steady2();
		break;
	case 4:
		steady3();
		break;
	case 5:
		steady4();
		break;
	case 6:
		liveState();
		break;
	case 7:
		liveFast();
		break;
	default:
		Serial.println("invalid state");
		break;
	}
#ifdef DEBUG
	Serial.begin(9600);
#endif
}

void loop()
{
}



/*main state*/
void mainState() {

	// light indication option 1
	cube.lightLevel(2);
	while (1) {
		Serial.println("mainState");
#ifdef DEBUG
		showmem("start");
#endif

		byte array1[6] = { 0,3, 1,3, 2,3 };
		byte array2[6] = { 0,0, 1,0, 2,0 };
		byte array3[6] = { 0,1, 1,1, 2,1 };
		byte array4[6] = { 0,2, 1,2, 2,2 };
		byte array5[6] = { 0,5, 1,5, 2,5 };
		byte array6[6] = { 0,8, 1,8, 2,8 };
		byte array7[6] = { 0,7, 1,7, 2,7 };
		byte array8[6] = { 0,6, 1,6, 2,6 };

		checkButton();



		cubeFrame* f[] = {
		  cube.createFrame(array8, 6, 80),
		  cube.createFrame(array7, 6, 70),
		  cube.createFrame(array6, 6, 60),
		  cube.createFrame(array5, 6, 50),
		  cube.createFrame(array4, 6, 40),
		  cube.createFrame(array3, 6, 30),
		  cube.createFrame(array2, 6, 20),
		  cube.createFrame(array1, 6, 10),
		};
#ifdef DEBUG
		showmem("before free");
#endif
		cube.lightFrames(f, 8);
		checkButton();
		// light each light one at a time
		for (byte level = 0; level < cube.getLevels(); level++)
		{
			for (byte col = 0; col < cube.getCols(); col++)
			{
				cube.lightPulse(level, col, 100);
				checkButton();
			}
		}
		checkButton();
		// light one level at a time, increasing speed each time
		for (byte d = 25; d > 2; d -= 2)
		{
			for (byte l = 1; l <= cube.getLevels(); l++)
			{
				cube.lightLevel(l, d);
				checkButton();
			}
		}

		// light each row on each level
		for (byte level = 1; level <= cube.getLevels(); level++)
		{
			for (byte row = 1; row <= cube.getLevels() * 2; row++)
			{
				cube.lightRow(row, level);
				checkButton();
			}
		}

		// light each plane
		for (byte i = 3; i; i--)
		{
			for (byte row = 1; row <= cube.getLevels() * 2; row++)
			{
				cube.lightPlane(row, 10 * i);
				checkButton();
			}
		}

		// single random light at a time
		cube.randomLight(random(25, 100), 100);

		// random column drop
		for (byte x = 0; x <= 15; x++)
		{
			cube.lightDrop(random(0, cube.getCols()), random(50, 150));
			checkButton();
		}

		// circle around cube at a random level
		for (byte x = 0; x <= 5; x++)
		{
			cube.lightPerimeter(random(0, cube.getLevels()), random(1, 5), random(25, 100));
			checkButton();
		}
		checkButton();
		// light each face
		byte planes[] = { cube.getLevels() + 1,cube.getLevels(),cube.getLevels() * 2,1 };
		for (byte i = 5; i; i--)
		{
			for (byte p = 0; p < sizeof(planes); p++)
			{
				cube.lightPlane(planes[p], 5 * i);
				checkButton();
			}
		}

		// random columns
		cube.randomColumn(25);

		// turn off a single column randomly
		cube.enableBuffer();
		for (byte c = 0; c < 30; c++)
		{
			cube.fillBuffer();
			cube.invertBuffer();
			cube.randomColumn();
			cube.drawBuffer(7);
			checkButton();
		}
		cube.enableBuffer(false);

		// cols in and out
		for (byte c = 1, d = 0; c <= 10; c++)
		{
			if (c % 2 == 0)
			{
				for (d = 0; d < 20; d++)
				{
					cube.lightColumn(2, 1);
					cube.lightColumn(4, 1);
					cube.lightColumn(6, 1);
					cube.lightColumn(8, 1);
					checkButton();
				}
			}
			else if (c % 4 == 1)
			{
				for (d = 0; d < 30; d++)
				{
					cube.lightColumn(1, 1);
					cube.lightColumn(3, 1);
					cube.lightColumn(7, 1);
					cube.lightColumn(9, 1);
					checkButton();
				}
			}
			else
			{
				for (d = 0; d < 70; d++)
				{
					cube.lightColumn(5, 1);
					checkButton();
				}
			}
		}

		// diamond and box
		byte diamond[] = { 0,4,  1,1, 1,3, 1,4, 1,5, 1,7,   2,4 };
		byte box[] = {
		  2,0, 2,1, 2,2, 2,3, 2,5, 2,6, 2,7, 2,8,
		  1,0, 1,2, 1,6, 1,8,
		  0,0, 0,1, 0,2, 0,3, 0,5, 0,6, 0,7, 0,8
		};
		cube.lightSequence(box, sizeof(box), 200);
		cube.lightSequence(diamond, sizeof(diamond), 400);

		// helicopter effect
		byte topSeq[8] = { 0,3,6,7,8,5,2,1 };
		byte botSeq[8] = { 8,5,2,1,0,3,6,7 };
		for (byte loops = 0, delay = 50; loops <= 8; loops++)
		{
			checkButton();
			for (byte s = 0; s < 8; s++)
			{
				byte seq[] = { 2,topSeq[s], 1,4, 0,botSeq[s] };
				cube.lightSequence(seq, sizeof(seq), delay);
				checkButton();
			}
			if (loops < 5) delay -= 10;  else delay += 10;
			checkButton();
		}
		checkButton();
		// turn off one light at a time
		cube.lightsOut();
	}

}

/*steady1() state*/
void steady1() {

	// light indication option 2
	cube.lightLevel(2);

	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
	digitalWrite(13, HIGH);
	for (int i = 0; i < PWM; i++) {
		analogWrite(pwmPin[i], MAX_PWM);
	}
	while (1) {
		Serial.println("steady1()");
		delay(100);
		checkButton();
	}
}

/*steady2() state*/
void steady2() {
	// light indication option 3
	cube.lightLevel(2);

	// turn on level gate
	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
	digitalWrite(13, HIGH);
	for (int i = 0; i < PWM; i++) {
		analogWrite(pwmPin[i], MAX_PWM*(2.0 / 3.0) - 20);
	}
	while (1) {
		Serial.println("steady2()");
		delay(100);
		checkButton();
	}
}

/*steady3() state*/
void steady3() {

	// light indication option 3
	cube.lightLevel(2);

	// turn on level gate
	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
	digitalWrite(13, HIGH);
	for (int i = 0; i < PWM; i++) {
		analogWrite(pwmPin[i], MAX_PWM*(1.0 / 3.0) - 50);
	}
	while (1) {
		Serial.println("steady3()");
		delay(100);
		checkButton();
	}
}// end steady3


/*steady4() state*/
void steady4() {

	// light indication option 3
	cube.lightLevel(2);

	// turn on level gate
	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
	digitalWrite(13, HIGH);
	for (int i = 0; i < PWM; i++) {
		analogWrite(pwmPin[i], 10);
	}
	while (1) {
		Serial.println("steady4()");
		delay(100);
		checkButton();
	}
}// end steady3

/*liveState()
*This state light will fade and bright sequentially
*/
void liveState() {
	// light indication option 3
	cube.lightLevel(2);

	// turn on lv gate
	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
	digitalWrite(13, HIGH);
	int val = 0;
	bool fadeUp = true;
	Serial.println("Live state");
	while (1) {
		if (fadeUp) val++;
		else val--;
		//for (int val = live; val < MAX_PWM; val++) {
		for (int i = 0; i < PWM; i++) {
			analogWrite(pwmPin[i], val);
		}
		checkButton();
		if (val <= 60) delay(200);
		else delay(10);
		//}
		if (val == 0 || val == 255)
			fadeUp = !fadeUp;
	}
}

/*liveState()
*This state light will fade and bright sequentially
*/
void liveFast() {

	// light indication option 3
	cube.lightLevel(2);

	// turn on lv gate
	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
	digitalWrite(13, HIGH);
	int val = 0;
	bool fadeUp = true;
	Serial.println("Live state");
	while (1) {
		if (fadeUp) val++;
		else val--;
		//for (int val = live; val < MAX_PWM; val++) {
		for (int i = 0; i < PWM; i++) {
			analogWrite(pwmPin[i], val);
		}
		checkButton();
		if (val <= 60) delay(30);
		else delay(10);
		//}
		if (val == 0 || val == 255)
			fadeUp = !fadeUp;
	}
}

/**This function check the state of the pin A0. (button pin)*/
void checkButton() {
	if (digitalRead(SWITCH_BUTTON) == LOW) {
		EEPROM.write(STATE_ADDRESS, EEPROM.read(STATE_ADDRESS) + 1);
		resetFunc();
	}
}