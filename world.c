// dynamics and collision objects
static dWorldID world;
static dSpaceID space;
static dJointGroupID contactgroup;

void world_init()
{
  // initialize physics
  dInitODE ();
  world = dWorldCreate ();
  space = dHashSpaceCreate (0);
  dWorldSetGravity (world, 0, 0, -300);
  dWorldSetCFM (world, 1e-5);
  dCreatePlane (space, 0, 0, 1, 0);
  contactgroup = dJointGroupCreate (0);

  // create ball
  float radius = 5;
  float density = .0001;
  gameobject* g = gameobject_create();
  g->body = dBodyCreate (world);
  g->geom = dCreateSphere (space, radius);
  dMassSetSphere (&(g->mass), density, radius);
  dBodySetMass (g->body, &(g->mass));
  dGeomSetBody (g->geom, g->body);
  // set initial position
  dBodySetPosition (g->body, 30, -30, 30);
  g->color = BLACK;

  // create ball 2
  radius = 5;
  density = .0001;
  g = gameobject_create();
  g->body = dBodyCreate (world);
  g->geom = dCreateSphere (space, radius);
  dMassSetSphere (&(g->mass), density, radius);
  dBodySetMass (g->body, &(g->mass));
  dGeomSetBody (g->geom, g->body);
  // set initial position
  dBodySetPosition (g->body, 30, 30, 30);
  g->color = BLUE;

  // create cube
  density = .0001;
  g = gameobject_create();
  g->body = dBodyCreate (world);
  g->geom = dCreateBox (space, 10, 10, 10);
  dMassSetBox (&(g->mass), density, 10, 10, 10);
  dBodySetMass (g->body, &(g->mass));
  dGeomSetBody (g->geom, g->body);
  // set initial position
  dBodySetPosition (g->body, 60, 30, 30);
  g->color = RED;

}

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.
static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
   dBodyID b1 = dGeomGetBody(o1);
   dBodyID b2 = dGeomGetBody(o2);
   dContact contact;  
   contact.surface.mode = dContactBounce | dContactSoftCFM;
   // friction parameter
   contact.surface.mu = dInfinity;
   // bounce is the amount of "bouncyness".
   contact.surface.bounce = 0.6;
   // bounce_vel is the minimum incoming velocity to cause a bounce
   contact.surface.bounce_vel = 0.1;
   // constraint force mixing parameter
   contact.surface.soft_cfm = 0.001;  
   if (dCollide (o1, o2, 1, &contact.geom, sizeof(dContact))) {
       dJointID c = dJointCreateContact (world, contactgroup, &contact);
       dJointAttach (c, b1, b2);
   }
}

void scene_discotech_update()
{
  float t = game_time;
  // Draw cube series
  for (int x=-10; x<11; x++)
    for (int y=-10; y<11; y++)
    {
      float displacement = (1+(abs(x))*(abs(y))/10);
      displacement *= 100*(.5+.5*cos(x/10.+y/10.+6*t));
      Vector3 position = {10*x, 10*y, -1/2};
      Color color = {200-displacement, 40+5*x, 40*y, 255};
      DrawCube(position, 10, 10, 1, color); 
    }
}

void world_update()
{
  scene_discotech_update();

  // find collisions and add contact joints
  dSpaceCollide (space, 0, &nearCallback);

  // step the simulation
  if (!paused)
    dWorldQuickStep (world, 0.01);  

  // remove all contact joints
  dJointGroupEmpty (contactgroup);

  // redraw sphere at new location
  for (int i=0; i<gameobjects_count; i++)
  {
    gameobject* g = &gameobjects[i];

    const dReal *pos = dGeomGetPosition (g->geom);
    const dReal *rot = dGeomGetRotation (g->geom);
    
    // variables needed by switch-case
    Vector3 p;
    Vector3 size;
    Vector3 zero = {0,0,0};

    dVector3 sides;
    dQuaternion Q;
    
    switch (dGeomGetClass(g->geom))
    {
      case dSphereClass:
        p.x = pos[0]; p.y = pos[1]; p.z = pos[2];
        DrawSphere(p, 5, g->color);
        break;
      case dBoxClass:
        dGeomBoxGetLengths (g->geom,sides);
        dGeomGetQuaternion(g->geom, Q);

        p.x = pos[0];
        p.y = pos[1];
        p.z = pos[2];

        size.x = sides[0];
        size.y = sides[1];
        size.z = sides[2];
  
        // R is rotation of type dMatrix3 (a 3x4 matrix, with an extra 0 col)
        rlPushMatrix();
        rlTranslatef(p.x, p.y, p.z);

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

        DrawCube(zero, size.x, size.y, size.z, g->color);
        rlPopMatrix();
        break;
      default:
        p.x = pos[0]; p.y = pos[1]; p.z = pos[2];
        DrawSphere(p, 5, MAGENTA);
        break;
    }
  }
}
