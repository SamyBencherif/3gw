#include <stdlib.h>
#include <math.h>

#include "raylib.h" /* 4.5.0 */
#include <rlgl.h>
#include <raymath.h>

#include "tinyphysicsengine.h" /* 995316e5 */

float game_time = 0;

#include "graphics.c"

#include "player.c"
#include "pause.c"

#include "world.c"

void update(void)
{
  if (IsKeyPressed(KEY_ESCAPE)) { 
    pause_toggle();
  }
  if (!paused) {
    first_person_controller();
    game_time += GetFrameTime();
  }

  BeginDrawing();
  ClearBackground(RAYWHITE);
  BeginMode3D(camera);

  world_update();

  EndMode3D();

  // 2D Drawing Mode
  if (paused) draw_pause_menu();
  else 
  {
    // draw 3d crosshair
    float size = 10;
    Color color = BLACK;
    color.a = 190;
    DrawLine(GetScreenWidth()/2-size, GetScreenHeight()/2, GetScreenWidth()/2+size, GetScreenHeight()/2, color); 
    DrawLine(GetScreenWidth()/2, GetScreenHeight()/2-size, GetScreenWidth()/2, GetScreenHeight()/2+size, color); 
  }

  // End 2D Drawing Mode

  EndDrawing();
}

int main(void)
{
  srandom(0x0);
  //SetTraceLogLevel(LOG_ERROR);

  int width = GetScreenWidth();
  int height = GetScreenHeight();

  InitWindow(width, height, "game");
  SetWindowState(FLAG_FULLSCREEN_MODE);

  // don't close on 'ESC'
  SetExitKey(0);

  // initialize audio
  InitAudioDevice();
  //printf("Waiting for audio device...\r\n");
  while (!IsAudioDeviceReady());
  SetMasterVolume(.25);

  camera.position = (Vector3){ 0.0, 5.0, 0.0f };
  camera.target = (Vector3){ 0.0, 5.0, -1.0f };
  camera.up = (Vector3){ 0.0, 1.0, 0.0f };
  camera.fovy = 80.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  DisableCursor();

  SetTargetFPS(60);

  player_init();
  world_init();

  while (!(WindowShouldClose()))
  {
    update();
  }

  CloseWindow();
  CloseAudioDevice();

  return 0;
}
