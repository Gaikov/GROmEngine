//
// Created by Roman on 5/3/2024.
//

#pragma once
#include "BaseEvent.h"
#include "nsLib/headers.h"

class nsEventDispatcher {
public:
    typedef std::function<void(const nsBaseEvent *event)> tEventHandler;
    typedef std::map<int, tEventHandler> tHandlersList;

public:
    virtual ~nsEventDispatcher() = default;
    int AddHandler(int eventId, const tEventHandler &handler);
    void RemoveHandler(int eventId, int handlerId);
    void Emmit(const nsBaseEvent &event);

private:
    tHandlersList& GetHandlers(int eventId);

    std::map<int, tHandlersList> _handlers;
    int _idCounter = 0;
};
