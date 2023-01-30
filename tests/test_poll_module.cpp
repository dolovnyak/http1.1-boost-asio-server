#include <gtest/gtest.h>

#define private public

#include "PollModule.h"
#include "HttpSession.h"
#include <sys/poll.h>
#include <sys/socket.h>

template<class CoreModule>
class TestSession : public Session<CoreModule> {
public:
    TestSession(int core_module_index, CoreModule* core_module, SocketFd socket)
            : Session<CoreModule>(core_module_index, core_module, socket) {}

    ~TestSession() {}

    bool ShouldCloseAfterResponse() const OVERRIDE {
        return true;
    }

    SessionType::Type GetType() const override {
        return SessionType::Http;
    }

    const std::string& GetResponseData() const override {
        throw std::logic_error("TestSession::GetResponseData() should not be called");
    }

    const std::string& GetName() const override {
        static std::string kName = "TestSession";
        return kName;
    }
};

TEST(Poll_Module, Process_Compress_Basic) {
    std::shared_ptr<Config> config = MakeShared(Config());
    std::queue<std::shared_ptr<Event> > event_queue;
    std::unordered_map<SocketFd, std::shared_ptr<Session<PollModule> > > sessions;

    PollModule poll_module(config, &event_queue, &sessions);
    poll_module._should_compress = true;
    poll_module._poll_fds = new struct pollfd[21];
    poll_module._poll_fds_size = 21;
    poll_module._poll_index = 21;

    for (int i = 0; i < 21; ++i) {
        if (i % 2 == 0) {
            poll_module._poll_fds[i].fd = i;
        }
        else {
            poll_module._poll_fds[i].fd = -1;
        }
    }

    poll_module.ProcessCompress();
    for (int i = 0; i < 11; ++i) {
        EXPECT_EQ(poll_module._poll_fds[i].fd, i * 2);
    }

    poll_module._should_compress = true;
    poll_module._poll_index = 3;
    poll_module._poll_fds[0].fd = 1;
    poll_module._poll_fds[1].fd = 2;
    poll_module._poll_fds[2].fd = -1;
    poll_module.ProcessCompress();
    ASSERT_EQ(poll_module._poll_index, 2);
    ASSERT_EQ(poll_module._poll_fds[0].fd, 1);
    ASSERT_EQ(poll_module._poll_fds[1].fd, 2);
}

TEST(Poll_Module, Process_Compress_Correct_Update_Sessions) {
    std::shared_ptr<Config> config = MakeShared(Config());
    std::queue<std::shared_ptr<Event> > event_queue;
    std::unordered_map<SocketFd, std::shared_ptr<Session<PollModule> > > sessions;

    PollModule poll_module(config, &event_queue, &sessions);
    poll_module._should_compress = true;
    poll_module._poll_fds = new struct pollfd[21];
    poll_module._poll_fds_size = 21;

    for (int i = 0; i < 21; ++i) {
        std::shared_ptr<Session<PollModule> > server_session = MakeShared<Session<PollModule> >(new TestSession<PollModule>(
                poll_module.GetNextSessionIndex(), &poll_module, SocketFd(i + 30)));
        poll_module.AddSession(i + 30, server_session);
    }
    ASSERT_EQ(poll_module._poll_index, 21);

    for (int i = 0; i < 5; ++i) {
        Session<PollModule>::It session_it = sessions.find(SocketFd(i + 30));

        session_it->second->available = false;
        sessions.erase(session_it);

        poll_module._poll_fds[i].fd = -1;
    }

    poll_module.ProcessCompress();

    ASSERT_EQ(poll_module._sessions->size(), 16);
    ASSERT_EQ(poll_module._poll_index, 16);
    for (int i = 0; i < 16; ++i) {
        EXPECT_EQ(poll_module._poll_fds[i].fd, i + 35);
    }
}