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

    bool Init();
    int32 AddReadEvent(const int32 fd);
    int32 AddWriteEvent(const int32 fd);
    const struct kevent* GetEventList();
    int32 GetEventCount() const;
    void SetTimeout(const int64 ms);
private:
    bool createKqueue();
    
    Event(const Event& event); // = delete
    const Event& operator=(const Event& event); // = delete
private:
    const int32 MAX_KEVENT_SIZE;
    int32 mKqueue;
    struct kevent* mEventList;
    int32 mEventCount;
    struct timespec mTimeout;
};
 
}
