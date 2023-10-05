Camera camera = { 0 };
Vector2 look = {0., 0.};
bool mouse_ready = false;

#define PLAYER_HEIGHT 15
#define JUMP_HEIGHT 6
#define JUMP_DURATION .5

#define NOCLIP false

float timer_jump = 0;
Vector3 forward = {0, 0, 0}; // player's forward direction

float player_radius = 1;

#define SHOW_FLOORPLAN true
#define FP_LINE_SIZE 256
int fp_line_count = 0;
typedef struct Line {
  int x1;
  int y1;
  int x2;
  int y2;
} Line;
Line fp_lines[FP_LINE_SIZE];

void player_move(float x, float y)
{
  camera.position.x += x;
  camera.position.y += y;
}

void player_jump()
{
  if (IsKeyPressed(KEY_SPACE) && timer_jump == 0) timer_jump = JUMP_DURATION;
  if (timer_jump > 0) timer_jump -= GetFrameTime();
  else timer_jump = 0;

  // let's call this 'stylized jumping' instead of physically based
  camera.position.z = PLAYER_HEIGHT + sin(timer_jump*PI/JUMP_DURATION) * JUMP_HEIGHT;
  camera.target.z = camera.position.z + forward.z;
}

void player_look(float x, float y)
{
  look.x += x; 
  look.y += y; 

  float pitch_clamp = PI/2-.0001;
  if (look.y < -pitch_clamp) look.y = -pitch_clamp;
  if (look.y > pitch_clamp) look.y = pitch_clamp;
}

// ported from CALICO main.js intersects
Vector3 nearest(Vector3 pos, Line wall)
{
  // This is a two dimensional function that applies to the XZ plane.
  // given a specific wall and a pos, it tells you the nearest point to pos
  // on the line of wall.
  // 
  // the point is returned in the XZ fields of the return value
  // the Y field is set to the parametrization parameter, t.
  // it is in the range [0, 1] when the point rests on the wall

  float w0 = wall.x1; float w1 = wall.y1; float w2 = wall.x2; float w3 = wall.y2;
  float px = pos.x; float py = pos.y;

  Vector2 wtop = {px-w0, py-w1};
  Vector2 wtow = {w2-w0, w3-w1};
  float wtowM = sqrt(wtow.x*wtow.x + wtow.y*wtow.y);
  wtow.x /= wtowM;
  wtow.y /= wtowM;

  float t = (wtop.x*wtow.x + wtop.y*wtow.y)/wtowM;

  Vector3 result = {w0+(w2-w0)*t, w1+(w3-w1)*t, t};

  return result;
}

void player_init()
{
  camera.position = (Vector3){ 0.0, 0.0, 5.0f };
  camera.target = (Vector3){ 0.0, -1.0, 5.0f };
  camera.up = (Vector3){ 0.0, 0.0, 1.0f };
  camera.fovy = 80.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  DisableCursor();

  SetTargetFPS(60);

  int box_size = 50;

  fp_lines[fp_line_count].x1 = -box_size; 
  fp_lines[fp_line_count].y1 = -box_size; 
  fp_lines[fp_line_count].x2 = -box_size; 
  fp_lines[fp_line_count].y2 = box_size; 
  fp_line_count++;

  fp_lines[fp_line_count].x1 = -box_size; 
  fp_lines[fp_line_count].y1 = box_size; 
  fp_lines[fp_line_count].x2 = box_size; 
  fp_lines[fp_line_count].y2 = box_size; 
  fp_line_count++;

  fp_lines[fp_line_count].x1 = box_size; 
  fp_lines[fp_line_count].y1 = box_size; 
  fp_lines[fp_line_count].x2 = box_size; 
  fp_lines[fp_line_count].y2 = -box_size; 
  fp_line_count++;

  fp_lines[fp_line_count].x1 = box_size; 
  fp_lines[fp_line_count].y1 = -box_size; 
  fp_lines[fp_line_count].x2 = -box_size; 
  fp_lines[fp_line_count].y2 = -box_size; 
  fp_line_count++;
}

void first_person_controller()
{
  float speed = .5;

  // compute forward look direction
  float r = cos(look.y);
  forward.x = r*cos(look.x-PI/2);
  forward.y = r*sin(look.x-PI/2);
  forward.z = sin(look.y);
  
  //forward = Vector3Normalize(forward);

  Vector3 right = {0, 0, 0};
  // compute orthogonal complement to forward
  right.x = cos(look.x);
  right.y = sin(look.x);
  right.z = 0;

  //right = Vector3Normalize(right);

  Vector2 walkForward = {forward.x, forward.y};
  walkForward = Vector2Normalize(walkForward);

  if (IsKeyDown(KEY_W))
    player_move(speed*walkForward.x, speed*walkForward.y);
  if (IsKeyDown(KEY_S))
    player_move(speed*-walkForward.x, speed*-walkForward.y);
  if (IsKeyDown(KEY_A))
    player_move(speed*right.x, speed*right.y);
  if (IsKeyDown(KEY_D))
    player_move(speed*-right.x, speed*-right.y);

  float look_speed = .001;

  Vector2 look_input = {0, 0};
  Vector2 md = GetMouseDelta();

  // support mouse look
  if (mouse_ready)
  { 
    look_input.x += md.x;
    look_input.y += md.y;
  }

  // support ijkl look
  float ijkl_look_speed = 25;
  look_input.x += ijkl_look_speed * (((int)IsKeyDown(KEY_L)) - ((int)IsKeyDown(KEY_J)));
  look_input.y += ijkl_look_speed * (((int)IsKeyDown(KEY_K)) - ((int)IsKeyDown(KEY_I)));

  player_look(-look_speed*look_input.x, -look_speed*look_input.y);

  camera.target.x = camera.position.x + forward.x;
  camera.target.y = camera.position.y + forward.y;
  camera.target.z = camera.position.z + forward.z;

  // mouse is ready on the frame after the first md != <0,0>
  if (md.x != 0 | md.y != 0)
    mouse_ready = true;

  if (!NOCLIP)
    player_jump();
  else
  {
    if (IsKeyDown(KEY_SPACE))
      camera.position.z += .3;
    if (IsKeyDown(KEY_LEFT_SHIFT))
      camera.position.z -= .3;

    camera.target.x = camera.position.x + forward.x;
    camera.target.y = camera.position.y + forward.y;
    camera.target.z = camera.position.z + forward.z;

  }

  if (!NOCLIP)
  for (int i=0; i<fp_line_count; i++)
  {
    Vector3 I = nearest(camera.position, fp_lines[i]);
    // I.x - x coord
    // I.y - y coord
    // I.z - t value

    float mag = sqrt(pow(camera.position.x - I.x, 2) + pow(camera.position.y - I.y, 2));
    if (0<=I.z && I.z<=1 && mag <= player_radius) 
    {
      Vector3 diff = Vector3Subtract(camera.position, I);
      float diffM = sqrt(diff.x*diff.x + diff.y*diff.y);
      diff.x /= diffM;
      diff.y /= diffM;

      Vector3 offset = Vector3Scale(diff, player_radius-diffM);
      offset.z = 0;

      camera.position = Vector3Add(camera.position, offset);
      camera.target.x = camera.position.x + forward.x;
      camera.target.y = camera.position.y + forward.y;
      camera.target.z = camera.position.z + forward.z;
    }
  }
}
