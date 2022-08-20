#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <type_traits>
#include <unordered_set>

class GeneralListener;
class ListenerOwner;

class GeneralEvent {
public:
    virtual ~GeneralEvent(){}
    virtual void remove(ListenerOwner* owner) = 0;
};

class ListenerOwner {
public:
    virtual ~ListenerOwner() {
        for (GeneralEvent* event : events) {
            event->remove(this);
        }
    }
    void add(GeneralEvent* event) {
        events.insert(event);
    }
    void remove(GeneralEvent* event) {
        events.erase(event);
    }
private:
    std::unordered_set<GeneralEvent*> events;
};

class GeneralListener {
public:
    virtual ~GeneralListener(){}
};

template<typename... Args>
class Listener: public GeneralListener {
public:
    virtual ~Listener(){}
    virtual void operator()(Args... args) = 0;
};

template<typename S, typename... Args>
class MemberListener: public Listener<Args...> {
public:
    using F = void (S::*)(Args...);
    virtual ~MemberListener(){}
    MemberListener(S* s, F f): s(s), f(f){}
    virtual void operator()(Args... args) {
        (s->*f)(std::forward<Args>(args)...);
    }
private:
    S* s;
    F f;
};

template<typename... Args>
class FunctionListener: public Listener<Args...> {
public:
    virtual ~FunctionListener(){}
    FunctionListener(std::function<void(Args...)> f): f(f){}
    virtual void operator()(Args... args) {
        f(std::forward<Args>(args)...);
    }
private:
    std::function<void(Args...)> f;
};

template<typename... Args>
class Event : public GeneralEvent {
public:
    using ListenerPair = std::pair<ListenerOwner*, std::shared_ptr<Listener<Args...>>>;
    
    virtual ~Event() {
        for (ListenerPair& pair : listeners) {
            pair.first->remove(this);
        }
    }
    void operator()(Args... args) {
        auto copy = listeners;
        for (ListenerPair& pair : copy) {
            if (std::find_if(listeners.begin(), listeners.end(), [&pair](ListenerPair& p) {
                    return p.first == pair.first;
                }) != listeners.end()) {
                pair.second->operator()(std::forward<Args>(args)...);
            }
        }
    }
    void remove(ListenerOwner* owner) override {
        listeners.erase(std::remove_if(listeners.begin(), listeners.end(), [owner](ListenerPair& pair) {
            return pair.first == owner;
        }), listeners.end());
    }
    void add(ListenerOwner* owner, const std::function<void(Args...)>& l) {
        listeners.push_back({owner, std::make_shared<FunctionListener<Args...>>(l)});
		owner->add(this);
    }
    template<typename S>
    void add(S* owner, void (S::* f)(Args...)) {
        static_assert (std::is_base_of_v<ListenerOwner, S>,
                "The owner of a listener must be a ListenerOwner");
        listeners.push_back({owner, std::make_shared<MemberListener<S, Args...>>(owner, f)});
		owner->add(this);
    }
private:
    std::vector<ListenerPair> listeners;
};

template<typename... Args>
class EventListeners {
public:
    EventListeners(Event<Args...>& event): event(event){}
    template<typename... T>
    void add(T... args) {
        event.add(std::forward<T>(args)...);
    }
private:
    Event<Args...>& event;
};

#define EVENT(name, ...) Event<__VA_ARGS__> name;\
    public: inline EventListeners<__VA_ARGS__> name ## Listeners(){\
        return EventListeners<__VA_ARGS__>(name);}\
    private:

