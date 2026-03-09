#include "CaravanDefence.h"
#include "game_engine_sdk/GameEngine.h"

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(1080, 960),
                                              .title = "Caravan Defence"},
    };

    auto game = std::make_unique<CaravanDefence>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
