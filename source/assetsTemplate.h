#ifndef ASSETS_H
#define ASSETS_H
// Texture Metadata only used for loading the texture
typedef struct
{
    const char* name;
    const int x,y;
    const void *textureData;
    const void *palletteData;
    const int numColors;
    const NE_TextureFormat textureFormat;
    const NE_TextureFlags textureFlags;
} MaterialMetadata;



//Map Textures
#include "cube_bin.h"
#include "debug_plane_bin.h"
#include "Debug_tex_bin.h"
#include "debugempty_tex_bin.h"
#include "tools/toolstrigger.h"

//Model Textures
#include "w_portalgun_tex_bin.h"
#include "w_portalgun_pal_bin.h"
#include "portal_orange_tex_bin.h"
#include "portal_orange_pal_bin.h"
#include "portal_blue_tex_bin.h"
#include "portal_blue_pal_bin.h"

//Models
#include "Debug_sphere_bin.h"
#include "portal_bin.h"

//Animated Models and Animations
#include "w_portalgun_dsm_bin.h"
#include "w_portalgun_fire1_dsa_bin.h"

// Auto Generated below






#endif