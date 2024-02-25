#include "Event.hpp"
#include "common.hpp"

namespace grc
{

Event::Event()
{

}

Event::~Event()
{
    close(mKqueue);
}

int Event::InitEvent()
{
    if (createKqueue() == FAILURE)
    {
        return FAILURE;
    }
    return SUCCESS;
}

int Event::AddReadEvent(const int fd)
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

int Event::AddWriteEvent(const int fd)
{
    struct kevent newEvent;
    EV_SET(&newEvent, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(mKqueue, &newEvent, 1, NULL, 0, NULL) == ERROR)
    {
        LOG(LogLevel::Error) << "WRITE event 등록 오류(errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return FAILURE;
    }
    return SUCCESS;
}

struct kevent* Event::GetEventList()
{
    std::memset(mEventList, 0, sizeof(mEventList));
    mEventCount = kevent(mKqueue, NULL, 0, mEventList, MAX_KEVENT_SIZE, NULL);
    if (mEventCount == ERROR)
    {
        LOG(LogLevel::Error) << "Event list 생성 오류(errno:" << errno << " - "
            << strerror(errno) << ") on kevent()";
        return NULL;
    }
    return mEventList;
}

int Event::GetEventCount() const
{
    return mEventCount;
}

int Event::createKqueue()
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
