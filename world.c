// dynamics and collision objects
static dWorldID world;
static dSpaceID space;
static dBodyID body;	
static dGeomID geom;	
static dMass m;
static dJointGroupID contactgroup;

void world_init()
{
   dInitODE ();
   // create world
   world = dWorldCreate ();
   space = dHashSpaceCreate (0);
   dWorldSetGravity (world,0,0,-300);
   dWorldSetCFM (world,1e-5);
   dCreatePlane (space,0,0,1,0);
   contactgroup = dJointGroupCreate (0);
   // create object
   body = dBodyCreate (world);
   geom = dCreateSphere (space,5);
   dMassSetSphere (&m,1,0.5);
   dBodySetMass (body,&m);
   dGeomSetBody (geom,body);
   // set initial position
   dBodySetPosition (body,30,-30,30);

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
   if (dCollide (o1,o2,1,&contact.geom,sizeof(dContact))) {
       dJointID c = dJointCreateContact (world,contactgroup,&contact);
       dJointAttach (c,b1,b2);
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

  const dReal *pos;
  const dReal *R;
  // find collisions and add contact joints
  dSpaceCollide (space,0,&nearCallback);
  // step the simulation
  dWorldQuickStep (world,0.01);  
  // remove all contact joints
  dJointGroupEmpty (contactgroup);
  // redraw sphere at new location
  pos = dGeomGetPosition (geom);
  R = dGeomGetRotation (geom); // rotation

  Vector3 p = {pos[0], pos[1], pos[2]};
  DrawSphere(p, 5, BLACK);
}
