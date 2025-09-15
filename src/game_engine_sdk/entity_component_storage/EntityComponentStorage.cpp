#include "game_engine_sdk/entity_component_storage/EntityComponentStorage.h"

EntityId EntityComponentStorage::create_entity() { return next_id++; }
