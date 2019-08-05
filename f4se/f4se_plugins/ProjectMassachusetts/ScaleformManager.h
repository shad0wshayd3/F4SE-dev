#pragma once
#include "DataManager.h"

class ScaleformManager : private DataManager {
public:
    static bool Init(GFxMovieView* View, GFxValue* F4SERoot);
};