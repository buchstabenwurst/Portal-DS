#include <NEMain.h>
#include <stdio.h>
#include <math.h>
#include <nds/arm9/trig_lut.h>
#include "main.h"
#include "draw3d.h"
#include "load.h"
#include "physics.h"


NE_Camera* Camara;

//move the Camera on the Global axis
void CameraMoveGlobal(NE_Camera *cam, PLAYER player) {
    float tmpTo[3];
    tmpTo[0] = player.position.x / (1 << 12 - LEVEL_RENDER_SIZE) + fixedToFloat(sinLerp(floatToFixed(player.rotation.y / 45, 12)) / 3 * fixedToFloat(cosLerp(floatToFixed(player.rotation.z / 45, 12)), 12), 12);
    tmpTo[1] = player.position.z / (1 << 12 - LEVEL_RENDER_SIZE) + fixedToFloat(sinLerp(floatToFixed(player.rotation.z / 45, 12)), 21) * 180;
    tmpTo[2] = player.position.y / (1 << 12 - LEVEL_RENDER_SIZE) + fixedToFloat(cosLerp(floatToFixed(player.rotation.y / 45, 12)) / 3 * fixedToFloat(cosLerp(floatToFixed(player.rotation.z / 45, 12)), 12), 12);

    NE_AssertPointer(cam, "NULL pointer");

    cam->matrix_is_updated = false;

    cam->from[0] = floatToFixed(player.position.x, LEVEL_RENDER_SIZE);
    cam->from[1] = floatToFixed(player.position.z, LEVEL_RENDER_SIZE);
    cam->from[2] = floatToFixed(player.position.y, LEVEL_RENDER_SIZE);

    cam->to[0] = floattof32(tmpTo[0]);
    cam->to[1] = floattof32(tmpTo[1]); //up and down
    cam->to[2] = floattof32(tmpTo[2]); //left and right

    localPlayer.lookVector.x = tmpTo[0] - player.position.x / (1 << 12 - LEVEL_RENDER_SIZE);
    localPlayer.lookVector.z = tmpTo[1] - player.position.z / (1 << 12 - LEVEL_RENDER_SIZE);
    localPlayer.lookVector.y = tmpTo[2] - player.position.y / (1 << 12 - LEVEL_RENDER_SIZE);
    
    localPlayer.lookVector = Normalize(localPlayer.lookVector);

    //portal gun
    NE_ModelSetRot(w_portalgun_model, 0,
                                        player.rotation.y / DERGEESTO511,
                                        0);
    NE_ModelSetCoord(w_portalgun_model, localPlayer.position.x / (1 << 12 - LEVEL_RENDER_SIZE) + fixedToFloat(sinLerp(floatToFixed(player.rotation.y / 45 - 50, 12)), 12) / 20,
                                        localPlayer.position.z / (1 << 12 - LEVEL_RENDER_SIZE) - 0.1 * LEVEL_SIZE,
                                        localPlayer.position.y / (1 << 12 - LEVEL_RENDER_SIZE) + fixedToFloat(cosLerp(floatToFixed(player.rotation.y / 45 - 50, 12)), 12) / 20);
    NE_ModelDraw(w_portalgun_model);

    if (debugVision) {
        NE_ModelRotate(debug_vision_model, 1, 2, 3);
        NE_ModelSetCoord(debug_vision_model, tmpTo[0], tmpTo[1], tmpTo[2]);
        NE_ModelDraw(debug_vision_model);
    }
}

