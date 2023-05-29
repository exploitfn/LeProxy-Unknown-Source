
float visdist = 100.f;

DWORD_PTR closestPawn = NULL;
Vector3 localactorpos;
Vector3 Location;

static float smoothness1 = 4;
static float smoothness2 = 4;
static float smoothness3 = 4;
static float smoothness4 = 4;
static float smoothness5 = 4;

static float fov1 = 35;
static float fov2 = 35;
static float fov3 = 35;
static float fov4 = 35;
static float fov5 = 35;

static float fspread1 = 0.0f;
static float fbloom1 = 0.0f;
static float fspread2 = 0.0f;
static float fbloom2 = 0.0f;
static float fspread3 = 0.0f;
static float fbloom3 = 0.0f;
static float fspread4 = 0.0f;
static float fbloom4 = 0.0f;
static float fspread5 = 0.0f;
static float fbloom5 = 0.0f;

bool ShowMenu = true;
bool Aimbot = true;
bool autofire = false;
bool MemoryAim = false;
bool SilentAim = false;
bool StickySilent = false;
bool Triggerbot = false;
static int AimbotFOV = 150;
float fspread = 0.0f;
float fbloom = 0.0f;
int smooth = 4;
int offset_value = 0;
bool box = true;
bool b_headbox = false;
bool corner = false;
bool ThreeD = false;
bool line = false;
bool skeleton = false;
bool distance = false;
bool weapon = false;
bool bot = false;
bool ammo = false;
bool weaponconfigs = false;
bool rapid = false;
bool PlayerFreeze = false;
bool weaponspeed = false;
bool norecoil = false;
bool ammocount = false;
bool fullauto = false;
bool playerspeed = false;

float RandomFloat(float a, float b) 
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}