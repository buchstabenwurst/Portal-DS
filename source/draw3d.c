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
    tmpTo[0] = fixedToFloat(sinLerp(floatToFixed(player.rotation.y / 45, 12)) / 3 * fixedToFloat(cosLerp(floatToFixed(player.rotation.z / 45, 12)), 12), 12);
    tmpTo[1] = fixedToFloat(sinLerp(floatToFixed(player.rotation.z / 45, 12)), 21) * 180;
    tmpTo[2] = fixedToFloat(cosLerp(floatToFixed(player.rotation.y / 45, 12)) / 3 * fixedToFloat(cosLerp(floatToFixed(player.rotation.z / 45, 12)), 12), 12);

    NE_AssertPointer(cam, "NULL pointer");

    cam->matrix_is_updated = false;

    cam->from[0] = 0;
    cam->from[1] = 0;
    cam->from[2] = 0;

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
    NE_ModelSetCoord(w_portalgun_model, fixedToFloat(sinLerp(floatToFixed(player.rotation.y / 45 - 50, 12)), 12) / 20,
                                        -0.1 * LEVEL_SIZE,
                                        fixedToFloat(cosLerp(floatToFixed(player.rotation.y / 45 - 50, 12)), 12) / 20);
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
        RenderQuad(level.portal[i].portaledPlane.plane->vertex1, level.portal[i].portaledPlane.plane->vertex2, level.portal[i].portaledPlane.plane->vertex3, level.portal[i].portaledPlane.plane->vertex4, level.portal[i].portaledPlane.plane->material, level.portal[i].portaledPlane.plane->x0, level.portal[i].portaledPlane.plane->x1, level.portal[i].portaledPlane.plane->y0, level.portal[i].portaledPlane.plane->y1);
        
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

