#ifndef LOAD_H
#define LOAD_H

#include <NEMain.h>


extern NE_Model* debug_vision_model, *portal_orange_model, *portal_blue_model;

extern NE_Model *w_portalgun_model, *elevator_b_model;

extern NE_Animation *w_portalgun_fire1_animation, *elevator_b_doorsopen, *elevator_b_doorsclose;

extern NE_Material *w_portalgun_tex;
extern NE_Palette *w_portalgun_pal;


extern NE_Material * Debug_Material, * debugempty, * toolstrigger;
extern NE_Palette * toolstriggerNEPalette;


void LoadTextures();

// Loads a level from raw Hammer editor file format.
// Prefers to load from the filesystem and will fallback to nitrofs if file is not found
// @param levelName Level name to try to load for example "sp_a2_triple_laser"
int loadLevelVmf(char* levelName);
int loadLevelBsp(char* levelName);

void LoadMisc(void);
//i split blocks to sides for using the nodraw texture
// @param x1,y1,z1 vertex 1 position
// @param x2,y2,z2 vertex 2 position
// @param x3,y3,z3 vertex 3 position
// @param id is id
void RenderPlanes(Level level);
#endif