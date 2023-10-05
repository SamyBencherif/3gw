/*
Shows spawning and simulating of 3D physics using ODE

2023-10-04 SB: start port to Raylib
The first step is to remove all DrawStuff calls, replacing with a comment that says GRAPHICS
We are using ODE with single precision
*/

//#include <unistd.h> // for usleep()
#include <ode/ode.h>
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "../player.c"

// some constants
#define NUM 100			// max number of objects
#define DENSITY (5.0)		// density of all objects
#define GPB 3			// maximum number of geometries per body
#define MAX_CONTACTS 8		// maximum number of contact points per body
#define USE_GEOM_OFFSET 1

// dynamics and collision objects

struct MyObject {
    dBodyID body;			// the body
    dGeomID geom[GPB];		// geometries representing this body
};

static int num=0;		// number of objects in simulation
static int nextobj=0;		// next object to recycle if num==NUM
static dWorldID world;
static dSpaceID space;
static MyObject obj[NUM];
static dJointGroupID contactgroup;
static int show_aabb = 0;	// show geom AABBs?
static int show_contacts = 0;	// show contact points?
static int random_pos = 1;	// drop objects from random position?
static int write_world = 0;
static int show_body = 0;

static dGeomID platform, ground;

dVector3 platpos = {0, 0, 0};
int mov_type = 2;
dReal mov_time = 0;


const dReal mov1_speed = 0.2;

dVector3 mov2_vel = { 0.2, 0.1, 0.25};

static void nearCallback (void *, dGeomID o1, dGeomID o2)
{
    dMatrix3 RI;
    static const dReal ss[3] = {0.02,0.02,0.02};

    dContact contact[MAX_CONTACTS];
    int numc = dCollide (o1, o2, MAX_CONTACTS,
                         &contact[0].geom, sizeof(dContact));

    if (numc)
        dRSetIdentity(RI);

    bool isplatform = (o1 == platform) || (o2 == platform);

    for (int i=0; i< numc; i++) {
        contact[i].surface.mode = dContactBounce;
        contact[i].surface.mu = 1;
        contact[i].surface.bounce = 0.10;
        contact[i].surface.bounce_vel = 0.01;
        
        dJointID c = dJointCreateContact (world,contactgroup,contact+i);
        dJointAttach (c, dGeomGetBody(o1), dGeomGetBody(o2));
        //if (show_contacts) 
            // GRAPHICS dsDrawBox (contact[i].geom.pos, RI, ss);
    }
}


// start simulation - set viewpoint

static float xyz[3] = {2.1106f,-1.3007,2.f};
static float hpr[3] = {150.f,-13.5000f,0.0000f};

static void start()
{
    // GRAPHICS dsSetViewpoint (xyz,hpr);

    printf ("To drop another object, press:\n");
    printf ("   c for cube.\n");
    printf ("   b for ball.\n");
    printf ("   l for capsule.\n");
    printf ("   y for cylinder.\n");
    printf ("Press m to change the movement type\n");
    printf ("Press space to reset the platform\n");
    printf ("To toggle showing the geom AABBs, press a.\n");
    printf ("To toggle showing the contact points, press t.\n");
    printf ("To toggle dropping from random position/orientation, press r.\n");
    printf ("To save the current state to 'state.dif', press 1.\n");
}


char locase (char c)
{
    if (c >= 'A' && c <= 'Z') return c + ('a'-'A');
    else return c;
}


// called when a key pressed

