#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace Jonaki {

class EventBus {
public:
    template<typename EventType>
    using Handler = std::function<void(const EventType&)>;

    static EventBus& instance() {
        static EventBus s_bus;
        return s_bus;
    }

    template<typename EventType>
    void subscribe(Handler<EventType> handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto typeIdx = std::type_index(typeid(EventType));
        m_handlers[typeIdx].push_back([handler](const void* evt) {
            handler(*static_cast<const EventType*>(evt));
        });
    }

    template<typename EventType>
    void publish(const EventType& event) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto typeIdx = std::type_index(typeid(EventType));
        auto it = m_handlers.find(typeIdx);
        if (it != m_handlers.end()) {
            for (const auto& h : it->second) {
                h(&event);
            }
        }
    }

private:
    EventBus() = default;
    std::mutex m_mutex;
    using RawHandler = std::function<void(const void*)>;
    std::unordered_map<std::type_index, std::vector<RawHandler>> m_handlers;
};

} // namespace Jonaki
