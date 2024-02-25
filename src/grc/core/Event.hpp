/**
 * @file Event.hpp
 * @author Taeil-Nam (nam0314@gmail.com)
 * @brief IO event와 관련된 역할을 수행하는 클래스
 * @version 0.1
 * @date 2024-02-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <sys/event.h>
#include "common.hpp"

namespace grc
{

class Event
{
public:
    Event();
    ~Event();

    int InitEvent();
    int AddReadEvent(const int fd);
    int AddWriteEvent(const int fd);
    struct kevent* GetEventList();
    int GetEventCount() const;
private:
    Event(const Event& event); // = delete
    const Event& operator=(const Event& event); // = delete

    int createKqueue();
private:
    int mKqueue;
    struct kevent mEventList[MAX_KEVENT_SIZE];
    int mEventCount;
};
 
}