static void command (int cmd)
{
    dsizeint i;
    int k;
    dReal sides[3];
    dMass m;
    int setBody;
  
    cmd = locase (cmd);
    if (cmd == 'c' || cmd == 'b' || cmd == 'l' || cmd == 'y')
        {
            setBody = 0;
            if (num < NUM) {
                i = num;
                num++;
            }
            else {
                i = nextobj;
                nextobj++;
                if (nextobj >= num) nextobj = 0;

                // destroy the body and geoms for slot i
                if (obj[i].body) {
                  dBodyDestroy (obj[i].body);
                }
                for (k=0; k < GPB; k++) {
                    if (obj[i].geom[k]) {
                      dGeomDestroy (obj[i].geom[k]);
                    }
                }
                memset (&obj[i],0,sizeof(obj[i]));
            }

            obj[i].body = dBodyCreate (world);
            for (k=0; k<3; k++) sides[k] = dRandReal()*0.5+0.1;

            dMatrix3 R;
            if (random_pos) 
                {
                    dBodySetPosition (obj[i].body,
                                      dRandReal()*2-1 + platpos[0],
                                      dRandReal()*2-1 + platpos[1],
                                      dRandReal()+2 + platpos[2]);
                    dRFromAxisAndAngle (R,dRandReal()*2.0-1.0,dRandReal()*2.0-1.0,
                                        dRandReal()*2.0-1.0,dRandReal()*10.0-5.0);
                }
            else 
                {
                    dBodySetPosition (obj[i].body, 
                                      platpos[0],
                                      platpos[1],
                                      platpos[2]+2);
                    dRSetIdentity (R);
                }
            dBodySetRotation (obj[i].body,R);
            dBodySetData (obj[i].body,(void*) i);

            if (cmd == 'c') {
                dMassSetBox (&m,DENSITY,sides[0],sides[1],sides[2]);
                obj[i].geom[0] = dCreateBox (space,sides[0],sides[1],sides[2]);
            }
            else if (cmd == 'l') {
                sides[0] *= 0.5;
                dMassSetCapsule (&m,DENSITY,3,sides[0],sides[1]);
                obj[i].geom[0] = dCreateCapsule (space,sides[0],sides[1]);
            }
            else if (cmd == 'y') {
                dMassSetCylinder (&m,DENSITY,3,sides[0],sides[1]);
                obj[i].geom[0] = dCreateCylinder (space,sides[0],sides[1]);
            }
            else if (cmd == 'b') {
                sides[0] *= 0.5;
                dMassSetSphere (&m,DENSITY,sides[0]);
                obj[i].geom[0] = dCreateSphere (space,sides[0]);
            }

            if (!setBody)
                for (k=0; k < GPB; k++) {
                    if (obj[i].geom[k]) {
                      dGeomSetBody (obj[i].geom[k],obj[i].body);
                    }
                }

            dBodySetMass (obj[i].body,&m);
        }
    else if (cmd == 'a') {
        show_aabb ^= 1;
    }
    else if (cmd == 't') {
        show_contacts ^= 1;
    }
    else if (cmd == 'r') {
        random_pos ^= 1;
    }
    else if (cmd == '1') {
        write_world = 1;
    }
    else if (cmd == ' ') {
        mov_time = 0;
    }
    else if (cmd == 'm') {
        mov_type = mov_type==1 ? 2 : 1;
        mov_time = 0;
    }
}


// draw a geom

void drawGeom (dGeomID g, const dReal *pos, const dReal *R, int show_aabb)
{
    int i;
	
    if (!g) return;
    if (!pos) pos = dGeomGetPosition (g);
    if (!R) R = dGeomGetRotation (g);
    dQuaternion Q;
    dGeomGetQuaternion(g, Q);

    int type = dGeomGetClass (g);
    if (type == dBoxClass) {
        dVector3 sides;
        dGeomBoxGetLengths (g,sides);
        // GRAPHICS dsDrawBox (pos,R,sides);
        float scale = 10;

        Vector3 rlPos;
        rlPos.x = scale*pos[0];
        rlPos.y = scale*pos[1];
        rlPos.z = scale*pos[2];

        Vector3 size;
        size.x = scale*sides[0];
        size.y = scale*sides[1];
        size.z = scale*sides[2];
  
        Vector3 zero;
        zero.x = 0;
        zero.y = 0;
        zero.z = 0;

        // R is rotation of type dMatrix3 (a 3x4 matrix, with an extra 0 col)
        rlPushMatrix();
        rlTranslatef(rlPos.x, rlPos.y, rlPos.z);

        // derive angle and axis of rotation from Quaternion
        // Thx to https://eater.net/quaternions/video/intro for explaining Quaternions to me !
        float angle = acos(Q[0])*2;
        float axis_x = Q[1];
        float axis_y = Q[2];
        float axis_z = Q[3];
        if (sin(angle) == 0)
        {
          axis_x = 1;
          axis_y = 0;
          axis_z = 0;
        }
        else
        {
          axis_x /= sin(angle);
          axis_y /= sin(angle);
          axis_z /= sin(angle);
        }

        // rlRotatef takes degrees
        rlRotatef(180/PI*angle, axis_x, axis_y, axis_z);

        Color c = RED;
        c.r -= 100*sin(size.x+rlPos.x); // "random" shade of red
        DrawCube(zero, size.x, size.y, size.z, c);
        rlPopMatrix();
    }
    else if (type == dSphereClass) {
        // GRAPHICS dsDrawSphere (pos,R,dGeomSphereGetRadius (g));
        float scale = 10;

        Vector3 rlPos;
        rlPos.x = scale*pos[0];
        rlPos.y = scale*pos[1];
        rlPos.z = scale*pos[2];
        DrawSphere(rlPos, scale*dGeomSphereGetRadius(g), BLUE);
    }
    else if (type == dCapsuleClass) {
        dReal radius,length;
        dGeomCapsuleGetParams (g,&radius,&length);
        // GRAPHICS dsDrawCapsule (pos,R,length,radius);
    }
    else if (type == dCylinderClass) {
        dReal radius,length;
        dGeomCylinderGetParams (g,&radius,&length);
        // GRAPHICS dsDrawCylinder (pos,R,length,radius);
    }

    if (show_body) {
        dBodyID body = dGeomGetBody(g);
        if (body) {
            const dReal *bodypos = dBodyGetPosition (body); 
            const dReal *bodyr = dBodyGetRotation (body); 
            dReal bodySides[3] = { 0.1, 0.1, 0.1 };
            // GRAPHICS dsSetColorAlpha(0,1,0,1);
            // GRAPHICS dsDrawBox(bodypos,bodyr,bodySides); 
        }
    }
    if (show_aabb) {
        // draw the bounding box for this geom
        dReal aabb[6];
        dGeomGetAABB (g,aabb);
        dVector3 bbpos;
        for (i=0; i<3; i++) bbpos[i] = 0.5*(aabb[i*2] + aabb[i*2+1]);
        dVector3 bbsides;
        for (i=0; i<3; i++) bbsides[i] = aabb[i*2+1] - aabb[i*2];
        dMatrix3 RI;
        dRSetIdentity (RI);
        // GRAPHICS dsSetColorAlpha (1,0,0,0.5);
        // GRAPHICS dsDrawBox (bbpos,RI,bbsides);
    }
}


