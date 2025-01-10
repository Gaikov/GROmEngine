//
// Created by Roman on 5/3/2024.
//

#include "EventDispatcher.h"

int nsEventDispatcher::AddHandler(int eventId, const tEventHandler &handler) {
    GetHandlers(eventId)[_idCounter] = handler;
    return _idCounter ++;
}

void nsEventDispatcher::RemoveHandler(int eventId, int handlerId) {
    GetHandlers(eventId).erase(handlerId);
}

nsEventDispatcher::tHandlersList &nsEventDispatcher::GetHandlers(int eventId) {
    return _handlers[eventId];
}

void nsEventDispatcher::Emmit(const nsBaseEvent &event) {
    auto &list = GetHandlers(event.GetId());
    for (auto &it : list) {
        it.second(&event);
    }
}
