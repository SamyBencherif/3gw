
void world_init()
{
}

void scene_discotech()
{
  float t = game_time;
  // Draw cube series
  for (int x=-10; x<11; x++)
    for (int y=-10; y<11; y++)
    {
      float displacement = (1+(abs(x))*(abs(y))/10);
      displacement *= (.5+.5*cos(x/10.+y/10.+6*t));
      Vector3 position = {10*x, 10*y, displacement/2};
      Color color = {200, 40+5*x, 40*y, 255};
      DrawCube(position, 3, 3, 1+displacement, color); 
    }
}

void world()
{
  scene_discotech();
}
