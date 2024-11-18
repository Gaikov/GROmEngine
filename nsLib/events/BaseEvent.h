//
// Created by Roman on 5/3/2024.
//

#pragma once

class nsBaseEvent {
public:
    explicit nsBaseEvent(int id);
    virtual ~nsBaseEvent() = default;

    int GetId() const { return _id; }

private:
    int _id;
};

