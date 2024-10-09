//
// Created by Roman on 5/3/2024.
//

#ifndef _GROM_EVENTDISPATCHER_H
#define _GROM_EVENTDISPATCHER_H

#include "BaseEvent.h"
#include "nsLib/headers.h"

class nsEventDispatcher {
public:
    typedef std::function<void(const nsBaseEvent &event)> tEventHandler;
    typedef std::list<const tEventHandler*> tHandlersList;

public:
    virtual ~nsEventDispatcher() = default;
    void AddHandler(int eventId, const tEventHandler* handler);
    void RemoveHandler(int eventId, const tEventHandler* handler);
    void Emmit(const nsBaseEvent &event);

private:
    tHandlersList& GetHandlers(int eventId);

    std::map<int, tHandlersList> _handlers;
};

#endif //_GROM_EVENTDISPATCHER_H
