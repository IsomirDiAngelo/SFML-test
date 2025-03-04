#ifndef COLLISION_TABLE_H
#define COLLISION_TABLE_H

// {tileType, isSolid}
static std::unordered_map<int, bool> collisionTable = {
    {2, true}, {3, true}, {4, true}, 
    {26, true}, {27, true}, {28, true}, {29, true}, {30, true}, 
    {51, true}, {52, true} ,{53, true}, {54, true}, {55, true},  
    {76, true}, {77, true}, {78, true}, {79, true}, {80, true},
    {102, true}, {103, true}, {104, true},
    {131, true}, {132, true}, {133, true},
    {236, true},
    {431, true}, {432, true}, {433, true}, {436, true}, {437, true}, {438, true},
};

static std::unordered_map<int, bool> dangerTable = {
    {211, true}, {59, true}, {60, true}
};

#endif