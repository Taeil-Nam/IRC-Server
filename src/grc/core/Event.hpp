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

#define MAX_KEVENT_SIZE 128

namespace grc
{

class Event
{
public:
    Event();
    ~Event();

    int32 Init();
    int32 AddReadEvent(const int32 fd);
    const struct kevent* GetEventList();
    const int32 GetEventCount() const;
private:
    Event(const Event& event); // = delete
    const Event& operator=(const Event& event); // = delete

    int32 createKqueue();
private:
    int32 mKqueue;
    struct kevent mEventList[MAX_KEVENT_SIZE];
    int32 mEventCount;
};
 
}
