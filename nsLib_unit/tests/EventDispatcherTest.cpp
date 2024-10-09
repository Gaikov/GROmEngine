//
// Created by Roman on 5/3/2024.
//

#include "UnitCommon.h"
#include "nsLib/events/EventDispatcher.h"

TEST(EventDispatcher, emmit) {
    nsEventDispatcher dispatcher;

    int count = 0;

    nsEventDispatcher::tEventHandler handler = [&count](const nsBaseEvent &e) {
        count++;
    };

    dispatcher.AddHandler(0, &handler);
    dispatcher.Emmit(nsBaseEvent(0));
    dispatcher.RemoveHandler(0, &handler);
    dispatcher.Emmit(nsBaseEvent(0));

    ASSERT_EQ(count, 1);

    nsEventDispatcher::tEventHandler removeHandler = [&](const nsBaseEvent &e) {
        dispatcher.RemoveHandler(0, &handler);
    };

    dispatcher.AddHandler(0, &removeHandler);
    dispatcher.AddHandler(0, &handler);

    dispatcher.Emmit(nsBaseEvent(0));

    ASSERT_EQ(count, 2);

    dispatcher.Emmit(nsBaseEvent(0));

    ASSERT_EQ(count, 2);
}