void RenderQuadNoformat(Vector3 vertex1, Vector3 vertex2, Vector3 vertex3, Vector3 vertex4, NE_Material* material, int texCoordx0, int texCoordx1, int texCoordy0, int texCoordy1) {

        // do not render plane if it would overflow the vertex position
        if((floatToFixed(abs(vertex1.x - localPlayer.position.x), LEVEL_RENDER_SIZE) > RENDERCLIPDISTANCE) | (floatToFixed(abs(vertex1.y - localPlayer.position.y), LEVEL_RENDER_SIZE) > RENDERCLIPDISTANCE) | (floatToFixed(abs(vertex1.z - localPlayer.position.z), LEVEL_RENDER_SIZE) > RENDERCLIPDISTANCE) |
            (floatToFixed(abs(vertex4.x - localPlayer.position.x), LEVEL_RENDER_SIZE) > RENDERCLIPDISTANCE) | (floatToFixed(abs(vertex4.y - localPlayer.position.y), LEVEL_RENDER_SIZE) > RENDERCLIPDISTANCE) | (floatToFixed(abs(vertex4.z - localPlayer.position.z), LEVEL_RENDER_SIZE) > RENDERCLIPDISTANCE))
            return;
        // Render a plane
        NE_MaterialUse(material);

        NE_PolyBegin(GL_QUAD);
        //NE_PolyNormal(level.Plane[i].nx / 100, level.Plane[i].nz / 100, level.Plane[i].ny / 100);

        NE_PolyTexCoord(texCoordy0, texCoordx1);
        NE_PolyVertexI(floatToFixed(vertex1.x - localPlayer.position.x, LEVEL_RENDER_SIZE), floatToFixed(vertex1.z - localPlayer.position.z, LEVEL_RENDER_SIZE), floatToFixed(vertex1.y - localPlayer.position.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(texCoordy1, texCoordx1);
        NE_PolyVertexI(floatToFixed(vertex2.x - localPlayer.position.x, LEVEL_RENDER_SIZE), floatToFixed(vertex2.z - localPlayer.position.z, LEVEL_RENDER_SIZE), floatToFixed(vertex2.y - localPlayer.position.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(texCoordy1, texCoordx0);
        NE_PolyVertexI(floatToFixed(vertex3.x - localPlayer.position.x, LEVEL_RENDER_SIZE), floatToFixed(vertex3.z - localPlayer.position.z, LEVEL_RENDER_SIZE), floatToFixed(vertex3.y - localPlayer.position.y, LEVEL_RENDER_SIZE));

        NE_PolyTexCoord(texCoordy0, texCoordx0);
        NE_PolyVertexI(floatToFixed(vertex4.x - localPlayer.position.x, LEVEL_RENDER_SIZE), floatToFixed(vertex4.z - localPlayer.position.z, LEVEL_RENDER_SIZE), floatToFixed(vertex4.y - localPlayer.position.y, LEVEL_RENDER_SIZE));

        NE_PolyEnd();
}

// Render a quad
void RenderQuad(Vector3 vertex1, Vector3 vertex2, Vector3 vertex3, Vector3 vertex4, NE_Material* material, int texCoordx0, int texCoordx1, int texCoordy0, int texCoordy1) {
    NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, 0);
    RenderQuadNoformat(vertex1, vertex2, vertex3, vertex4, material, texCoordx0, texCoordx1, texCoordy0, texCoordy1);
}

//Render all Planes in the level
void RenderPlanes(Level level) {
    int i;
    for (i = 0; i < MAX_PLANES; i++)
    {
        // could be optimized by just removing it from the level.Plane array
        if(!level.Plane[i].isDrawn)
            continue;

        RenderQuad(level.Plane[i].vertex1, level.Plane[i].vertex2, level.Plane[i].vertex3, level.Plane[i].vertex4,level.Plane[i].material, level.Plane[i].x0, level.Plane[i].x1, level.Plane[i].y0, level.Plane[i].y1);
    }
}

void renderModels(){
    Entity* elevatorEnt;
    findEntityByName("Arrival_Logic-elevator_1_body", &elevatorEnt);
    Model* elevator = (Model*)elevatorEnt->child;
    // printf("%s\n", elevatorEnt->targetName);
    NE_ModelSetCoordI(elevator_b_model, floatToFixed(elevatorEnt->position.x - localPlayer.position.x, LEVEL_RENDER_SIZE), floatToFixed(elevatorEnt->position.z - localPlayer.position.z, LEVEL_RENDER_SIZE), floatToFixed(elevatorEnt->position.y - localPlayer.position.y, LEVEL_RENDER_SIZE));
    NE_ModelDraw(elevator_b_model);
}

void RenderDebug(void)
{
    //print position and rotation
    if (debugText) {
        printf("\x1b[3;1HPos: x:%.2f y:%.2f z:%.2f\x1b[4;1HRot: x:%.2f y:%.2f z:%.2f\n0:%f 1:%f 2:%f", 
        localPlayer.position.x, localPlayer.position.y, localPlayer.position.z, localPlayer.rotation.x, localPlayer.rotation.y, localPlayer.rotation.z, localPlayer.lookVector.x, localPlayer.lookVector.y, localPlayer.lookVector.z);
    }
    if (debugVision) {
        NE_PolyFormat(0, 1, NE_LIGHT_0, NE_CULL_NONE, 0);
        for (int i = 0; i < level.currentHitbox; i++) {
            NE_PolyBegin(GL_QUAD);
            NE_MaterialUse(debugempty);
            
            RenderQuadNoformat(level.allHitboxes[i].vertex[0], level.allHitboxes[i].vertex[1], level.allHitboxes[i].vertex[2], level.allHitboxes[i].vertex[3], debugempty, 0, 0, 0 ,0);
            RenderQuadNoformat(level.allHitboxes[i].vertex[4], level.allHitboxes[i].vertex[5], level.allHitboxes[i].vertex[6], level.allHitboxes[i].vertex[7], debugempty, 0, 0, 0 ,0);
        
            RenderQuadNoformat(level.allHitboxes[i].vertex[0], level.allHitboxes[i].vertex[1], level.allHitboxes[i].vertex[6], level.allHitboxes[i].vertex[7], debugempty, 0, 0, 0 ,0);
        
            RenderQuadNoformat(level.allHitboxes[i].vertex[2], level.allHitboxes[i].vertex[3], level.allHitboxes[i].vertex[4], level.allHitboxes[i].vertex[5], debugempty, 0, 0, 0 ,0);

        }
    }
}

void Draw3DScene(void)
{
    NE_CameraUse(Camara);

    CameraMoveGlobal(Camara, localPlayer);
    renderModels();
    renderPortals();
    RenderPlanes(level);
    RenderDebug();
}