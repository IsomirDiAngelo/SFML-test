#ifndef COLLISION_TABLE_H
#define COLLISION_TABLE_H

#include <set>

// {tileType, isSolid}
static std::unordered_map<int, bool> collisionTable = {
    {2, true}, {3, true}, {4, true}, 
    {26, true}, {27, true}, {28, true}, {29, true}, {30, true}, 
    {51, true}, {52, true} ,{53, true}, {54, true}, {55, true},  
    {76, true}, {77, true}, {78, true}, {79, true}, {80, true},
    {102, true}, {103, true}, {104, true},
    {131, true}, {132, true}, {133, true},
    {236, true},
    {251, true}, {252, true}, {253, true}, {254, true}, {255, true},
    {377, true}, {378, true}, {379, true}, 
    {401, true}, {402, true}, {403, true}, {404, true}, {405, true}, 
    {426, true}, {427, true}, {428, true}, {429, true}, {430, true},
    {431, true}, {432, true}, {433, true}, {436, true}, {437, true}, {438, true},
    {451, true}, {452, true}, {453, true}, {454, true}, {455, true},
    {477, true}, {478, true}, {479, true},
};

static std::unordered_map<int, bool> dangerTable = {
    {211, true}, {59, true}, {60, true}
};

static std::set<int> leavesTiles = {377, 378, 379, 401, 402, 404, 405, 426, 430, 451, 452, 454, 455, 477, 478, 479};

static std::set<int> branchesTiles = {431, 432, 433, 436, 437, 438};

#endif