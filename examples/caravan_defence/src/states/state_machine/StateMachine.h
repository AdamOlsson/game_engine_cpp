#pragma once

#include "StateTransition.h"
#include <tuple>
#include <unordered_map>

template <typename Context, typename Tuple> struct states_valid;

template <typename T>
concept is_tuple = requires { []<typename... Ts>(std::tuple<Ts...> *) {}((T *)nullptr); };

template <typename States, typename Context>
concept has_required_functions = requires(States t, Context ctx, float dt) {
    { t.on_enter(ctx) } -> std::same_as<void>;
    { t.on_exit(ctx) } -> std::same_as<void>;
    { t.update(dt, ctx) } -> std::same_as<util::StateTransition>;
};

template <typename Context, typename... Ts>
struct states_valid<Context, std::tuple<Ts...>> {
    static constexpr bool value = (has_required_functions<Ts, Context> && ...);
};

namespace util {

template <is_tuple States, typename StateContext> class StateMachine {
  private:
    static_assert(states_valid<StateContext, States>::value, "Not all states are valid.");

    States m_states; // Tuple with all possible states
    void (*m_on_enter)(States &, StateContext &) = nullptr;
    void (*m_on_exit)(States &, StateContext &) = nullptr;
    StateTransition (*m_update)(States &, StateContext &, float) = nullptr;

    template <typename S> void bind() {
        m_on_enter = [](States &s, StateContext &c) { std::get<S>(s).on_enter(c); };
        m_on_exit = [](States &s, StateContext &c) { std::get<S>(s).on_exit(c); };
        m_update = [](States &s, StateContext &c, float dt) {
            return std::get<S>(s).update(dt, c);
        };
    }

    using binder_fn = void (*)(StateMachine &);

    static constexpr auto make_bind_table() {
        return []<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::array<binder_fn, sizeof...(Is)>{
                [](StateMachine &m) { m.bind<std::tuple_element_t<Is, States>>(); }...};
        }(std::make_index_sequence<std::tuple_size_v<States>>{});
    }

    void transition_to(std::type_index type) {
        static constexpr auto table = make_bind_table();

        // Parallel array of type_index values — built once, scanned at transition time
        static const auto types = []<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::array<std::type_index, sizeof...(Is)>{
                std::type_index(typeid(std::tuple_element_t<Is, States>))...};
        }(std::make_index_sequence<std::tuple_size_v<States>>{});

        for (std::size_t i = 0; i < types.size(); ++i) {
            if (types[i] == type) {
                table[i](*this);
                return;
            }
        }
    }

  public:
    StateMachine() = default;

    StateMachine(StateMachine &&) noexcept = default;
    StateMachine(const StateMachine &) = delete;
    StateMachine &operator=(StateMachine &&) noexcept = default;
    StateMachine &operator=(const StateMachine &) = delete;

    template <typename InitialState> void init() { bind<InitialState>(); }

    template <typename S> S &get_state() { return std::get<S>(m_states); }

    template <typename NextState> void transition(StateContext &context) {
        m_on_exit(m_states, context);
        bind<NextState>();
        m_on_enter(m_states, context);
    }

    void update(float dt, StateContext &context) {
        auto result = m_update(m_states, context, dt);

        if (result.next.has_value()) {
            m_on_exit(m_states, context);
            transition_to(result.next.value());
            m_on_enter(m_states, context);
        }
    }
};

} // namespace util
