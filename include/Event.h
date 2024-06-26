// Event.h
//

#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>

template<typename... Args>
class Event {
public:
    using EventHandler = std::function<void(Args...)>;

    void subscribe(void* subscriber, EventHandler handler) {
        observers.push_back({ subscriber, handler });
    }

    void unsubscribe(void* subscriber) {
        observers.erase(std::remove_if(observers.begin(), observers.end(),
            [&](const Observer& obs) { return obs.subscriber == subscriber; }),
            observers.end());
    }

    void notify(Args... args) {
        for (auto& observer : observers) {
            observer.handler(std::forward<Args>(args)...);
        }
    }

private:
    struct Observer {
        void* subscriber;
        EventHandler handler;
    };

    std::vector<Observer> observers;
};
