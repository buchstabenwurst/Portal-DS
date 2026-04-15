#include <NEMain.h>
#include <string.h>
#include <squirrel.h>
#include <sqstdio.h>
#include "assetsArray.h"
#include "main.h"
#include "load.h"
#include "physics.h"
#include "bspfile.h"


//Models
NE_Model* debug_vision_model, *portal_orange_model, *portal_blue_model;

//Animated models
NE_Model *w_portalgun_model, *elevator_b_model;

//Animations
NE_Animation *w_portalgun_fire1_animation, *elevator_b_doorsopen, *elevator_b_doorsclose;

//Model textues
NE_Material *w_portalgun_tex, *portal_orange_tex, *portal_blue_tex;
NE_Palette *w_portalgun_pal, *portal_orange_pal, *portal_blue_pal;

NE_Material * Debug_Material, * debugempty, * toolstrigger;
NE_Palette * toolstriggerNEPalette;

NE_Material* loadTexture(char* name){
    // load Texture by searching for the Terures mane in textures[] and using the same index for neMaterials[] and nePallettes[]
    for(int i = 0; i < sizeof(textures)/sizeof(MaterialMetadata); i++){
        if (strcmp(textures[i].name, name) == 0){
            if(!alreadyLoadedTextures[i]){
                *neMaterials[i] = NE_MaterialCreate();
                *nePallettes[i] = NE_PaletteCreate();
                NE_MaterialTexLoad(*neMaterials[i], textures[i].textureFormat, textures[i].x, textures[i].y, textures[i].textureFlags, textures[i].textureData);
                // load pallette if necessary
                if(textures[i].numColors >= 0 )
                    {
                        NE_PaletteLoad(*nePallettes[i], textures[i].palletteData, textures[i].numColors, textures[i].textureFormat);
                        NE_MaterialSetPalette(*neMaterials[i], *nePallettes[i]);
                    }
                alreadyLoadedTextures[i] = true;
                // printf("[loadTexture] Loaded %s\n",textures[i].name);
                // printf(" Pallette Memory Used: %d%%\n Texture Memory Used: %d%%\n",100-NE_PaletteFreeMemPercent(), 100-NE_TextureFreeMemPercent());
            }
            return *neMaterials[i];
        }
    }
    return debugempty;
}

// load textures
// @param textureMode   0 = Low Resolution but many diffrent textures
//                      1 = TODO High Resolution but more repeating textures
//                      2 = TODO Portal 1 Textures
void LoadTextures()
{
    Debug_Material = NE_MaterialCreate();
    debugempty = NE_MaterialCreate();
    toolstrigger = NE_MaterialCreate();
    toolstriggerNEPalette = NE_PaletteCreate();
    w_portalgun_tex = NE_MaterialCreate();
    w_portalgun_pal = NE_PaletteCreate();
    portal_orange_tex = NE_MaterialCreate();
    portal_orange_pal = NE_PaletteCreate();
    portal_blue_tex = NE_MaterialCreate();
    portal_blue_pal = NE_PaletteCreate();

    NE_MaterialTexLoad(w_portalgun_tex, NE_PAL16, 128, 128, NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T, (u8*)w_portalgun_tex_bin);
    NE_PaletteLoad(w_portalgun_pal, (void*)w_portalgun_pal_bin, 16, NE_PAL16);
    NE_MaterialSetPalette(w_portalgun_tex, w_portalgun_pal);
    loadTexture("MODELS/ELEVATOR/ELEVATOR_MAIN");

    NE_MaterialTexLoad(portal_orange_tex, NE_PAL4, 32, 64, NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T, portal_orange_tex_bin);
    NE_PaletteLoad(portal_orange_pal, portal_orange_pal_bin, 4, NE_PAL4);
    NE_MaterialSetPalette(portal_orange_tex, portal_orange_pal);
    NE_MaterialTexLoad(portal_blue_tex, NE_PAL4, 32, 64, NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T, portal_blue_tex_bin);
    NE_PaletteLoad(portal_blue_pal, portal_blue_pal_bin, 4, NE_PAL4);
    NE_MaterialSetPalette(portal_blue_tex, portal_blue_pal);

    //transparent
    //NE_MaterialTexLoad(portal_orange_tex, NE_PAL4, 32, 64, NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T | NE_TEXTURE_COLOR0_TRANSPARENT, (u8*)portal_orange_tex_bin);
    //NE_PaletteLoad(portal_orange_pal, (void*)portal_orange_pal_bin, 2, NE_PAL4);
    //NE_MaterialSetPalette(portal_orange_tex, portal_orange_pal);
    //NE_MaterialTexLoad(portal_blue_tex, NE_PAL4, 32, 64, NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T | NE_TEXTURE_COLOR0_TRANSPARENT, (u8*)portal_blue_tex_bin);
    //NE_PaletteLoad(portal_blue_pal, (void*)portal_blue_pal_bin, 2, NE_PAL4);
    //NE_MaterialSetPalette(portal_blue_tex, portal_blue_pal);

    NE_MaterialTexLoad(Debug_Material, NE_A1RGB5, 128, 128, NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T, (u8*)Debug_tex_bin);

    NE_MaterialTexLoad(debugempty, NE_A1RGB5, 64, 64, NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T, (u8*)debugempty_tex_bin);
}

void loadSave() {
    FILE* savefile;
    savefile = fopen("fat:/_nds/PortalDS/save.sav", "rb");
    while (1)
    {
        char word[128];
        int res = fscanf(savefile, "%s", word);
        if (res == EOF) {
            break; // EOF = End Of File. Quit the loop.
        }
        if (strcmp(word, "test") == 0) // Read
        {
            fscanf(savefile, "%s", word);
            printf("\x1b[9;1Hx1:%s", word);
        }
    }
    fclose(savefile);
}

