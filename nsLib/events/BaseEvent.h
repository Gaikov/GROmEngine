//
// Created by Roman on 5/3/2024.
//

#ifndef _GROM_BASEEVENT_H
#define _GROM_BASEEVENT_H

class nsBaseEvent {
public:
    explicit nsBaseEvent(int id);
    virtual ~nsBaseEvent() = default;

    int GetId() const { return _id; }

private:
    int _id;
};

#endif //_GROM_BASEEVENT_H
