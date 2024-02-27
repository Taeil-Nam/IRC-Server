#include "Event.hpp"
#include "common.hpp"

namespace grc
{

Event::Event()
{
    mKqueue = ERROR;
    mEventCount = 0;
    std::memset(mEventList, 0, sizeof(mEventList));
}

Event::~Event()
{
    close(mKqueue);
}

int32 Event::Init()
{
    if (createKqueue() == FAILURE)
    {
        return FAILURE;
    }
    return SUCCESS;
}

int32 Event::AddReadEvent(const int32 fd)
{
    struct kevent newEvent;
    EV_SET(&newEvent, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mKqueue, &newEvent, 1, NULL, 0, NULL) == ERROR)
    {
        LOG(LogLevel::Error) << "READ event 등록 오류(errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return FAILURE;
    }
    return SUCCESS;
}

const struct kevent* Event::GetEventList()
{
    mEventCount = kevent(mKqueue, NULL, 0, mEventList, MAX_KEVENT_SIZE, NULL);
    if (mEventCount == ERROR)
    {
        LOG(LogLevel::Error) << "Event list 생성 오류(errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return NULL;
    }
    return mEventList;
}

const int32 Event::GetEventCount() const
{
    return mEventCount;
}

int32 Event::createKqueue()
{
    mKqueue = kqueue();
    if (mKqueue == ERROR)
    {
        LOG(LogLevel::Error) << "Kqueue 생성 오류(errno:" << errno << " - "
            << strerror(errno) << ") on kqueue()";
        return FAILURE;
    }
    return SUCCESS;
}

}
