#ifndef COLLISION_TABLE_H
#define COLLISION_TABLE_H

// {tileType, isSolid}
static std::unordered_map<int, bool> collisionTable = {
    {3, true}, {28, true}, {30, true}, {55, true}, {26, true}, {102, true}, {104, true},
    {211, false}
};

static std::unordered_map<int, bool> dangerTable = {
    {211, true}, {59, true}, {60, true}
};

#endif