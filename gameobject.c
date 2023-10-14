// dynamics and collision objects
static dWorldID world;
static dSpaceID space;
static dJointGroupID contactgroup;

typedef struct gameobject {

  Color color;

  // physics
  dGeomID geom;
  dBodyID body;
  dMass mass;

} gameobject;

#define GAMEOBJECTS_MAX 100
int gameobjects_count = 0;
gameobject gameobjects[GAMEOBJECTS_MAX];

gameobject* gameobject_create() {
  gameobjects_count++;
  if (gameobjects_count == GAMEOBJECTS_MAX)
  {
    printf("Maximum number of gameobjects %d exceeded.", GAMEOBJECTS_MAX);
    exit(1);
  }
  return &gameobjects[gameobjects_count-1];
}

void gameobject_init_body(gameobject* g)
{
  g->body = dBodyCreate (world);
  dBodySetMass(g->body, &(g->mass));
  dGeomSetBody(g->geom, g->body);
}