void renderPortals(void){
    for(int i = 0; i < 2; i++){
        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, 0);

        NE_ModelSetCoord(i ? portal_blue_model : portal_orange_model, level.portal[i].position.x / (1 << 12 - LEVEL_RENDER_SIZE), level.portal[i].position.z / (1 << 12 - LEVEL_RENDER_SIZE), level.portal[i].position.y / (1 << 12 - LEVEL_RENDER_SIZE));
        NE_ModelSetRot(i ? portal_blue_model : portal_orange_model, 0, level.portal[i].rotation.y / DERGEESTO511, level.portal[i].rotation.z / DERGEESTO511);
        NE_ModelDraw(i ? portal_blue_model : portal_orange_model);
        
        if(level.portal[i].portaledPlane.plane == NULL)
            continue;

        Vector2 texCoord0 = { 
            .x=level.portal[i].portaledPlane.plane->x0,
            .y=level.portal[i].portaledPlane.plane->y0
            };
        Vector2 texCoord1 = { 
            .x=level.portal[i].portaledPlane.plane->x1,
            .y=level.portal[i].portaledPlane.plane->y1
            };
        RenderQuad(level.portal[i].portaledPlane.plane->vertex1, level.portal[i].portaledPlane.plane->vertex2, level.portal[i].portaledPlane.plane->vertex3, level.portal[i].portaledPlane.plane->vertex4, level.portal[i].portaledPlane.plane->material, texCoord0, texCoord1);
        
        // // make a pointer so it dosnt have to be "level.portal[i].portaledPlane.plane" evry time
        // PLANE* plane = level.portal[i].portaledPlane.plane;

        // /*
        // Wall Fragments
        //         0
        // ----------------
        //     |4/    \5|   
        //     |/      \| 
        //  1  | portal |  2
        //     |\      /|
        //     |6\    /7|  
        // ----------------
        //         3
        // */
        // PLANE wallFragments[8];
        // // WallFragment 0
        // wallFragments[0].vertex1 = plane->vertex1;
        // wallFragments[0].vertex1.z = plane->vertex1.z - (plane->vertex1.z - level.portal[i].position.z);
        // wallFragments[0].vertex2 = plane->vertex2;
        // wallFragments[0].vertex2.z = plane->vertex2.z - (plane->vertex2.z - level.portal[i].position.z);
        // wallFragments[0].vertex3 = plane->vertex3;
        // wallFragments[0].vertex4 = plane->vertex4;

        // wallFragments[0].x0 = plane->x0;
        // wallFragments[0].x1 = plane->x1 + (plane->vertex2.z - level.portal[i].position.z) / 2;
        // wallFragments[0].y0 = plane->y0;
        // wallFragments[0].y1 = plane->y1;
        
        // // WallFragment 1
        // wallFragments[1].vertex1 = plane->vertex1;
        // wallFragments[1].vertex2 = plane->vertex2;
        // wallFragments[1].vertex2.x = plane->vertex2.x - (plane->vertex2.x - level.portal[i].position.x) - 21 * (level.portal->rotation.y / 360);
        // wallFragments[1].vertex2.y = plane->vertex2.y - (plane->vertex2.y - level.portal[i].position.y) - 21 * ((level.portal->rotation.y + 90) / 360);
        // wallFragments[1].vertex3 = plane->vertex3;
        // wallFragments[1].vertex3.z = plane->vertex3.z - (plane->vertex3.z - level.portal[i].position.z);
        // wallFragments[1].vertex3.x = plane->vertex3.x - (plane->vertex3.x - level.portal[i].position.x) - 21 * (level.portal->rotation.y / 360);
        // wallFragments[1].vertex3.y = plane->vertex3.y - (plane->vertex3.y - level.portal[i].position.y) - 21 * ((level.portal->rotation.y + 90) / 360);
        // wallFragments[1].vertex4 = plane->vertex4;
        // wallFragments[1].vertex4.z = plane->vertex4.z - (plane->vertex4.z - level.portal[i].position.z);

        // wallFragments[1].x0 = plane->x0 + (plane->vertex4.z - level.portal[i].position.z) / 2;
        // wallFragments[1].x1 = plane->x1;
        // wallFragments[1].y0 = plane->y0;
        // wallFragments[1].y1 = plane->y1 + (((plane->vertex2.x - level.portal[i].position.x) + (plane->vertex2.y - level.portal[i].position.y)) / 2) + 10.5 * (level.portal->rotation.y / 90);
        // printf("%f\n",level.portal->rotation.y);
        
        // // use the same materials
        // for(int j=0; j<8; j++){
        //     wallFragments[j].material = plane->material;
        //     // wallFragments[j].vertex1 = plane->vertex1;
        //     // wallFragments[j].vertex2 = plane->vertex2;
        //     // wallFragments[j].vertex3 = plane->vertex3;
        //     // wallFragments[j].vertex4 = plane->vertex4;
        // }
        
        // for(int j=0; j<8; j++){
            
        //     // TODO change the plane type to use Vector2 texture coordinates
        //     Vector2 texCoord0 = { 
        //         .x=wallFragments[j].x0,
        //         .y=wallFragments[j].y0
        //         };
        //     Vector2 texCoord1 = { 
        //         .x=wallFragments[j].x1,
        //         .y=wallFragments[j].y1
        //         };
        //     RenderQuad(wallFragments[j].vertex1, wallFragments[j].vertex2, wallFragments[j].vertex3, wallFragments[j].vertex4, wallFragments[j].material, texCoord0, texCoord1);
        //     
        // }
    }
}

