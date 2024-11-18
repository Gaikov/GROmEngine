//
// Created by Roman on 5/3/2024.
//

#include "UnitCommon.h"
#include "nsLib/events/EventDispatcher.h"

class TestEvent : public nsBaseEvent {
public:
    explicit TestEvent(int id) : nsBaseEvent(id) {}
};

TEST(EventDispatcher, emmit) {
    nsEventDispatcher dispatcher;

    int count = 0;

    nsEventDispatcher::tEventHandler handler = [&count](const nsBaseEvent *e) {
        count++;
    };

    int id = dispatcher.AddHandler(0, handler);
    dispatcher.Emmit(TestEvent(0));
    dispatcher.RemoveHandler(0, id);
    dispatcher.Emmit(nsBaseEvent(0));

    ASSERT_EQ(count, 1);

    dispatcher.AddHandler(0, [&](const nsBaseEvent *e) {
        dispatcher.RemoveHandler(0, id);
    });
    id = dispatcher.AddHandler(0, handler);

    dispatcher.Emmit(nsBaseEvent(0));

    ASSERT_EQ(count, 2);

    dispatcher.Emmit(nsBaseEvent(0));

    ASSERT_EQ(count, 2);
}
