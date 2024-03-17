#include "Event.hpp"

namespace grc
{

Event::Event()
: MAX_KEVENT_SIZE(128)
, mKqueue(ERROR)
, mEventCount(0)
, mEventList(NULL)
{
    SetTimeout(5);
}

Event::~Event()
{
    close(mKqueue);
    delete [] mEventList;
}

bool Event::Init()
{
    mEventList = new (std::nothrow) struct kevent[MAX_KEVENT_SIZE];
    if (mEventList == NULL)
    {
        return FAILURE;
    }
    std::memset(mEventList, 0, sizeof(struct kevent) * MAX_KEVENT_SIZE);
    if (createKqueue() == FAILURE)
    {
        return FAILURE;
    }
    return SUCCESS;
}

bool Event::AddReadEvent(const int32 fd)
{
    struct kevent newEvent;
    EV_SET(&newEvent, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mKqueue, &newEvent, 1, NULL, 0, NULL) == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to add READ event(errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return FAILURE;
    }
    return SUCCESS;
}

bool Event::AddWriteEvent(const int32 fd)
{
    struct kevent newEvent;
    EV_SET(&newEvent, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mKqueue, &newEvent, 1, NULL, 0, NULL) == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to add WRITE event(errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return FAILURE;
    }
    return SUCCESS;
}


const struct kevent* Event::GetEventList()
{
    mEventCount = kevent(mKqueue, NULL, 0,
                         mEventList, MAX_KEVENT_SIZE, &mTimeout);
    if (mEventCount == ERROR)
    {
        LOG(LogLevel::Error) << "Failed to generate Event list (errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return NULL;
    }
    return mEventList;
}

int32 Event::GetEventCount() const
{
    return mEventCount;
}

bool Event::createKqueue()
{
    mKqueue = kqueue();
    if (mKqueue == ERROR)
    {
        LOG(LogLevel::Error) << "Faild to excute Kqueue (errno:" << errno << " - "
            << strerror(errno) << ") on kqueue()";
        return FAILURE;
    }
    return SUCCESS;
}

void Event::SetTimeout(const int64 ms)
{
    mTimeout.tv_sec = ms / 1000;
    mTimeout.tv_nsec = (ms % 1000) * 1000 * 1000;
}

}