// loads a .vmf file
int loadLevelVmf(char* levelName) {
    FILE* levelFile;
    PLANE tempPlane;
    char* location = "fat:/_nds/PortalDS/levels/";
    char* extension = ".vmf";
    char fileLocation[strlen(location) + strlen(levelName) + strlen(extension) + 1];
    snprintf(fileLocation, sizeof(fileLocation), "%s%s%s", location, levelName, extension);
    if ((levelFile = fopen(fileLocation, "rb")) == NULL) {
        location = "nitro:/levels/";
        snprintf(fileLocation, sizeof(fileLocation), "%s%s%s", location, levelName, extension);
        levelFile = fopen(fileLocation, "rb");
    }
    int i = 0;
    //Plane = (PLANE*)malloc(n * sizeof(PLANE));
    ////Plane = malloc(n * sizeof(PLANE));
    while (1)
    {
        char word[256];
        //tempPlane = (PLANE*)malloc(sizeof(PLANE));
        int res = fscanf(levelFile, "%s", word);
        tempPlane.isDrawn = 1;
        if (res == EOF) {
            break; // EOF = End Of File. Quit the loop.
        }
        if (strcmp(word, "side") == 0) // Read block
        {
            //temporary values (overwritten with evry net object)
            char id[3];
            char tempx1[10], tempy1[10], tempz1[10];
            char tempx2[10], tempy2[10], tempz2[10];
            char tempx3[10], tempy3[10], tempz3[10];
            char tempMaterial[30];
            char tempuaxis[6], tempuscale[6];
            char tempvaxis[6], tempvscale[6];
            fscanf(levelFile, "%*s%*11c%3s%*[\"]", id); //search for the start of the data
            fscanf(levelFile, "%*s%*c\"(%s %s %s)", tempx1, tempy1, tempz1);    //read file values into temporary values
            fscanf(levelFile, " (%s %s %s)", tempx2, tempy2, tempz2);
            fscanf(levelFile, " (%s %s %s)", tempx3, tempy3, tempz3);
            fscanf(levelFile, "%*s%*[^/]/%[^\"]", tempMaterial);
            fscanf(levelFile, "%*s%*s%*s%*s%*s %[^]]] %[^\"]", tempuaxis, tempuscale);
            fscanf(levelFile, "%*s%*s%*s%*s%*s %[^]]] %[^\"]", tempvaxis, tempvscale);
            int planeId = (float)atof(id); //unused
            tempPlane.vertex1.x = (float)atof(tempx1) * LEVEL_SIZE;
            tempPlane.vertex1.y = (float)atof(tempy1) * LEVEL_SIZE;
            tempPlane.vertex1.z = (float)atof(tempz1) * LEVEL_SIZE;
            tempPlane.vertex2.x = (float)atof(tempx2) * LEVEL_SIZE;
            tempPlane.vertex2.y = (float)atof(tempy2) * LEVEL_SIZE;
            tempPlane.vertex2.z = (float)atof(tempz2) * LEVEL_SIZE;
            tempPlane.vertex3.x = (float)atof(tempx3) * LEVEL_SIZE;
            tempPlane.vertex3.y = (float)atof(tempy3) * LEVEL_SIZE;
            tempPlane.vertex3.z = (float)atof(tempz3) * LEVEL_SIZE;
            float uaxis = (float)atof(tempuaxis);
            float uscale = (float)atof(tempuscale);
            float vaxis = (float)atof(tempvaxis);
            float vscale = (float)atof(tempvscale);

            // load apropreate Texture
            tempPlane.material = loadTexture(tempMaterial);
            if (strcmp(tempMaterial, "TOOLSNODRAW") == 0)
                tempPlane.isDrawn = 0;
                //if material not recognized use debug texture
            else {
                tempPlane.material = debugempty;
                if (i >= 3000)
                    break;
                //tempPlane.isDrawn = 0;
            }


            if (tempPlane.isDrawn) {


                //calculate vertex 4 and textures
                tempPlane.x0 = 0;
                tempPlane.y0 = 0;
                tempPlane.x1 = 0;
                tempPlane.y1 = 0;
                bool collisionsAlreadySet = false;
                //get texture size and adjust it (for diffent resolutions)
                int tempTextureFactorMultiply = 0;
                int tempTextureFactorDivide = 0;
                int textureSizeY = NE_TextureGetSizeY(tempPlane.material);
                if (textureSizeY == 32) {
                    tempTextureFactorMultiply = 1;
                    tempTextureFactorDivide = 8;
                }
                if (textureSizeY == 64) {
                    tempTextureFactorMultiply = 2;
                    tempTextureFactorDivide = 8;
                }
                else if (textureSizeY == 128) {
                    tempTextureFactorMultiply = 4;
                    tempTextureFactorDivide = 4;
                }
                else if (textureSizeY == 256) {
                    tempTextureFactorMultiply = 4;
                    tempTextureFactorDivide = 4;
                }
                else if (textureSizeY == 512) {
                    tempTextureFactorMultiply = 4;
                    tempTextureFactorDivide = 1;
                }

                //if floor or cieling
                if (tempPlane.vertex1.z == tempPlane.vertex3.z) {
                    //create vertex x4/y4/z4
                    tempPlane.vertex4.y = tempPlane.vertex3.y;
                    tempPlane.vertex4.x = tempPlane.vertex1.x;
                    tempPlane.vertex4.z = tempPlane.vertex2.z;

                    level.allHitboxes[level.currentHitbox].vertex[0] = tempPlane.vertex1;
                    level.allHitboxes[level.currentHitbox].vertex[1] = tempPlane.vertex2;
                    level.allHitboxes[level.currentHitbox].vertex[2] = tempPlane.vertex3;
                    level.allHitboxes[level.currentHitbox].vertex[3] = tempPlane.vertex4;
                    level.allHitboxes[level.currentHitbox].vertex[7] = tempPlane.vertex1;
                    level.allHitboxes[level.currentHitbox].vertex[6] = tempPlane.vertex2;
                    level.allHitboxes[level.currentHitbox].vertex[5] = tempPlane.vertex3;
                    level.allHitboxes[level.currentHitbox].vertex[4] = tempPlane.vertex4;
                    collisionsAlreadySet = true;
                    //create texture coordinates
                    if (textureMode == 0) {
                        tempPlane.y0 = tempPlane.vertex1.x * -uscale * 2 + uaxis / 2;
                        tempPlane.x0 = tempPlane.vertex3.y * -vscale * 2 + vaxis / 2;
                        tempPlane.y1 = tempPlane.vertex2.x * -uscale * 2 + uaxis / 2;
                        tempPlane.x1 = tempPlane.vertex1.y * -vscale * 2 + vaxis / 2;
                    }
                    if (textureMode == 1) {
                        tempPlane.y0 = tempPlane.vertex1.x * -uscale * 4 + uaxis / 2;
                        tempPlane.x0 = tempPlane.vertex3.y * -vscale * 4 + vaxis / 2;
                        tempPlane.y1 = tempPlane.vertex2.x * -uscale * 4 + uaxis / 2;
                        tempPlane.x1 = tempPlane.vertex1.y * -vscale * 4 + vaxis / 2;
                    }
                    //if cieling
                    if (tempPlane.vertex2.y == tempPlane.vertex3.y) {
                        //create vertex x4/y4/z4
                        tempPlane.vertex4.y = tempPlane.vertex1.y;
                        tempPlane.vertex4.x = tempPlane.vertex3.x;
                        tempPlane.vertex4.z = tempPlane.vertex2.z;
                        //create texture coordinates
                        if (textureMode == 0) {
                            tempPlane.y0 = tempPlane.vertex1.y * -uscale * 2 + uaxis / 2;
                            tempPlane.x0 = tempPlane.vertex3.x * -vscale * 2 + vaxis / 2;
                            tempPlane.y1 = tempPlane.vertex2.y * -uscale * 2 + uaxis / 2;
                            tempPlane.x1 = tempPlane.vertex1.x * -vscale * 2 + vaxis / 2;
                        }
                        if (textureMode == 1) {
                            tempPlane.y0 = tempPlane.vertex1.y * -uscale * 4 + uaxis / 2;
                            tempPlane.x0 = tempPlane.vertex3.x * -vscale * 4 + vaxis / 2;
                            tempPlane.y1 = tempPlane.vertex2.y * -uscale * 4 + uaxis / 2;
                            tempPlane.x1 = tempPlane.vertex1.x * -vscale * 4 + vaxis / 2;
                        }
                    }
                }
                //if wall is on the y axis
                else if (tempPlane.vertex1.x == tempPlane.vertex2.x) {
                    //create vertex x4/y4/z4
                    tempPlane.vertex4.y = tempPlane.vertex1.y;
                    tempPlane.vertex4.x = tempPlane.vertex3.x;
                    tempPlane.vertex4.z = tempPlane.vertex3.z;
                    //tempPlane.nx = tempPlane.vertex2.x + 128;
                    //tempPlane.ny = tempPlane.vertex2.y;
                    //tempPlane.nz = (tempPlane.vertex2.z - tempPlane.vertex3.z) / 2;
                    //create texture coordinates
                    if (textureMode == 0) {
                        tempPlane.y0 = tempPlane.vertex1.y * -uscale * 2 + uaxis / 2;
                        tempPlane.x0 = tempPlane.vertex3.z * -vscale * 2 + vaxis / 2 + 32;
                        tempPlane.y1 = tempPlane.vertex2.y * -uscale * 2 + uaxis / 2;
                        tempPlane.x1 = tempPlane.vertex1.z * -vscale * 2 + vaxis / 2 + 32;
                    }
                    else if (textureMode == 1) {
                        tempPlane.y0 = tempPlane.vertex1.y * -uscale * 4 + uaxis / 2;
                        tempPlane.x0 = tempPlane.vertex3.z * -vscale * 4 + vaxis / 2;
                        tempPlane.y1 = tempPlane.vertex2.y * -uscale * 4 + uaxis / 2;
                        tempPlane.x1 = tempPlane.vertex1.z * -vscale * 4 + vaxis / 2;
                    }
                }
                //if wall is on the x axis
                else if (tempPlane.vertex2.y == tempPlane.vertex3.y) {
                    //create vertex x4/y4/z4
                    tempPlane.vertex4.y = tempPlane.vertex3.y;
                    tempPlane.vertex4.x = tempPlane.vertex1.x;
                    tempPlane.vertex4.z = tempPlane.vertex3.z;

                    //create texture coordinates
                    if (textureMode == 0) {
                        tempPlane.y0 = tempPlane.vertex1.x * -uscale * 2 + uaxis / 2 + 32;
                        tempPlane.x0 = tempPlane.vertex3.z * -vscale * 2 + vaxis / 2 + 32;
                        tempPlane.y1 = tempPlane.vertex2.x * -uscale * 2 + uaxis / 2 + 32;
                        tempPlane.x1 = tempPlane.vertex1.z * -vscale * 2 + vaxis / 2 + 32;
                    }
                    else if (textureMode == 1) {
                        tempPlane.y0 = tempPlane.vertex1.x * -uscale * 4 + uaxis / 2;
                        tempPlane.x0 = tempPlane.vertex3.z * -vscale * 4 + vaxis / 2;
                        tempPlane.y1 = tempPlane.vertex2.x * -uscale * 4 + uaxis / 2;
                        tempPlane.x1 = tempPlane.vertex1.z * -vscale * 4 + vaxis / 2;
                    }
                }

                level.Plane[i] = tempPlane;
                level.allHitboxes[level.currentHitbox].isPlane = true;
                level.allHitboxes[level.currentHitbox].isDynamic = false;
                if (!collisionsAlreadySet) {
                    level.allHitboxes[level.currentHitbox].vertex[0] = tempPlane.vertex1;
                    level.allHitboxes[level.currentHitbox].vertex[1] = tempPlane.vertex2;
                    level.allHitboxes[level.currentHitbox].vertex[6] = tempPlane.vertex3;
                    level.allHitboxes[level.currentHitbox].vertex[7] = tempPlane.vertex4;
                    level.allHitboxes[level.currentHitbox].vertex[3] = tempPlane.vertex1;
                    level.allHitboxes[level.currentHitbox].vertex[2] = tempPlane.vertex2;
                    level.allHitboxes[level.currentHitbox].vertex[5] = tempPlane.vertex3;
                    level.allHitboxes[level.currentHitbox].vertex[4] = tempPlane.vertex4;
                }
                level.allHitboxes[level.currentHitbox].attachedPlane = &level.Plane[i];
                level.currentHitbox++;
                //printf("\nplane id:%d\nmaterial:%s\nx1:%s y1:%.0f z1:%.0f\nx2:%.0f y2:%.0f z2:%.0f\nx3:%.0f y3:%.0f z3:%.0f\n", planeId, tempMaterial, tempx1, level.Plane[i].vertex1.y, level.Plane[i].vertex1.z, level.Plane[i].vertex2.x, level.Plane[i].vertex2.y, level.Plane[i].vertex2.z, level.Plane[i].vertex3.x, level.Plane[i].vertex3.y, level.Plane[i].vertex3.z);
                //printf("\nplane id:%d\nuaxis:%.0f uscale:%.2f\nvaxis:%.0f vscale:%.2f\n", planeId, level.Plane[i].uaxis, level.Plane[i].uscale, level.Plane[i].vaxis, level.Plane[i].vscale);
                
                i++;
                //break;
            }
        }
        else if (strcmp(word, "\"info_player_start\"") == 0) // Read Player spawn
        {
            char tmpx[8];
            char tmpy[8];
            char tmpz[8];
            fscanf(levelFile, "%*31c%s %s %s", tmpx, tmpy, tmpz);
            //set the player positon
            localPlayer.position.x = (float)atof(tmpx) * LEVEL_SIZE;
            localPlayer.position.y = (float)atof(tmpy) * LEVEL_SIZE;
            localPlayer.position.z = (float)atof(tmpz) * LEVEL_SIZE;
        }
    }
    if (i >= MAX_PLANES)
        printf("Warning max Planes reached:%d", i);

    fclose(levelFile);

    level.planeCount = i;
    level.name = levelName;

    return 0;
}

// loads a .bsp file
int loadLevelBsp(char* levelName) {
    FILE* levelFile;
    PLANE tempPlane;
    char* location = "fat:/_nds/PortalDS/levels/";
    char* extension = ".bsp";
    char fileLocation[strlen(location) + strlen(levelName) + strlen(extension) + 1];
    snprintf(fileLocation, sizeof(fileLocation), "%s%s%s", location, levelName, extension);
    if ((levelFile = fopen(fileLocation, "rb")) == NULL) {
        location = "nitro:/levels/";
        snprintf(fileLocation, sizeof(fileLocation), "%s%s%s", location, levelName, extension);
        levelFile = fopen(fileLocation, "rb");
    }


    // printf("[load] %s\n", levelName);
    level.name = levelName;
    printMemory(__FILE__, __func__, __LINE__);

    // Read the VBSP Header
    struct dheader_t header;
    fread(&header, 1, sizeof(struct dheader_t), levelFile);
    // Check the file identifier
    if(header.ident != IDBSPHEADER)
        printf("ERROR: Wrong File header\n");
    // Check the VBSP version (Portal 2 = 21)
    if(header.version != 21)
        printf("WARNING: Wrong BSP version\n");
    
    // Read faces
    struct dface_t faceLump[header.lumps[LUMP_FACES].filelen / sizeof(struct dface_t)];
    // struct dface_t faceLump[MAX_MAP_FACES];
    fseek(levelFile, header.lumps[LUMP_FACES].fileofs, SEEK_SET);
    fread(&faceLump, header.lumps[LUMP_FACES].filelen, 1, levelFile);

    // Read original faces
    struct dface_t originalFaceLump[header.lumps[LUMP_ORIGINALFACES].filelen / sizeof(struct dface_t)];
    // struct dface_t originalFaceLump[MAX_MAP_FACES];
    fseek(levelFile, header.lumps[LUMP_ORIGINALFACES].fileofs, SEEK_SET);
    fread(&originalFaceLump, header.lumps[LUMP_ORIGINALFACES].filelen, 1, levelFile);

    // Read edges
    struct dedge_t edgeLump[header.lumps[LUMP_EDGES].filelen / sizeof(struct dedge_t)];
    // struct dedge_t edgeLump[MAX_MAP_EDGES];
    fseek(levelFile, header.lumps[LUMP_EDGES].fileofs, SEEK_SET);
    fread(&edgeLump, header.lumps[LUMP_EDGES].filelen, 1, levelFile);

    // Read surfEdges
    int* surfedgeLump = malloc(header.lumps[LUMP_SURFEDGES].filelen);
    fseek(levelFile, header.lumps[LUMP_SURFEDGES].fileofs, SEEK_SET);
    fread(surfedgeLump, header.lumps[LUMP_SURFEDGES].filelen, 1, levelFile);

    // Read vertecies
    struct dvertex_t vertexLump[header.lumps[LUMP_VERTEXES].filelen / sizeof(struct dvertex_t)];
    // struct dvertex_t vertexLump[MAX_MAP_VERTS];
    fseek(levelFile, header.lumps[LUMP_VERTEXES].fileofs, SEEK_SET);
    fread(&vertexLump, header.lumps[LUMP_VERTEXES].filelen, 1, levelFile);


    // Read textureinfo
    struct texinfo_s texinfoLump[header.lumps[LUMP_TEXINFO].filelen / sizeof(struct texinfo_s)];
    // struct texinfo_s texinfoLump[MAX_MAP_TEXINFO];
    fseek(levelFile, header.lumps[LUMP_TEXINFO].fileofs, SEEK_SET);
    fread(&texinfoLump, header.lumps[LUMP_TEXINFO].filelen, 1, levelFile);

    // Read texturedata
    struct dtexdata_t texdataLump[header.lumps[LUMP_TEXDATA].filelen / sizeof(struct dtexdata_t)];
    // struct dtexdata_t texdataLump[MAX_MAP_TEXDATA];
    fseek(levelFile, header.lumps[LUMP_TEXDATA].fileofs, SEEK_SET);
    fread(&texdataLump, header.lumps[LUMP_TEXDATA].filelen, 1, levelFile);

    // Read texture name table
    int* texdataStringTable = malloc(header.lumps[LUMP_TEXDATA_STRING_TABLE].filelen);
    // struct dtexdata_t texdataLump[MAX_MAP_TEXDATA];
    fseek(levelFile, header.lumps[LUMP_TEXDATA_STRING_TABLE].fileofs, SEEK_SET);
    fread(texdataStringTable, header.lumps[LUMP_TEXDATA_STRING_TABLE].filelen, 1, levelFile);

    // Read texture name strings
    char* texdataStrings = malloc(header.lumps[LUMP_TEXDATA_STRING_DATA].filelen);
    // struct dtexdata_t texdataLump[MAX_MAP_TEXDATA];
    fseek(levelFile, header.lumps[LUMP_TEXDATA_STRING_DATA].fileofs, SEEK_SET);
    fread(texdataStrings, header.lumps[LUMP_TEXDATA_STRING_DATA].filelen, 1, levelFile);


    // every face

    int planesToLoad = min(header.lumps[LUMP_FACES].filelen / sizeof(struct dface_t), MAX_PLANES);
    // face=plane in level file; plane=plane ingame(excluded nodraw)
    int plane = 0;
    for (int face = 0; face < planesToLoad + (planesToLoad-plane); face++)
    {
        int firstEdge = originalFaceLump[faceLump[face].origFace].firstedge;
        int nextEdge = originalFaceLump[faceLump[face].origFace].firstedge + 2;
        if(faceLump[face].numedges >= 4){
            if(surfedgeLump[firstEdge] > 0){
                level.Plane[plane].vertex1 = vertexLump[edgeLump[abs(surfedgeLump[firstEdge])].v[0]].point;
                level.Plane[plane].vertex2 = vertexLump[edgeLump[abs(surfedgeLump[firstEdge])].v[1]].point;
            }else{
                level.Plane[plane].vertex1 = vertexLump[edgeLump[abs(surfedgeLump[firstEdge])].v[1]].point;
                level.Plane[plane].vertex2 = vertexLump[edgeLump[abs(surfedgeLump[firstEdge])].v[0]].point;
            }
            
            if(surfedgeLump[nextEdge] > 0){
                level.Plane[plane].vertex3 = vertexLump[edgeLump[abs(surfedgeLump[nextEdge])].v[0]].point;
                level.Plane[plane].vertex4 = vertexLump[edgeLump[abs(surfedgeLump[nextEdge])].v[1]].point;
            }else{
                level.Plane[plane].vertex3 = vertexLump[edgeLump[abs(surfedgeLump[nextEdge])].v[1]].point;
                level.Plane[plane].vertex4 = vertexLump[edgeLump[abs(surfedgeLump[nextEdge])].v[0]].point;
            }
        }

        level.Plane[plane].isDrawn = true;
        // if(faceLump[face].firstedge == faceLump[310].firstedge){
        //     level.Plane[plane].isDrawn = true;
        //     printf("%.f %.f %.f\n%.f %.f %.f\n%.f %.f %.f\n%.f %.f %.f\n",
        //         level.Plane[plane].vertex1.x,
        //         level.Plane[plane].vertex1.y,
        //         level.Plane[plane].vertex1.z,
        //         level.Plane[plane].vertex2.x,
        //         level.Plane[plane].vertex2.y,
        //         level.Plane[plane].vertex2.z,
        //         level.Plane[plane].vertex3.x,
        //         level.Plane[plane].vertex3.y,
        //         level.Plane[plane].vertex3.z,
        //         level.Plane[plane].vertex4.x,
        //         level.Plane[plane].vertex4.y,
        //         level.Plane[plane].vertex4.z
        //     );
        //     for (int i = 0; i < 10; i++)
        //     {
        //         // printf("%f\n",dot(vertexLump[edgeLump[abs(surfedgeLump[firstEdge+i])].v[1]].point, vertexLump[edgeLump[abs(surfedgeLump[firstEdge+i+1])].v[1]].point));
        //         // printf("%d\n",vertexLump[edgeLump[abs(surfedgeLump[faceLump[face].firstedge + i])].v[0]].point);
        //     }
        //     printf("%d",faceLump[face].numedges);
        //     // printf("%d\n%d\n%d",surfedgeLump[faceLump[face].firstedge + 0],surfedgeLump[faceLump[face].firstedge + 1],surfedgeLump[faceLump[face].firstedge + 2]);
        // }

        
        // UV
        Vector3 tv[2];
        tv[0].x = texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[0][0];
        tv[0].y = texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[0][1];
        tv[0].z = texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[0][2];
        tv[1].x = texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[1][0];
        tv[1].y = texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[1][1];
        tv[1].z = texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[1][2];
        level.Plane[plane].x1 = (dot(tv[1], level.Plane[plane].vertex1) + texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[1][3]) / 8;
        level.Plane[plane].y1 = (dot(tv[0], level.Plane[plane].vertex1) + texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[0][3]) / 8;
        level.Plane[plane].x0 = (dot(tv[1], level.Plane[plane].vertex3) + texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[1][3]) / 8;
        level.Plane[plane].y0 = (dot(tv[0], level.Plane[plane].vertex3) + texinfoLump[faceLump[face].texinfo].textureVecsTexelsPerWorldUnits[0][3]) / 8;


        // printf("%s\n",texdataStrings + texdataStringTable[texdataLump[texinfoLump[faceLump[face].texinfo].texdata].nameStringTableID]);
        // printf("%d ",texdataLump[texinfoLump[faceLump[face].texinfo].texdata].nameStringTableID);
        // printf("%d ",surfedgeLump[faceLump[face].firstedge + 0]);

        // char* tempMaterial = malloc(strlen(texdataStrings + texdataStringTable[texdataLump[texinfoLump[faceLump[face].texinfo].texdata].nameStringTableID]));
        // strcpy(tempMaterial, texdataStrings + texdataStringTable[texdataLump[texinfoLump[faceLump[face].texinfo].texdata].nameStringTableID]);
        char* tempMaterial = texdataStrings + texdataStringTable[texdataLump[texinfoLump[faceLump[face].texinfo].texdata].nameStringTableID];

        // load apropreate Texture
        level.Plane[plane].material = loadTexture(tempMaterial);
        // printf("%s\n",tempMaterial);
        if (strcmp(tempMaterial, "TOOLS/TOOLSTRIGGER") == 0) {
            // level.Plane[plane].material = toolstrigger;
            level.Plane[plane].x0 = 0;
            level.Plane[plane].y0 = 0;
            level.Plane[plane].x1 = 255;
            level.Plane[plane].y1 = 255;
        }
        else if (strcmp(tempMaterial, "TOOLS/TOOLSNODRAW") == 0){
            level.Plane[plane].isDrawn = 0;
            continue;
        }

        

        // Floor UV rotated??? todo: find out why
        // quick fix
        if (level.Plane[plane].vertex1.z == level.Plane[plane].vertex3.z) {
            int tmpX0 = level.Plane[plane].x0;
            int tmpX1 = level.Plane[plane].x1;
            level.Plane[plane].x0 = level.Plane[plane].y0;
            level.Plane[plane].x1 = level.Plane[plane].y1;
            level.Plane[plane].y0 = tmpX0;
            level.Plane[plane].y1 = tmpX1;
        }

        plane++;
    }

    printMemory(__FILE__, __func__, __LINE__);

    // Read models
    struct dmodel_t modelLump[header.lumps[LUMP_MODELS].filelen / sizeof(struct dmodel_t)];
    // struct dmodel_t faceLump[MAX_MAP_MODELS];
    fseek(levelFile, header.lumps[LUMP_MODELS].fileofs, SEEK_SET);
    fread(&modelLump, header.lumps[LUMP_MODELS].filelen, 1, levelFile);

    // Read Entitys
    fseek(levelFile, header.lumps[LUMP_ENTITIES].fileofs, SEEK_SET);
    while (1)
    {
        char word[256];
        int res = fscanf(levelFile, "%s", word);
        tempPlane.isDrawn = 1;
        fpos_t fpos;
        fpos_t fStartPos;
        fgetpos(levelFile, &fpos);
        if (res == EOF || (long)fpos >= (header.lumps[LUMP_ENTITIES].fileofs + header.lumps[LUMP_ENTITIES].filelen)) {
            break; // EOF = End Of File. Quit the loop.
        }
        if (strcmp(word, "{") == 0)
        {
            fgetpos(levelFile, &fStartPos);
            
        }
        
        if (strcmp(word, "\"info_player_start\"") == 0){ // Read Player spawn
            // remember current cursor position
            fgetpos(levelFile, &fpos);
            // go to the position value
            fsetpos(levelFile, &fStartPos);
            while(strcmp(word, "}"))
            {
                fscanf(levelFile, "%s", word);
                if(strcmp(word, "\"origin\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    break;
                }
            }
            
            // fseek(levelFile, -88, SEEK_CUR);
            float tmpx, tmpy, tmpz;
            fscanf(levelFile, "%f %f %f", &tmpx, &tmpy, &tmpz);
            // Set the player positon
            localPlayer.position.x = tmpx;
            localPlayer.position.y = tmpy;
            localPlayer.position.z = tmpz;

            // localPlayer.position.x = -3000;
            // localPlayer.position.y = -1000;
            // localPlayer.position.z = 100;
            // Read worldspawn
            Vector3 playerStartPosition;
            Vector3 playerStartRotation;
            printf("info_player_start:\n%f\n%f\n%f\n", tmpx, tmpy, tmpz);
            // go to where we left off
            fsetpos(levelFile, &fpos);
            printMemory(__FILE__, __func__, __LINE__);
        }
        if (strcmp(word, "\"trigger_once\"") == 0){ // Read Trigger_once
            char model[64];
            int modelIndex;
            Vector3 position;
            char name[64], input[64];
            // remember current cursor position
            fgetpos(levelFile, &fpos);
            // go to the position value
            fsetpos(levelFile, &fStartPos);
            while(strcmp(word, "}"))
            {
                fscanf(levelFile, "%s", word);
                if(strcmp(word, "\"model\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%s", &model);
                    model[strlen(model) - 1] = 0; // remove the "
                    if(model[0] == '*'){
                        // remove the *
                        for (int i = 0; i < 63; i++)
                        {
                            model[i] = model[i+1];
                        }
                        modelIndex = atoi(model);
                    }
                }
                if(strcmp(word, "\"origin\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &position.x, &position.y, &position.z);
                }
                if(strcmp(word, "\"targetname\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &name);
                }else
                if(strcmp(word, "\"OnStartTouch\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &input);
                    // printf("%s\n",input);
                    break;

                }
                // printf("%s",word);
            }
            // printf("trigger_once:\n%f\n%f\n%f\n", position.x, position.y, position.z);
            // printf("trigger_once:\n%d\n", modelIndex);
            Vector3 size = {
                .x = modelLump[modelIndex].maxs.x - modelLump[modelIndex].mins.x,
                .y = modelLump[modelIndex].maxs.y - modelLump[modelIndex].mins.y,
                .z = modelLump[modelIndex].maxs.z - modelLump[modelIndex].mins.z
            };
            Vector3 rotation = {
                .x = 0,
                .y = 0,
                .z = 0,
            };
            // printf("trigger_once:\n%f\n%f\n%f\n", size.x, size.y, size.z);
            //  position.x = 256;
            //  position.y = 0;
            //  position.z = 0;

            char* namePermanent = malloc(sizeof(char) * (strlen(name) + 1));
            strcpy(namePermanent, name);

            addHitbox(size,&position, &rotation, false);
            level.allHitboxes[level.currentHitbox].attachedTrigger = malloc(sizeof(trigger));
            level.allHitboxes[level.currentHitbox].attachedTrigger->input = (char*)malloc((strlen(input) + 1) * sizeof(char));
            strcpy(level.allHitboxes[level.currentHitbox].attachedTrigger->input, input);
            
            level.allHitboxes[level.currentHitbox].isTrigger = true;
            level.allHitboxes[level.currentHitbox].attachedTrigger->mode = 1; // set mode to trigger_once
            level.allHitboxes[level.currentHitbox].attachedTrigger->alreadyTriggered = false;

            registerEntity("trigger_once", namePermanent, level.allHitboxes[level.currentHitbox].attachedTrigger, position, rotation);

            //printf("\n\n%s\n%s\n\n\n", level.allHitboxes[level.currentHitbox].attachedTrigger->input, input);
            // printf("%d", level.currentHitbox);
            // go to where we left off
            // fsetpos(levelFile, &fpos);

            printMemory(__FILE__, __func__, __LINE__);
        } 
        if (strcmp(word, "\"point_teleport\"") == 0){ // Read point_teleport
            // remember current cursor position
            fgetpos(levelFile, &fpos);
            // go to the position value
            fsetpos(levelFile, &fStartPos);
            Vector3 position, rotation;
            char name[64], target[32];
            while(strcmp(word, "}"))
            {
                fscanf(levelFile, "%s", word);
                if(strcmp(word, "\"origin\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &position.x, &position.y, &position.z);
                }else
                if(strcmp(word, "\"targetname\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &name);
                }else
                if(strcmp(word, "\"target\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &target);
                }else
                if(strcmp(word, "\"angles\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &rotation.x, &rotation.y, &rotation.z);
                }
            }
            
            char* namePermanent = malloc(sizeof(char) * (strlen(name) + 1));
            strcpy(namePermanent, name);
            char* targetPermanent = malloc(sizeof(char) * (strlen(target) + 1));
            strcpy(targetPermanent, target);

            pointTeleport* entity = malloc(sizeof(pointTeleport));
            entity->target = targetPermanent;
            // printf("\n\n%s\n\n\n",name);
            // printf("\n\n%s\n\n\n",target);
            registerEntity("point_teleport", namePermanent, entity, position, rotation);
            
            // go to where we left off
            fsetpos(levelFile, &fpos);
            printMemory(__FILE__, __func__, __LINE__);
        }
        if (strcmp(word, "\"logic_script\"") == 0){ // Read logic_script
            // remember current cursor position
            fgetpos(levelFile, &fpos);
            // go to the position value
            fsetpos(levelFile, &fStartPos);
            Vector3 position, rotation;
            char name[64], script[32];
            while(strcmp(word, "}"))
            {
                fscanf(levelFile, "%s", word);
                if(strcmp(word, "\"origin\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &position.x, &position.y, &position.z);
                }else
                if(strcmp(word, "\"targetname\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &name);
                }else
                if(strcmp(word, "\"vscripts\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &script);
                }else
                if(strcmp(word, "\"angles\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &rotation.x, &rotation.y, &rotation.z);
                }
            }
            
            // load the script file
            char* location = "nitro:/scripts/vscripts/";
            snprintf(fileLocation, strlen(location)+strlen(script)+1, "%s%s", location, script);
            
            // check if file exists and load it
            FILE *file;
            if ((file = fopen(fileLocation, "r")))
            {
                fclose(file);
                sqstd_dofile(squirrelvm, fileLocation, false, false);
    
                char* namePermanent = malloc(sizeof(char) * (strlen(name) + 1));
                strcpy(namePermanent, name);
    
                // printf("\n\n%s\n\n\n",fileLocation);
                registerEntity("logic_script", namePermanent, NULL, position, rotation);
                
            }
            // go to where we left off
            fsetpos(levelFile, &fpos);
            printMemory(__FILE__, __func__, __LINE__);
        }
        if (strcmp(word, "\"func_tracktrain\"") == 0){ // Read func_tracktrain
            // remember current cursor position
            fgetpos(levelFile, &fpos);
            // go to the position value
            fsetpos(levelFile, &fStartPos);
            Vector3 position, rotation;
            int startSpeed;
            char name[64], target[32];
            while(strcmp(word, "}"))
            {
                fscanf(levelFile, "%s", word);
                if(strcmp(word, "\"origin\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &position.x, &position.y, &position.z);
                }else
                if(strcmp(word, "\"targetname\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &name);
                }else
                if(strcmp(word, "\"target\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &target);
                }else
                if(strcmp(word, "\"angles\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &rotation.x, &rotation.y, &rotation.z);
                }else
                if(strcmp(word, "\"startspeed\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%d", startSpeed);
                }
            }

            char* namePermanent = malloc(sizeof(char) * (strlen(name) + 1));
            strcpy(namePermanent, name);

            funcTracktrain* entity = malloc(sizeof(funcTracktrain));
            entity->startSpeed = startSpeed;

            registerEntity("func_tracktrain", namePermanent, entity, position, rotation);
            
            // go to where we left off
            fsetpos(levelFile, &fpos);
            printMemory(__FILE__, __func__, __LINE__);
        }
        if (strcmp(word, "\"path_track\"") == 0){ // Read path_track
            // remember current cursor position
            fgetpos(levelFile, &fpos);
            // go to the position value
            fsetpos(levelFile, &fStartPos);
            Vector3 position, rotation;
            char name[64], target[32];
            while(strcmp(word, "}"))
            {
                fscanf(levelFile, "%s", word);
                if(strcmp(word, "\"origin\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &position.x, &position.y, &position.z);
                }else
                if(strcmp(word, "\"targetname\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &name);
                }else
                if(strcmp(word, "\"target\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &target);
                }else
                if(strcmp(word, "\"angles\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &rotation.x, &rotation.y, &rotation.z);
                }
            }

            char* namePermanent = malloc(sizeof(char) * (strlen(name) + 1));
            strcpy(namePermanent, name);

            registerEntity("path_track", namePermanent, NULL, position, rotation);
            
            // go to where we left off
            fsetpos(levelFile, &fpos);
            printMemory(__FILE__, __func__, __LINE__);
        }
        if (strcmp(word, "\"prop_dynamic\"") == 0){ // Read prop_dynamic
            // remember current cursor position
            fgetpos(levelFile, &fpos);
            // go to the position value
            fsetpos(levelFile, &fStartPos);
            Vector3 position, rotation;
            char name[64], target[32], model[64];
            while(strcmp(word, "}"))
            {
                fscanf(levelFile, "%s", word);
                if(strcmp(word, "\"origin\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &position.x, &position.y, &position.z);
                }else
                if(strcmp(word, "\"targetname\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &name);
                }else
                if(strcmp(word, "\"target\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &target);
                }else
                if(strcmp(word, "\"angles\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%f %f %f", &rotation.x, &rotation.y, &rotation.z);
                }else
                if(strcmp(word, "\"model\"") == 0){
                    fseek(levelFile, 1, SEEK_CUR);
                    fscanf(levelFile, "%[^\"]", &model);
                }
            }

            char* namePermanent = malloc(sizeof(char) * (strlen(name) + 1));
            strcpy(namePermanent, name);
            
            // printf("%s\n",model);
            // Model* entity = malloc(sizeof(Model));
            // entity->position = position;
            // entity->rotation = rotation;
            // TODO: include modelname in entity
            registerEntity("prop_dynamic", namePermanent, NULL, position, rotation);

            // go to where we left off
            fsetpos(levelFile, &fpos);
            printMemory(__FILE__, __func__, __LINE__);
        }
    }
    

    // printf("%d\n", faceLump[0].numedges);
    level.planeCount = plane;
    
    fclose(levelFile);
}

void LoadMisc (void)
{
    // Portalgun
    w_portalgun_model = NE_ModelCreate(NE_Animated);
    w_portalgun_fire1_animation = NE_AnimationCreate();
    NE_AnimationLoad(w_portalgun_fire1_animation, w_portalgun_fire1_dsa_bin);
    NE_ModelLoadDSM(w_portalgun_model, w_portalgun_dsm_bin);
    NE_ModelSetAnimation(w_portalgun_model, w_portalgun_fire1_animation);
    NE_ModelSetMaterial(w_portalgun_model, w_portalgun_tex);
    NE_ModelScaleI(w_portalgun_model, floatToFixed(3, LEVEL_RENDER_SIZE), floatToFixed(3, LEVEL_RENDER_SIZE), floatToFixed(3, LEVEL_RENDER_SIZE));
    NE_ModelSetCoord(w_portalgun_model, 0, 0.1, 0);

    // Entrance/Exit Elevator
    elevator_b_model = NE_ModelCreate(NE_Animated);
    elevator_b_doorsopen = NE_AnimationCreate();
    elevator_b_doorsclose = NE_AnimationCreate();
    NE_AnimationLoad(elevator_b_doorsopen, elevator_b_elevator_b_doorsopen_dsa_bin);
    NE_AnimationLoad(elevator_b_doorsclose, elevator_b_elevator_b_doorsclose_dsa_bin);
    NE_ModelLoadDSM(elevator_b_model, elevator_b_dsm_bin);
    NE_ModelSetAnimation(elevator_b_model, elevator_b_doorsopen);
    NE_ModelSetMaterial(elevator_b_model, NEMaterial_models_elevator_elevator_main);
    NE_ModelScaleI(elevator_b_model, floatToFixed(50, LEVEL_RENDER_SIZE), floatToFixed(50, LEVEL_RENDER_SIZE), floatToFixed(50, LEVEL_RENDER_SIZE));

    portal_orange_model = NE_ModelCreate(NE_Static);
    NE_ModelLoadStaticMesh(portal_orange_model, (u32 *)portal_bin);
    NE_ModelSetMaterial(portal_orange_model, portal_orange_tex);
    NE_ModelScaleI(portal_orange_model, floatToFixed(55,LEVEL_RENDER_SIZE), floatToFixed(55, LEVEL_RENDER_SIZE), floatToFixed(55, LEVEL_RENDER_SIZE));
    NE_ModelSetCoord(portal_orange_model, 0, -1, 0);

    portal_blue_model = NE_ModelCreate(NE_Static);
    NE_ModelLoadStaticMesh(portal_blue_model, (u32 *)portal_bin);
    NE_ModelSetMaterial(portal_blue_model, portal_blue_tex);
    NE_ModelScaleI(portal_blue_model, floatToFixed(55, LEVEL_RENDER_SIZE), floatToFixed(55, LEVEL_RENDER_SIZE), floatToFixed(55, LEVEL_RENDER_SIZE));
    NE_ModelSetCoord(portal_blue_model, 0, -1, 0);

    if (debugVision) {
        debug_vision_model = NE_ModelCreate(NE_Static);
        NE_ModelLoadStaticMesh(debug_vision_model, (u32 *)Debug_sphere_bin);
        NE_ModelSetMaterial(debug_vision_model, debugempty);
        NE_ModelScaleI(debug_vision_model, floatToFixed(10, LEVEL_RENDER_SIZE), floatToFixed(10, LEVEL_RENDER_SIZE), floatToFixed(10, LEVEL_RENDER_SIZE));
    }
}