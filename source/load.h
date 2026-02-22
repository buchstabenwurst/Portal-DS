#ifndef LOAD_H
#define LOAD_H

#include <NEMain.h>


extern NE_Model* debug_vision_model, *portal_orange_model, *portal_blue_model;

extern NE_Model *w_portalgun_model;

extern NE_Animation *w_portalgun_fire1_animation;

extern NE_Material *w_portalgun_tex;
extern NE_Palette *w_portalgun_pal;


extern NE_Material* white_ceiling_tile002a, * white_floor_tile002a, * white_floor_tile002a_hd, * white_wall_tile003a, * white_wall_tile003a_hd, * white_wall_tile003c, * white_wall_tile003c_hd, * white_wall_tile003f, * white_wall_tile003f_hd, * black_floor_metal_001c, * black_floor_metal_001c_hd, * black_wall_metal_002a, * black_wall_metal_002a_hd, * black_wall_metal_002b, * black_wall_metal_002b_hd, * black_wall_metal_002c, * black_wall_metal_002c_hd, * Debug_Material, * debugempty, * toolstrigger;
extern NE_Palette* white_ceiling_tile002aNEPalette, * white_floor_tile002aNEPalette, * white_floor_tile002a_hdNEPalette, * white_wall_tile003aNEPalette, * white_wall_tile003a_hdNEPalette, * white_wall_tile003cNEPalette, * white_wall_tile003c_hdNEPalette, * white_wall_tile003fNEPalette, * white_wall_tile003f_hdNEPalette, * black_floor_metal_001cNEPalette, * black_floor_metal_001c_hdNEPalette, * black_wall_metal_002aNEPalette, * black_wall_metal_002a_hdNEPalette, * black_wall_metal_002bNEPalette, * black_wall_metal_002b_hdNEPalette, * black_wall_metal_002cNEPalette, * black_wall_metal_002c_hdNEPalette, * toolstriggerNEPalette;



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