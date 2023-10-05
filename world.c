
TPE_Body body;
TPE_World world;
TPE_Joint joint;

TPE_Vec3 environmentDistance(TPE_Vec3 point, TPE_Unit maxDistance)
{
  return TPE_envGround(point,0); // just an infinite flat plane
}

void world_init()
{
  joint = TPE_joint(TPE_vec3(50*TPE_F, 30*TPE_F, 50*TPE_F),5*TPE_F);
  TPE_bodyInit(&body,&joint,1,0,0,2 * TPE_F);
  TPE_worldInit(&world,&body,1,environmentDistance);
}

void scene_discotech_update()
{
  float t = game_time;
  // Draw cube series
  for (int x=-10; x<11; x++)
    for (int z=-10; z<11; z++)
    {
      float displacement = (1+(abs(x))*(abs(z))/10);
      displacement *= 100*(.5+.5*cos(x/10.+z/10.+6*t));
      Vector3 position = {10*x, -1/2, 10*z};
      Color color = {200-displacement, 40+5*x, 40*z, 255};
      DrawCube(position, 10, 1, 10, color); 
    }
}

void world_update()
{
  scene_discotech_update();

  Vector3 position;

  position.x = TPE_bodyGetCenterOfMass(&body).x / TPE_F;
  position.y = TPE_bodyGetCenterOfMass(&body).y / TPE_F;
  position.z = TPE_bodyGetCenterOfMass(&body).z / TPE_F;

  DrawCube(position, 5, 5, 5, RED); 

  TPE_bodyApplyGravity(&body,TPE_F / 100);
  TPE_worldStep(&world);
}
