#include "Core.hpp"
#include "Network.hpp"
#include "utils/LogManager.hpp"
#include <cstdio>
#include <unistd.h>

namespace grc
{

Core::Core(const int port, const std::string& password)
: mPort(port)
, mPassword(password)
{

}

Core::~Core()
{

}

void Core::Run()
{
    /* 초기화 */
    if (Init() == FAILURE)
    {
        LOG(LogLevel::Error) << "Core 초기화 오류";
        return;
    }

    /* 메인 로직 */
    LOG(LogLevel::Notice) << "IRC Server 시작 (Port = " << mPort << ")";
    while (true)
    {
        /* 이벤트 처리 */
        const struct kevent* eventList = mEvent.GetEventList();
        const int eventCount = mEvent.GetEventCount();
        for (int i = 0; i < eventCount; i++)
        {
            const struct kevent& event = eventList[i];
            if (event.ident == STDIN_FILENO && event.filter == EVFILT_READ)
            {
                // ConsoleWindow에서 처리 mConsoleWindow.Read();
            }
            else if (event.ident != STDIN_FILENO && event.filter == EVFILT_READ)
            {
                mNetwork.Read(event.ident);
            }
        }
        /* 새로운 클라이언트 연결 */
        const std::vector<int32>& newClients = mNetwork.FetchNewClients();
        for (size_t i = 0; i < newClients.size(); i++)
        {
            if (mEvent.AddReadEvent(newClients[i]) == FAILURE)
            {
                LOG(LogLevel::Notice) << "Client(" << mNetwork.GetIP(newClients[i]) << ") READ event 등록 실패";
                continue;
            }
            LOG(LogLevel::Notice) << "Client(" << mNetwork.GetIP(newClients[i]) << ") 연결됨";
            mNetwork.ClearNewClients();
        }
        /* IRC 로직 수행 */
        // Todo: IRC 로직 추가
    }
}

int32 Core::Init()
{
    if (mEvent.Init() == FAILURE)
    {
        LOG(LogLevel::Error) << "Event 객체 초기화 오류";
        return FAILURE;
    }
    // if (mConsoleWindow.Init() == FAILURE)
    // {
    //     LOG(LogLevel::Error) << "ConsoleWindow 객체 초기화 오류";
    //     return FAILURE;
    // }
    if (mNetwork.Init(mPort) == FAILURE)
    {
        LOG(LogLevel::Error) << "Network 객체 초기화 오류";
        return FAILURE;
    }

    /* Add basic events */
    // if (mEvent.AddReadEvent(STDIN_FILENO) == FAILURE)
    // {
    //     LOG(LogLevel::Error) << "STDIN event 등록 오류";
    //     return FAILURE;
    // }
    if (mEvent.AddReadEvent(mNetwork.GetServerSocket()) == FAILURE)
    {
        LOG(LogLevel::Error) << "server socket event 등록 오류";
        return FAILURE;
    }

    return SUCCESS;
}

}
