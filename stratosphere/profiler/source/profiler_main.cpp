/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stratosphere.hpp>

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;
    u32 __nx_fs_num_sessions = 1;

    #define INNER_HEAP_SIZE 0x4000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);
}

namespace ams {

    ncm::ProgramId CurrentProgramId = ncm::SystemProgramId::Profiler;

    namespace result {

        bool CallFatalOnResultAssertion = true;

    }

}

using namespace ams;

void __libnx_initheap(void) {
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	/* Newlib */
	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

void __appInit(void) {
    /* ... */
}

void __appExit(void) {
    /* ... */
}

namespace {

    constexpr size_t NumServers  = 1;
    sf::hipc::ServerManager<NumServers> g_server_manager;

    constexpr sm::ServiceName BananaServiceName = sm::ServiceName::Encode("banana");
    constexpr size_t          BananaMaxSessions = 1;

}

namespace ams::profiler {

    namespace {

        enum class BananaState {
            NotInitialized,
            Initialized,
            Running,
        };

        os::SystemEvent g_banana_event(os::EventClearMode_ManualClear, true);
        BananaState g_banana_state;
        TimeSpan g_banana_wait_span;
        os::ThreadType g_banana_thread;
        alignas(0x1000) u8 g_banana_stack[0x4000];

    }

    namespace impl {

        #define AMS_SM_I_PROFILER_INTERFACE_INFO(C, H)                                             \
            AMS_SF_METHOD_INFO(C, H, 1, Result, GetSystemEvent,      (ams::sf::OutCopyHandle out)) \
            AMS_SF_METHOD_INFO(C, H, 2, Result, StartSignalingEvent, (u64 timespan))               \
            AMS_SF_METHOD_INFO(C, H, 3, Result, StopSignalingEvent,  ())

        AMS_SF_DEFINE_INTERFACE(IProfiler, AMS_SM_I_PROFILER_INTERFACE_INFO)

    }

    R_DEFINE_NAMESPACE_RESULT_MODULE(150);

    R_DEFINE_ERROR_RANGE(ProfilerError, 1, 1023);
        R_DEFINE_ERROR_RESULT(BadState,        7);

    /* Service definition. */
    class Profiler final {
        private:
            static void ThreadFunc(void*) {
                while (g_banana_state == BananaState::Running) {
                    os::SleepThread(g_banana_wait_span);
                    g_banana_event.Signal();
                }
            }
        public:
            Profiler() {
                g_banana_state = BananaState::NotInitialized;
            }

            Result GetSystemEvent(ams::sf::OutCopyHandle out) {
                R_UNLESS(g_banana_state == BananaState::NotInitialized, profiler::ResultBadState());
                
                out.SetValue(g_banana_event.GetReadableHandle());
                g_banana_state = BananaState::Initialized;

                return ResultSuccess();
            }

            Result StartSignalingEvent(u64 timespan) {
                R_UNLESS(g_banana_state == BananaState::Initialized, profiler::ResultBadState());
                
                g_banana_state = BananaState::Running;
                g_banana_wait_span.FromNanoSeconds(timespan);
                R_ABORT_UNLESS(os::CreateThread(&g_banana_thread, ThreadFunc, nullptr, g_banana_stack, sizeof(g_banana_stack), -2, 3));
                os::StartThread(&g_banana_thread);

                return ResultSuccess();
            }

            Result StopSignalingEvent() {
                R_UNLESS(g_banana_state == BananaState::Running, profiler::ResultBadState());

                g_banana_state = BananaState::Initialized;
                os::WaitThread(&g_banana_thread);
                os::DestroyThread(&g_banana_thread);
                
                return ResultSuccess();
            }
    };
    static_assert(impl::IsIProfiler<Profiler>);

}

int main(int argc, char **argv)
{
    /* Create service. */
    R_ABORT_UNLESS((g_server_manager.RegisterServer<profiler::impl::IProfiler, profiler::Profiler>(BananaServiceName, BananaMaxSessions)));

    /* Loop forever, servicing our service. */
    g_server_manager.LoopProcess();

    /* Cleanup */
    return 0;
}

