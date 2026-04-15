#ifndef MAIN_H
#define MAIN_H
#include <NEMain.h>
#include <squirrel.h>

#define LEVEL_SIZE 1
#define LEVEL_RENDER_SIZE 4

#define BSP_USE_LESS_MEMORY

#define MAX_PLANES 2000
#define MAX_HITBOXES 120
#define PLAYER_HIGHT 64 * LEVEL_SIZE //space below the camera
#define PLAYER_HIGHT_TOP 2 * LEVEL_SIZE //space above the camera
#define PLAYER_WIDTH 15 * LEVEL_SIZE
#define PLAYER_JUMPFORCE 2

#define SINMULTIPLIER 32790

#define PORTAL_PROJECTILE_WIDTH 64 * LEVEL_SIZE

#define CONSOLE_BACKGROUND_LAYER 2

// 2D point 
typedef struct
{
	float x;
	float y;
} Vector2;

// 3D point 
typedef struct
{
	float x;
	float y;
	float z;
} Vector3;

typedef struct
{
    Vector3 velocity;
	bool isGrounded;
} PHYSICS;

// Plane values
typedef struct
{
	Vector3 vertex1;
	Vector3 vertex2;
	Vector3 vertex3;
	Vector3 vertex4;
	NE_Material* material;
	int x0;
	int x1;
	int y0;
	int y1;
	float nx;
	float ny;
	float nz;
	bool isDrawn;
	NE_Physics* Physics;
} PLANE;

typedef struct
{
	char* input;
	int mode; // 0 = continuous, 1 = trigger_once
	bool alreadyTriggered;
}trigger;

typedef struct
{
	char* target;
}pointTeleport;

typedef struct
{
	Vector3 vertex[8];
	Vector3 vector[3];
	float sizeX;
	float sizeY;
	float sizeZ;
	Vector3 position;
	Vector3 rotation;
	Vector3* attachedPosition; // Position for dynamic hitboxes
	Vector3* attachedRotation; // Rotation for dynamic hitboxes
	bool isDynamic; //if the hitbox can't move
	bool isPlane; // it is only a Plane
	bool isTrigger;
	PLANE* attachedPlane; // The plane it belongs to
	trigger* attachedTrigger; // The trigger it belongs to
} hitbox;

typedef struct
{
	Vector3 position;
	Vector3 rotation;
} Cube;

typedef struct
{
	PLANE* plane;
	Vector2 portalPosition;
} PortaledPlane;

typedef struct
{
	bool type; //color
	bool onFloor; //if the portal is on the floor/cieling
	Vector3 position;
	Vector3 rotation;
	PortaledPlane portaledPlane;
} Portal;

typedef struct
{
	char* className;
	char* targetName;
	Vector3 position;
	Vector3 rotation;
	void* child;
} Entity;

typedef struct
{
	NE_Model* model;
} Model;

typedef struct
{
	int startSpeed;
	Entity* nextNode;
	bool isMoving;
} funcTracktrain;

typedef struct
{
	char* name;
	PLANE Plane[MAX_PLANES];
	Portal portal[2];
	int planeCount;
	hitbox allHitboxes[MAX_HITBOXES];
	int currentHitbox;
	int dynamicHitbxes[20]; // Wich hitboxes are Dynamic
	int currentDynamicHitbox;
	Entity entities[100];
	int currentEntity;
} Level;

typedef struct
{
	Vector3 position; //Player Position (x,y,z)
	Vector3 rotation; //Player Rotaion (x,y,z)
	hitbox hitbox;
	PHYSICS physics;
	char* name; //Player Name
	Vector3 lookVector; // vector in witch direction the player is looking
	bool isJumping;
} PLAYER;


extern int textureMode;
extern int sensitivityHorizontal;
extern int sensitivityVertical;
extern bool debugText;
extern bool debugVision;
extern bool drawCollision;
extern HSQUIRRELVM squirrelvm;
extern Level level;
extern PLAYER localPlayer;
extern hitbox testBox, testBox2;
extern bool isConsoleOpen;
extern Keyboard *keyboard;

// Call a Squirrel (ingame script language) function
int callSquirrel(HSQUIRRELVM vm, const char* function);
void registerEntity(char* className, char* targetName, void* entity, Vector3 position, Vector3 rotation);
int findEntityByName(char* name, Entity** entity);
int max(int __arg1, int __arg2);
int min(int __arg1, int __arg2);
void printfWarning(const char* message, ...);
void printMemory(char* file, char* func, int line);
int entFire(char* entityName,char *action, ...);

#endif