
dMass m;
static dBodyID body;
static dGeomID geom;

static dWorldID world;
static dSpaceID space;

dReal* pos;
dReal* R;

void world_init()
{
  // create an ODE physics object

  body = dBodyCreate(world);
  dBodySetPosition(body, 0, 0, 5);

  //dRFromAxisAndAngle(R, x, y, z, angle) // ?
  dRSetIdentity (R);

  dBodySetRotation (body, R);

  // so it can know it's index ? (when in an array)
  // dBodySetData (obj[i].body,(void*) i);

  dMassSetBox (&m, .5, 1, 1, 1);
  geom = dCreateBox (space, 1, 1, 1);

  dGeomSetBody(geom, body);
  dBodySetMass(body, &m);
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

  dGeomID g =  geom;

  // draw ODE object
  pos = dGeomGetPosition (g);
  R = dGeomGetRotation (g);

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
}
