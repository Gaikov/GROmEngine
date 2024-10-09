//
// Created by Roman on 5/3/2024.
//

#include "EventDispatcher.h"

void nsEventDispatcher::AddHandler(int eventId, const tEventHandler* handler) {
    GetHandlers(eventId).push_back(handler);
}

void nsEventDispatcher::RemoveHandler(int eventId, const tEventHandler* handler) {
    GetHandlers(eventId).remove(handler);
}

nsEventDispatcher::tHandlersList &nsEventDispatcher::GetHandlers(int eventId) {
    return _handlers[eventId];
}

void nsEventDispatcher::Emmit(const nsBaseEvent &event) {
    auto list = GetHandlers(event.GetId());
    for (auto it : list) {
        (*it)(event);
    }
}