// simulation loop

static void updatecam()
{
    xyz[0] = platpos[0] + 3.3;
    xyz[1] = platpos[1] - 1.8;
    xyz[2] = platpos[2] + 2;
    // GRAPHICS dsSetViewpoint (xyz, hpr);
}

static void simLoop (int pause)
{
    const dReal stepsize = 0.02;

    // GRAPHICS dsSetColor (0,0,2);
    dSpaceCollide (space,0,&nearCallback);
    if (!pause) {
        dGeomSetPosition(platform, platpos[0], platpos[1], platpos[2]);
        updatecam();
        dWorldQuickStep (world,stepsize);
        //dWorldStep (world,stepsize);
    }

    if (write_world) {
        FILE *f = fopen ("state.dif","wt");
        if (f) {
            dWorldExportDIF (world,f,"X");
            fclose (f);
        }
        write_world = 0;
    }
  
    // remove all contact joints
    dJointGroupEmpty (contactgroup);

    // GRAPHICS dsSetColor (1,1,0);
    // GRAPHICS dsSetTexture (DS_WOOD);
    for (int i=0; i<num; i++) {
        for (int j=0; j < GPB; j++) {
            if (! dBodyIsEnabled (obj[i].body)) {
                // GRAPHICS dsSetColor (1,0.8,0);
            }
            else {
                // GRAPHICS dsSetColor (1,1,0);
            }
            drawGeom (obj[i].geom[j],0,0,show_aabb);
        }
    }
    // GRAPHICS dsSetColor (1,0,0);
    drawGeom (platform,0,0,show_aabb);
    //usleep(5000);
}


void scene_discotech()
{
  float t = 0;
  // Draw cube series
  for (int x=-10; x<11; x++)
    for (int z=-10; z<11; z++)
    {
      float displacement = (1+(abs(x))*(abs(z))/10);
      displacement *= (.5+.5*cos(x/10.+z/10.+6*t));
      Vector3 position = {10.f*x, 10.f*z, displacement/2};
      Color color;// = {200, 40+5*x, 40*z, 255};
      color.r = 200;
      color.g = 40+5*x;
      color.b = 40*z;
      color.a = 255;
      DrawCube(position, 3, 1+displacement, 3, color); 
    }
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    // create world
    dInitODE();
    start();
    player_init();
    world = dWorldCreate();

    space = dHashSpaceCreate (0);

    contactgroup = dJointGroupCreate (0);
    dWorldSetGravity (world,0,0,-0.5);
    dWorldSetCFM (world,1e-5);
    
    dWorldSetLinearDamping(world, 0.00001);
    dWorldSetAngularDamping(world, 0.005);
    dWorldSetMaxAngularSpeed(world, 200);

    dWorldSetContactSurfaceLayer (world,0.001);
    ground = dCreatePlane (space,0,0,1,0);
    
    memset (obj,0,sizeof(obj));

    // create lift platform
    platform = dCreateBox(space, 4, 4, 1);

    dGeomSetCategoryBits(ground, 1ul);
    dGeomSetCategoryBits(platform, 2ul);
    dGeomSetCollideBits(ground, ~2ul);
    dGeomSetCollideBits(platform, ~1ul);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update

        // Draw
        BeginDrawing();
				ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        scene_discotech();
				simLoop(0);

				// if a key is pressed, run command
				if (IsKeyPressed(KEY_C)) command('c');
				if (IsKeyPressed(KEY_Y)) command('y');
				if (IsKeyPressed(KEY_B)) command('b');
  
        first_person_controller();

        EndMode3D();

				//DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    // De-Initialization
    dJointGroupDestroy (contactgroup);
    dSpaceDestroy (space);
    dWorldDestroy (world);
    dCloseODE();
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
