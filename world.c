
void world_init()
{
  // create an ODE physics object
  /*
  dMatrix3 R;
  dMass m;
  dBody body;
  dGeom geom;

  body = dBodyCreate(world)
  dBodySetPosition(body, 0, 0, 5);

  dRFromAxisAndAngle(R, x, y, z, angle) // ?

  dBodySetRotation (body, R);

  // so it can know it's index ? (when in an array)
  // dBodySetData (obj[i].body,(void*) i);

  dMassSetBox (&m, DENSITY, 1, 1, 1);
  geom = dCreateBox (space, 1, 1, 1);

  dGeomSetBody(geom, body);
  dBodySetMass(body, &m);

  */
}

void scene_discotech()
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

void world()
{
  scene_discotech();
}
