typedef struct gameobject {

    Color color;

    // physics
    dGeomID geom;
    dBodyId body;
    dMass mass;

} gameobject;

#define gameobjects_max 100
int gameobjects_count = 0;
gameobject gameobjects[gameobjects_max];