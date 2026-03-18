#pragma once
#include <tuple>

#include "DefendState.h"
#include "EventState.h"
#include "IntroState.h"

using CaravanDefenceStates = std::tuple<IntroState, DefendState, EventState>;
