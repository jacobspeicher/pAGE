#pragma once

#include <list>
#include <map>
#include <typeindex>
#include <memory>

#include <spdlog/spdlog.h>

#include "Event.h"

class IEventCallback {
private:
	virtual void Call(Event& e) = 0;

public:
	virtual ~IEventCallback() = default;

	void Execute(Event& e) {
		Call(e);
	}
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
private:
	typedef void (TOwner::* CallbackFunction)(TEvent&);

	TOwner* ownerInstance;
	CallbackFunction callbackFunction;

	virtual void Call(Event& e) override {
		std::invoke(callbackFunction, ownerInstance, static_cast<TEvent&>(e));
	}

public:
	EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction) {
		this->ownerInstance = ownerInstance;
		this->callbackFunction = callbackFunction;
	}
	
	virtual ~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
private:
	const std::string outputPrefix = "EVENT BUS :";
	std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
	EventBus() {
		spdlog::info("{0} Created", outputPrefix);
	}
	~EventBus() {
		spdlog::info("{0} Destroyed", outputPrefix);
	}

	// clears the subscriber list
	void Reset() {
		subscribers.clear();
	}

	// subscribe to an event type <T>
	// in the implementation, a listener subscribes to an event
	template <typename TEvent, typename TOwner>
	void SubscribeToEvent(TOwner* ownerInstance, void (TOwner::* callbackFunction)(TEvent&)) {
		if (!subscribers[typeid(TEvent)].get()) {
			subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
		}

		auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
		subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
	}

	// emit an event of type <T>
	// in the implementation, as soon as something emits an event all listener callbacks are executed
	template <typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args) {
		auto handlers = subscribers[typeid(TEvent)].get();
		if (handlers) {
			for (auto itr = handlers->begin(); itr != handlers->end(); ++itr) {
				auto handler = itr->get();
				TEvent event(std::forward<TArgs>(args)...);
				handler->Execute(event);
			}
		}
	}
};