// Render a quad
void RenderQuad(Vector3 vertex1, Vector3 vertex2, Vector3 vertex3, Vector3 vertex4, NE_Material* material, Vector2 texCoord0, Vector2 texCoord1) {

        // Render a plane
        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, 0);

        NE_MaterialUse(material);

        NE_PolyBegin(GL_QUAD);
        //NE_PolyNormal(level.Plane[i].nx / 100, level.Plane[i].nz / 100, level.Plane[i].ny / 100);

        NE_PolyTexCoord(texCoord0.y, texCoord1.x);
        NE_PolyVertexI(floatToFixed(vertex1.x, LEVEL_RENDER_SIZE), floatToFixed(vertex1.z, LEVEL_RENDER_SIZE), floatToFixed(vertex1.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(texCoord1.y, texCoord1.x);
        NE_PolyVertexI(floatToFixed(vertex2.x, LEVEL_RENDER_SIZE), floatToFixed(vertex2.z, LEVEL_RENDER_SIZE), floatToFixed(vertex2.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(texCoord1.y, texCoord0.x);
        NE_PolyVertexI(floatToFixed(vertex3.x, LEVEL_RENDER_SIZE), floatToFixed(vertex3.z, LEVEL_RENDER_SIZE), floatToFixed(vertex3.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(texCoord0.y, texCoord0.x);
        NE_PolyVertexI(floatToFixed(vertex4.x, LEVEL_RENDER_SIZE), floatToFixed(vertex4.z, LEVEL_RENDER_SIZE), floatToFixed(vertex4.y, LEVEL_RENDER_SIZE));

        NE_PolyEnd();
}

//Render all Planes in the level
void RenderPlanes(Level level) {
    int i;
    for (i = 0; i < MAX_PLANES; i++)
    {
        // could be optimized by just removing it from the level.Plane array
        if(!level.Plane[i].isDrawn)
            continue;

        // Render a plane
        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_NONE, 0);

        NE_MaterialUse(level.Plane[i].material);

        NE_PolyBegin(GL_QUAD);
        //NE_PolyNormal(level.Plane[i].nx / 100, level.Plane[i].nz / 100, level.Plane[i].ny / 100);

        NE_PolyTexCoord(level.Plane[i].y0, level.Plane[i].x1);
        NE_PolyVertexI(floatToFixed(level.Plane[i].vertex1.x, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex1.z, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex1.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(level.Plane[i].y1, level.Plane[i].x1);
        NE_PolyVertexI(floatToFixed(level.Plane[i].vertex2.x, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex2.z, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex2.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(level.Plane[i].y1, level.Plane[i].x0);
        NE_PolyVertexI(floatToFixed(level.Plane[i].vertex3.x, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex3.z, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex3.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(level.Plane[i].y0, level.Plane[i].x0);
        NE_PolyVertexI(floatToFixed(level.Plane[i].vertex4.x, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex4.z, LEVEL_RENDER_SIZE), floatToFixed(level.Plane[i].vertex4.y, LEVEL_RENDER_SIZE));

        NE_PolyEnd();
    }
}

void RenderDebug(void)
{
    //print position and rotation
    if (debugText) {
        printf("\x1b[3;1HPos: x:%.2f y:%.2f z:%.2f\x1b[4;1HRot: x:%.2f y:%.2f z:%.2f\n0:%f 1:%f 2:%f", 
        localPlayer.position.x, localPlayer.position.y, localPlayer.position.z, localPlayer.rotation.x, localPlayer.rotation.y, localPlayer.rotation.z, localPlayer.lookVector.x, localPlayer.lookVector.y, localPlayer.lookVector.z);
    }
    if (debugVision) {
        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_NONE, 0);
        for (int i = 0; i < level.currentHitbox; i++) {
            NE_PolyBegin(GL_QUAD);
            NE_MaterialUse(debugempty);
            // top and bottom
            for (int j = 0; j < 8; j++) {
                NE_PolyVertexI(floatToFixed(level.allHitboxes[i].vertex[j].x, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j].z, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j].y, LEVEL_RENDER_SIZE));
            }
            // sides
            for (int j = 0; j < 8; j += 6) {
                NE_PolyVertexI(floatToFixed(level.allHitboxes[i].vertex[j].x, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j].z, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j].y, LEVEL_RENDER_SIZE));
                NE_PolyVertexI(floatToFixed(level.allHitboxes[i].vertex[j + 1].x, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j + 1].z, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j + 1].y, LEVEL_RENDER_SIZE));
            }
            for (int j = 2; j < 6; j += 2) {
                NE_PolyVertexI(floatToFixed(level.allHitboxes[i].vertex[j].x, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j].z, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j].y, LEVEL_RENDER_SIZE));
                NE_PolyVertexI(floatToFixed(level.allHitboxes[i].vertex[j + 1].x, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j + 1].z, LEVEL_RENDER_SIZE), floatToFixed(level.allHitboxes[i].vertex[j + 1].y, LEVEL_RENDER_SIZE));
            }
        }
    }
}

void Draw3DScene(void)
{
    NE_CameraUse(Camara);

    CameraMoveGlobal(Camara, localPlayer);
    renderPortals();
    RenderPlanes(level);
    RenderDebug();
}