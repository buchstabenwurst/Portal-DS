#include <NEMain.h>
#include <math.h>
#include "physics.h"
#include "main.h"
#include "load.h"

float dot(Vector3 a, Vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 cross(Vector3 a, Vector3 b) {
    Vector3 result;
    result.x = a.y * b.z - b.y * a.z;
    result.y = a.z * b.x - b.z * a.x;
    result.z = a.x * b.y - b.x * a.y;
    return result;
}
Vector3 Normalize(Vector3 vector) {
    float len_v = f32tofloat(sqrtf32(floattof32(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z)));
    vector.x /= len_v;
    vector.y /= len_v;
    vector.z /= len_v;
    //printf("\x1b[6;2H%f", len_v);
    return vector;
}

//https://gamedev.stackexchange.com/questions/25397/obb-vs-obb-collision-detection
Vector2 SATtest(Vector3 vector, hitbox box)
{
    Vector2 along;
    along.x = INFINITY, along.y = -INFINITY;
    for (int i = 0; i < 8; i++)
    {
        // just dot it to get the min/max along this axis.
        float dotVal = dot(vector, box.vertex[i]);
        if (dotVal < along.x)  along.x = dotVal;
        if (dotVal > along.y)  along.y = dotVal;
    }
    //printf("\x1b[10;2H%f,%f",along.x,along.y);
    return along;
}


inline bool isBetweenOrdered(float val, float lowerBound, float upperBound) {
    return lowerBound <= val && val <= upperBound;
}

bool overlaps(float min1, float max1, float min2, float max2)
{
    return isBetweenOrdered(min2, min1, max1) || isBetweenOrdered(min1, min2, max2);
}

Vector3 getVectorNormalized(hitbox box) {
    //get direction vector
    box.vector[0].x = box.vertex[0].x - box.vertex[1].x;
    box.vector[0].y = box.vertex[0].y - box.vertex[1].y;
    box.vector[0].z = box.vertex[0].z - box.vertex[1].z; //not tested yet

    //Normalize
    float len_v = f32tofloat(sqrtf32(floattof32(box.vector[0].x * box.vector[0].x + box.vector[0].y * box.vector[0].y + box.vector[0].z * box.vector[0].z)));
    box.vector[0].x /= len_v;
    box.vector[0].y /= len_v;
    box.vector[0].z /= len_v;

    //printf("\x1b[6;2H%f", len_v);
    return box.vector[0];
}

// Shape1 and Shape2 must be CONVEX HULLS and not too small
bool intersects(hitbox* box1, hitbox* box2)
{
    box1->vector[0] = getVectorNormalized(*box1);
    box2->vector[0] = getVectorNormalized(*box2);

    box1->vector[1].x = box1->vector[0].y;
    box1->vector[1].y = -box1->vector[0].x;
    box1->vector[1].z = -box1->vector[0].z; 

    box1->vector[2].x = -box1->vector[0].z;
    box1->vector[2].y = -box1->vector[0].y;
    box1->vector[2].z = box1->vector[0].x;


    box2->vector[1].x = box2->vector[0].y;
    box2->vector[1].y = -box2->vector[0].x;
    box2->vector[1].z = box2->vector[0].z;

    box2->vector[2].x = -box2->vector[0].z;
    box2->vector[2].y = -box2->vector[0].y;
    box2->vector[2].z = box2->vector[0].x;
    //printf("\x1b[7;2H%f,%f,%f", box1->vector[0].x, box1->vector[0].y, box1->vector[0].z);
    //printf("\x1b[8;2H%f,%f,%f", box1->vector[1].x, box1->vector[1].y, box1->vector[1].z);

    
    // Get the normals for one of the shapes,
    for (int i = 0; i < 3 ; i++)
    {
        //float shape1Min, shape1Max, shape2Min, shape2Max;
        Vector2 shape1, shape2;
        shape1 = SATtest(box1->vector[i], *box1);
        shape2 = SATtest(box1->vector[i], *box2);
        if (!overlaps(shape1.x, shape1.y, shape2.x, shape2.y))
        {
            //printf("\x1b[15;2Hlol");
            return 0; // NO INTERSECTION
        }
    }

    // TEST SHAPE2.normals as well
    for (int i = 0; i < 3; i++)
    {
        // otherwise, go on with the next test
        Vector2 shape1, shape2;
        shape1 = SATtest(box2->vector[i], *box1);
        shape2 = SATtest(box2->vector[i], *box2);
        if (!overlaps(shape1.x, shape1.y, shape2.x, shape2.y))
        {
            //printf("\x1b[15;7Hlmao");
            return 0; // NO INTERSECTION
        }
    }

    // if overlap occurred in ALL AXES, then they do intersect
    return 1;
}

// unused
bool linePlaneIntersection(Vector3* contact, Vector3 ray, Vector3 rayOrigin,
    Vector3 normal, Vector3 coord) {
    // get d value
    float d = dot(normal, coord);

    if (dot(normal, ray) == 0) {
        return false; // No intersection, the line is parallel to the plane
    }
    
    // Compute the X value for the directed line ray intersecting the plane
    float x = (d - dot(normal, rayOrigin)) / dot(normal, ray);
    printf("\x1b[7;2H%f", rayOrigin.x + Normalize(ray).x * x);

    // output contact point
    contact->x = rayOrigin.x + Normalize(ray).x * x; //Make sure your ray vector is normalized
    contact->y = rayOrigin.y + Normalize(ray).y * x;
    contact->z = rayOrigin.z + Normalize(ray).z * x;
    return true;
}

// from Wikipedia lol
bool ray_intersects_triangle(Vector3 ray_origin,
    Vector3 ray_vector,
    Vector3 vertex1,
    Vector3 vertex2,
    Vector3 vertex3,
    Vector3* out_intersection_point)
{
    float epsilon = EPSILON;

    Vector3 edge1;
    edge1.x = vertex2.x - vertex1.x;
    edge1.y = vertex2.y - vertex1.y;
    edge1.z = vertex2.z - vertex1.z;

    Vector3 edge2;
    edge2.x = vertex3.x - vertex1.x;
    edge2.y = vertex3.y - vertex1.y;
    edge2.z = vertex3.z - vertex1.z;
    Vector3 ray_cross_e2 = cross(ray_vector, edge2);
    float det = dot(edge1, ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return false;    // This ray is parallel to this triangle.

    float inv_det = 1.0 / det;
    Vector3 s;
    s.x = ray_origin.x - vertex1.x;
    s.y = ray_origin.y - vertex1.y;
    s.z = ray_origin.z - vertex1.z;
    float u = inv_det * dot(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return false;

    Vector3 s_cross_e1 = cross(s, edge1);
    float v = inv_det * dot(ray_vector, s_cross_e1);

    if (v < 0 || u + v > 1)
        return false;

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = inv_det * dot(edge2, s_cross_e1);

    if (t > epsilon) // ray intersection
    {
        out_intersection_point->x = ray_origin.x + ray_vector.x * t;
        out_intersection_point->y = ray_origin.y + ray_vector.y * t;
        out_intersection_point->z = ray_origin.z + ray_vector.z * t;
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

float getDistance(Vector3 pos1, Vector3 pos2) {
    Vector3 distance;
    distance.x = pos1.x - pos2.x;
    distance.y = pos1.y - pos2.y;
    distance.z = pos1.z - pos2.z;
    float distance1 = f32tofloat(sqrtf32(floattof32(distance.x * distance.x + distance.y * distance.y + distance.z * distance.z)));
    return distance1;
}

// check if pos2 or pos3 is closer to pos1
// @returns true if pos3 is closer
// @returns posNearest = the coser position
Vector3 getNearest(Vector3 pos1, Vector3 pos2, Vector3 pos3, bool* pos3Nearest) {
    float distance1 = getDistance(pos1, pos2);
    float distance2 = getDistance(pos1, pos3);
    //printf("\x1b[7;2H%f %f", distance1hyp, distance2hyp);
    if (distance1 >= distance2) {
        *pos3Nearest = true;
        return pos3;
    }
    else {
        *pos3Nearest = false;
        return pos2;
    }
}

// Note: shooting a portal on a trigger crashes the game
bool setPlayerPortalPosition(PLAYER player, hitbox* hitbox, bool portal) {
    Vector3 pos;
    Vector3 tmpPos;
    float reachLengh = 100;
    Vector3 ray;
    ray.x = player.lookVector.x * reachLengh; // multiply by portalable distance
    ray.y = player.lookVector.y * reachLengh;
    ray.z = player.lookVector.z * reachLengh;
    
    for (int i = 1; i < level.currentHitbox; i++) {
        bool hasHit = false;
        bool isNearest = true;
        Vector3 orientation;
        orientation.z = 0;
        orientation.y = 0;
        // find the Direction of the Wall hit
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[0], level.allHitboxes[i].vertex[1], level.allHitboxes[i].vertex[6], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[0], level.allHitboxes[i].vertex[7], level.allHitboxes[i].vertex[6], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        if (hasHit && isNearest) { orientation.y = 0; isNearest = false; goto hasHit;}
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[1], level.allHitboxes[i].vertex[6], level.allHitboxes[i].vertex[5], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[1], level.allHitboxes[i].vertex[2], level.allHitboxes[i].vertex[5], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        if (hasHit && isNearest) { orientation.y = 90; isNearest = false; goto hasHit;}
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[2], level.allHitboxes[i].vertex[5], level.allHitboxes[i].vertex[4], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[2], level.allHitboxes[i].vertex[3], level.allHitboxes[i].vertex[4], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        if (hasHit && isNearest) { orientation.y = 0; isNearest = false; goto hasHit;}
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[3], level.allHitboxes[i].vertex[4], level.allHitboxes[i].vertex[7], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[3], level.allHitboxes[i].vertex[0], level.allHitboxes[i].vertex[7], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        if (hasHit && isNearest) { orientation.y = -90; isNearest = false; goto hasHit;}
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[0], level.allHitboxes[i].vertex[1], level.allHitboxes[i].vertex[2], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[0], level.allHitboxes[i].vertex[3], level.allHitboxes[i].vertex[2], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        if (hasHit && isNearest) { orientation.y = player.rotation.y; orientation.z = 90; isNearest = false; goto hasHit;}
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[7], level.allHitboxes[i].vertex[6], level.allHitboxes[i].vertex[5], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        hasHit += ray_intersects_triangle(player.position, ray, level.allHitboxes[i].vertex[7], level.allHitboxes[i].vertex[4], level.allHitboxes[i].vertex[5], &tmpPos);
        pos = getNearest(player.position, pos, tmpPos, &isNearest);
        if (hasHit && isNearest) { orientation.y = player.rotation.y; orientation.z = -90; isNearest = false; goto hasHit;}
        //printf("\x1b[7;2H%f %f %f", along.x, along.y, pos.z);
        //for (int j = 0; j < 3; j++) {
        //    Vector2 boundsShape = SATtest(player.lookVector, level.allHitboxes[1].vector);
        //    if (!overlaps(boundsVector.x, boundsVector.y, 0, INFINITY)) {
        //        return 0;
        //    }
        //}
        if (hasHit) {
            hasHit:
            Vector3 rotation = level.allHitboxes[i].rotation;
            rotation.y += level.allHitboxes[i].vector[1].y * 90 + orientation.y;
            rotation.z += orientation.z;
            level.portal[portal].rotation = rotation;
            level.portal[portal].position = pos;
            hitbox = &level.allHitboxes[i];
            // unhide previous portaled plane (if it exists)
            if(level.portal[portal].portaledPlane.plane != NULL)
                level.portal[portal].portaledPlane.plane->isDrawn = true;
            // hide current portaled plane
            hitbox->attachedPlane->isDrawn = false;
            level.portal[portal].portaledPlane.plane = hitbox->attachedPlane;
            level.portal[portal].portaledPlane.portalPosition.x = pos.x - hitbox->attachedPlane->vertex1.x;
        }
    }
    return 1;
}

void shootPortal(bool portal) {

    hitbox hitbox; 
    setPlayerPortalPosition(localPlayer, &hitbox, portal);
}

void doCollisions(void) {
    for (int i = 0; i < level.currentDynamicHitbox; i++) {
        //if is dynamic
        for (int j = 0; j < level.currentHitbox; j++) {
            if (level.dynamicHitbxes[i] == j)
                continue;
            //if too far away, skip
            if (getDistance(level.allHitboxes[level.dynamicHitbxes[i]].position, level.allHitboxes[j].position) > 1000)
                continue;
            if (intersects(&level.allHitboxes[level.dynamicHitbxes[i]], &level.allHitboxes[j])) {
                //printf("amogus");
                if(level.allHitboxes[j].isTrigger){
                    if(level.allHitboxes[j].attachedTrigger->input){
                        char entity[64];
                        char command[32];
                        char commandTarget[32] = "Placeholder";
                        sscanf(level.allHitboxes[j].attachedTrigger->input, "%[^\x1B]\x1B%[^\x1B]\x1B%[^\x1B]", entity, command, commandTarget); // "%[^\x1B]" is used like %s but the file format uses wierd seperators
                        if(level.allHitboxes[j].attachedTrigger->mode == 1 && !level.allHitboxes[j].attachedTrigger->alreadyTriggered){ // if trigger_once & not already triggred
                            level.allHitboxes[j].attachedTrigger->alreadyTriggered = true;
                            // TODO: call function
                            // printf("%s\n",command);
                            entFire(entity, command, commandTarget);
                        }
                    }
                }
            }


        }
    }
}

void calculateCollisionVertecies(hitbox* hitbox){
    hitbox->position = *hitbox->attachedPosition; // update position
    hitbox->rotation = *hitbox->attachedRotation; // update rotation

    float tmpSinY = fixedToFloat(sinLerp(floatToFixed((hitbox->rotation.y + 45) / 45, 12)), 12);
    float tmpCosY = fixedToFloat(cosLerp(floatToFixed((hitbox->rotation.y + 45) / 45, 12)), 12);

    for (int j = 0; j < 8; j++)
        hitbox->vertex[j] = hitbox->position;

    hitbox->vertex[0].x += hitbox->sizeX * tmpSinY;
    hitbox->vertex[0].y += hitbox->sizeY * tmpCosY;
    hitbox->vertex[0].z += hitbox->sizeZ;

    hitbox->vertex[1].x += hitbox->sizeX * tmpCosY;
    hitbox->vertex[1].y -= hitbox->sizeY * tmpSinY;
    hitbox->vertex[1].z += hitbox->sizeZ;

    hitbox->vertex[2].x -= hitbox->sizeX * tmpSinY;
    hitbox->vertex[2].y -= hitbox->sizeY * tmpCosY;
    hitbox->vertex[2].z += hitbox->sizeZ;
    
    hitbox->vertex[3].x -= hitbox->sizeX * tmpCosY;
    hitbox->vertex[3].y += hitbox->sizeY * tmpSinY;
    hitbox->vertex[3].z += hitbox->sizeZ;


    hitbox->vertex[4].x -= hitbox->sizeX * tmpCosY;
    hitbox->vertex[4].y += hitbox->sizeY * tmpSinY;
    hitbox->vertex[4].z -= hitbox->sizeZ;

    hitbox->vertex[5].x -= hitbox->sizeX * tmpSinY;
    hitbox->vertex[5].y -= hitbox->sizeY * tmpCosY;
    hitbox->vertex[5].z -= hitbox->sizeZ;

    hitbox->vertex[6].x += hitbox->sizeX * tmpCosY;
    hitbox->vertex[6].y -= hitbox->sizeY * tmpSinY;
    hitbox->vertex[6].z -= hitbox->sizeZ;

    hitbox->vertex[7].x += hitbox->sizeX * tmpSinY;
    hitbox->vertex[7].y += hitbox->sizeY * tmpCosY;
    hitbox->vertex[7].z -= hitbox->sizeZ;
}

void updateCollisions(void) {
    for (int i = 0; i < level.currentDynamicHitbox; i++) {
        calculateCollisionVertecies(&level.allHitboxes[level.dynamicHitbxes[i]]);
    }
}

void addHitbox(Vector3 size, Vector3* position, Vector3* rotation, bool isDynamic) {
    level.allHitboxes[level.currentHitbox].sizeX = size.x / 2;
    level.allHitboxes[level.currentHitbox].sizeY = size.y / 2;
    level.allHitboxes[level.currentHitbox].sizeZ = size.z / 2;
    level.allHitboxes[level.currentHitbox].isDynamic = isDynamic;
    if (!isDynamic) {
        level.allHitboxes[level.currentHitbox].position = *position;
        level.allHitboxes[level.currentHitbox].rotation = *rotation;

        level.allHitboxes[level.currentHitbox].attachedPosition = &level.allHitboxes[level.currentHitbox].position;
        level.allHitboxes[level.currentHitbox].attachedRotation = &level.allHitboxes[level.currentHitbox].rotation;

        calculateCollisionVertecies(&level.allHitboxes[level.currentHitbox]);
    }
    else {
        level.allHitboxes[level.currentHitbox].attachedPosition = position;
        level.allHitboxes[level.currentHitbox].attachedRotation = rotation;
        level.dynamicHitbxes[level.currentDynamicHitbox] = level.currentHitbox;
        level.currentDynamicHitbox++;
    }
    level.currentHitbox++;
}

PLAYER playerPhysics(PLAYER player){
    bool alreadyCollidedX = false;
    bool alreadyCollidedY = false;
    bool alreadyCollidedZ = false;

        //if walking through portal
        for(int j = 0; j < 2; j++){
            if (level.portal[j].onFloor){
                if (player.position.y + PLAYER_WIDTH * 1.6 > level.portal[j].position.y && player.position.y - PLAYER_WIDTH * 1.6 < level.portal[j].position.y && // if player is close enough to the plane
                    player.position.x - PLAYER_WIDTH * 1.6 < level.portal[j].position.x && player.position.x + PLAYER_WIDTH * 1.6 > level.portal[j].position.x && // if player is within the plane
                    player.position.z - 0.5 < level.portal[j].position.z && player.position.z + 0.5 > level.portal[j].position.z)
                {
                    alreadyCollidedZ = true;
                    player.physics.isGrounded = false;
                }
                
            }
            else{
                if (player.position.y + PLAYER_WIDTH * 1.6 > level.portal[j].position.y && player.position.y - PLAYER_WIDTH * 1.6 < level.portal[j].position.y && // if player is close enough to the plane
                    player.position.x - PLAYER_WIDTH * 1.6 < level.portal[j].position.x && player.position.x + PLAYER_WIDTH * 1.6 > level.portal[j].position.x && // if player is within the plane
                    player.position.z - PLAYER_HIGHT_TOP < level.portal[j].position.z && player.position.z + PLAYER_HIGHT > level.portal[j].position.z)
                {
                    alreadyCollidedY = true;
                    alreadyCollidedX = true;
                }
            }
        }

    for (int i = 0; i < level.planeCount; i++){
        if (!alreadyCollidedZ){ //if not already collided this frame
            if (level.Plane[i].vertex1.z == level.Plane[i].vertex3.z){
                //floor
                if (player.position.y + PLAYER_WIDTH > (level.Plane[i].vertex1.y) && player.position.y - PLAYER_WIDTH < (level.Plane[i].vertex2.y) && // if player is within the plane
                player.position.x - PLAYER_WIDTH < (level.Plane[i].vertex3.x) && player.position.x + PLAYER_WIDTH > (level.Plane[i].vertex2.x) &&
                player.position.z > (level.Plane[i].vertex1.z - PLAYER_HIGHT_TOP) && player.position.z < (level.Plane[i].vertex1.z + PLAYER_HIGHT)) // if player is enough above the plane
                {
                    player.physics.isGrounded = true;
                    player.position.z = level.Plane[i].vertex1.z + PLAYER_HIGHT - 0.001;
                    alreadyCollidedZ = true;
                }
                else {
                    player.physics.isGrounded = false;
                }
                //cieling
                if (player.position.y + PLAYER_WIDTH > (level.Plane[i].vertex1.y) && player.position.y - PLAYER_WIDTH < (level.Plane[i].vertex2.y) && // if player is within the plane
                player.position.x + PLAYER_WIDTH > (level.Plane[i].vertex3.x) && player.position.x - PLAYER_WIDTH < (level.Plane[i].vertex2.x) &&
                player.position.z > (level.Plane[i].vertex1.z - PLAYER_HIGHT_TOP) && player.position.z < (level.Plane[i].vertex1.z + PLAYER_HIGHT)) // if player is enough above the plane
                {
                    player.physics.velocity.z = 0;
                    player.position.z = level.Plane[i].vertex1.z - PLAYER_HIGHT_TOP - 0.001;
                }
            }
        }
        if (!alreadyCollidedY){ //if not already collided this frame
            //if y aligned wall
            if (level.Plane[i].vertex1.y == level.Plane[i].vertex2.y && level.Plane[i].vertex1.y == level.Plane[i].vertex3.y){ //if it even is a y aligned wall
                if (player.position.y + PLAYER_WIDTH > (level.Plane[i].vertex1.y) && player.position.y - PLAYER_WIDTH < (level.Plane[i].vertex1.y)){ // if player is close enough to the plane
                    if (player.position.x - PLAYER_WIDTH * 0.9 < (level.Plane[i].vertex1.x) && player.position.x + PLAYER_WIDTH * 0.9 > (level.Plane[i].vertex2.x) && // if player is within the plane
                    player.position.z < (level.Plane[i].vertex3.z + PLAYER_HIGHT - 0.01) && player.position.z > (level.Plane[i].vertex2.z - PLAYER_HIGHT_TOP))
                    {
                        player.physics.velocity.y = 0;
                        player.position.y = level.Plane[i].vertex1.y - PLAYER_WIDTH - 0.000001;
                        alreadyCollidedY = true;
                    }
                    //if y aligned flipped wall
                    if (player.position.x + PLAYER_WIDTH * 0.9 > (level.Plane[i].vertex1.x) && player.position.x - PLAYER_WIDTH * 0.9 < (level.Plane[i].vertex2.x) && // if player is within the plane
                    player.position.z < (level.Plane[i].vertex3.z + PLAYER_HIGHT - 0.01) && player.position.z > (level.Plane[i].vertex2.z - PLAYER_HIGHT_TOP))
                    {
                        player.physics.velocity.y = 0;
                        player.position.y = level.Plane[i].vertex1.y + PLAYER_WIDTH + 0.000001;
                        alreadyCollidedY = true;
                    }
                }
            }
        }
        if (!alreadyCollidedX){ //if not already collided this frame
            //if x aligned wall
            if (level.Plane[i].vertex1.x == level.Plane[i].vertex2.x && level.Plane[i].vertex1.x == level.Plane[i].vertex3.x){
                if (player.position.x + PLAYER_WIDTH > (level.Plane[i].vertex1.x) && player.position.x - PLAYER_WIDTH < (level.Plane[i].vertex1.x)){ // if player is close enough to the plane
                    if (player.position.y - PLAYER_WIDTH * 0.9 < (level.Plane[i].vertex1.y) && player.position.y + PLAYER_WIDTH * 0.9 > (level.Plane[i].vertex2.y) && // if player is within the plane
                    player.position.z < (level.Plane[i].vertex3.z + PLAYER_HIGHT - 0.01) && player.position.z > (level.Plane[i].vertex2.z - PLAYER_HIGHT_TOP))
                    {
                        player.physics.velocity.x = 0;
                        player.position.x = level.Plane[i].vertex1.x + PLAYER_WIDTH + 0.000001;
                        alreadyCollidedX = true;
                    }
                    //if x aligned flipped wall
                    if (player.position.y + PLAYER_WIDTH * 0.9 > (level.Plane[i].vertex1.y) && player.position.y - PLAYER_WIDTH * 0.9 < (level.Plane[i].vertex2.y) && // if player is within the plane
                    player.position.z < (level.Plane[i].vertex3.z + PLAYER_HIGHT - 0.01) && player.position.z > (level.Plane[i].vertex2.z - PLAYER_HIGHT_TOP))
                    {
                        player.physics.velocity.x = 0;
                        player.position.x = level.Plane[i].vertex1.x - PLAYER_WIDTH - 0.000001;
                        alreadyCollidedX = true;
                    }
                }
            }
        }
    }

    if (player.physics.isGrounded){
        if (player.isJumping){
            player.isJumping = false;
        } 
        else {
            player.physics.velocity.z = 0;
        }
    }
    else {
        //add gravity
        player.physics.velocity.z -= GRAVITY;
    }

    //add velocity influence
    player.position.x += player.physics.velocity.x;
    player.position.y += player.physics.velocity.y;
    player.position.z += player.physics.velocity.z;
    //reduce velocity
    player.physics.velocity.x *= FRICTION;
    player.physics.velocity.y *= FRICTION;
    player.physics.velocity.z *= AIR_FRICTION;

        return player;
}

void doPhysics (void){
    localPlayer = playerPhysics(localPlayer);
    updateCollisions();
    doCollisions();
}