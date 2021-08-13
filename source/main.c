// Simple citro2d untextured shape example
#include <citro2d.h>
#include <3ds.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

#define MAX_DPAD_VALUE 155

// Would be cool if they would be even, dunno what happens if they are odd

#define SHUTTLE_HEIGHT 40
#define SHUTTLE_WIDTH 4 

#define MAX_SPEED 5

float br = 5.0; // Ballradius
int score[] = {0,0};

float bx =  (SCREEN_WIDTH / 2); // Ball X
float by =  (SCREEN_HEIGHT / 2); // Ball Y
float db[2] = {0.0f,0.0f}; // Ball velocity

// Initialize Playerpositions
int p1p = (int) (SCREEN_HEIGHT / 2); 
int p2p = (int) (SCREEN_HEIGHT / 2);

bool startFlag = false;

// Function Definitions
int updatePlayerPos(int pos, float dy);
bool isNewPosOutOfBounds(int newPos);
void updateBallPosition();
void resetBallPosition(bool lastPlayerScored);

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	//Matrix containing the name of each key. Useful for printing when a key is pressed
	char keysNames[32][32] = {
		"KEY_A", "KEY_B", "KEY_SELECT", "KEY_START",
		"KEY_DRIGHT", "KEY_DLEFT", "KEY_DUP", "KEY_DDOWN",
		"KEY_R", "KEY_L", "KEY_X", "KEY_Y",
		"", "", "KEY_ZL", "KEY_ZR",
		"", "", "", "",
		"KEY_TOUCH", "", "", "",
		"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
		"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
	};
	time_t t;
	srand((unsigned) time(&t));

	// Init libs
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Create colors
	u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
	u32 clrGreen = C2D_Color32(0x00, 0xFF, 0x00, 0xFF);
	u32 clrRed   = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
	u32 clrBlue  = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);

	u32 ballClr = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);

	u32 clrTri1 = C2D_Color32(0xFF, 0x15, 0x00, 0xFF);
	u32 clrTri2 = C2D_Color32(0x27, 0x69, 0xE5, 0xFF);

	u32 clrClear = C2D_Color32(0xFF, 0xD8, 0xB0, 0x68);

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		circlePosition pos;

		hidCircleRead(&pos);

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
		if (kDown & KEY_A & !startFlag) {
			startFlag = true;
			resetBallPosition(false);
		}
	
		printf("\x1b[1;1HPong for 3DS");
		printf("\x1b[2;1HScore: %i - %i\x1b[K", score[0], score[1]);
		printf("\x1b[3;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[4;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[5;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage()*100.0f);
		printf("\x1b[6;1Hdx:  %f\x1b[K", db[0]);
		printf("\x1b[7;1Hdy:  %f\x1b[K", db[1]);

		if (!startFlag) {continue;}

		p1p = updatePlayerPos(p1p, pos.dy); // update P1 position
		p2p = by; // update P2 Position

		//Update Ball position
		updateBallPosition();

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, clrClear);
		C2D_SceneBegin(top);

		C2D_DrawCircleSolid(bx, by, 0, br, ballClr); // Draw ball
		C2D_DrawRectSolid(0, p1p - (SHUTTLE_HEIGHT / 2), 0, SHUTTLE_WIDTH, SHUTTLE_HEIGHT, clrBlue); // Draw Player 1
		C2D_DrawRectSolid(SCREEN_WIDTH - SHUTTLE_WIDTH, p2p - (SHUTTLE_HEIGHT / 2), 0, SHUTTLE_WIDTH, SHUTTLE_HEIGHT, clrBlue); // Draw Player 2

		
		C3D_FrameEnd(0);
	}

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}

int updatePlayerPos(int pos, float dy) {
	int newPos = (int) (-(dy / MAX_DPAD_VALUE) * MAX_SPEED);

	if (isNewPosOutOfBounds(pos + newPos))
		return pos;
	else
		return pos + newPos;
}

bool isNewPosOutOfBounds(int newPos) {
	if (newPos < SHUTTLE_HEIGHT / 2 || newPos > SCREEN_HEIGHT - SHUTTLE_HEIGHT / 2)
		return true;
	else
		return false;
}

void updateBallPosition() {
	float nextX = bx + db[0];
	float nextY = by + db[1];

	// If top or bottom Hit
	if (nextY > SCREEN_HEIGHT - br)
		db[1] = -1 * abs(db[1]);
	if (nextY < br)
		db[1] = abs(db[1]);

	// If paddle hit
	if (nextX <= SHUTTLE_WIDTH && abs(p1p - by) <= SHUTTLE_HEIGHT / 2 && nextX > 0) {
		db[0] *=-1.1;
		
		if (db[1] == 0)
			db[1] = (abs(p1p - by) / (float) (SHUTTLE_HEIGHT / 2)) + 1;
		else
			db[1] *= (abs(p1p - by) / (float)  (SHUTTLE_HEIGHT / 2)) + 1;
		printf("\x1b[8;1Hdy Multiplier:  %f\x1b[K", (abs(p1p - by) / (float)  (SHUTTLE_HEIGHT / 2)) + 1);

		bx = SHUTTLE_WIDTH;
		by = nextY;
		return;

	}

	if (nextX >= SCREEN_WIDTH - SHUTTLE_WIDTH && abs(p2p - by) <= SHUTTLE_HEIGHT / 2 && nextX < SCREEN_WIDTH) {
		db[0] *=-1.1;
		
		if (db[1] == 0)
			db[1] = (abs(p2p - by) / (float)  (SHUTTLE_HEIGHT / 2)) + 1;
		else
			db[1] *= (abs(p2p - by) / (float) (SHUTTLE_HEIGHT / 2)) + 1;
		printf("\x1b[8;1Hdy Multiplier:  %f\x1b[K",(abs(p2p - by) / (float)  (SHUTTLE_HEIGHT / 2)) + 1);
		
		bx = SCREEN_WIDTH - SHUTTLE_WIDTH;
		by = nextY;
		return;

	}

	// if score

	if (bx < 0 && abs(p1p - by) > SHUTTLE_HEIGHT / 2) {
		score[1] += 1;
		resetBallPosition(true);
		return;
	}

	if (bx > SCREEN_WIDTH && abs(p2p - by) > SHUTTLE_HEIGHT / 2) {
		score[0] += 1;
		resetBallPosition(false);
		return;
	}

	bx = nextX;
	by = nextY;
}

void resetBallPosition(bool lastPlayerScored) {
	if (lastPlayerScored) {
		db[0] = 2;
		db[1] = (float) (rand() % 2);
	} else {
		db[0] = -2;
		db[1] = (float) (rand() % 2);
	}
	bx = SCREEN_WIDTH / 2;
	by = SCREEN_HEIGHT / 2;
}