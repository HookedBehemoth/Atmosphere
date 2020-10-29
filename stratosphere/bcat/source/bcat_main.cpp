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

#define INNER_HEAP_SIZE 0x18000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char nx_inner_heap[INNER_HEAP_SIZE];

void __libnx_initheap(void);
void __appInit(void);
void __appExit(void);

/* Exception handling. */
alignas(16) u8 __nx_exception_stack[ams::os::MemoryPageSize];
u64 __nx_exception_stack_size = sizeof(__nx_exception_stack);
void __libnx_exception_handler(ThreadExceptionDump *ctx);
}

namespace ams {
    ncm::ProgramId CurrentProgramId = ncm::SystemProgramId::Bcat;

    namespace result {

        bool CallFatalOnResultAssertion = true;

    }

}

using namespace ams;

void __libnx_exception_handler(ThreadExceptionDump *ctx) {
    ams::CrashHandler(ctx);
}

void __libnx_initheap(void) {
    void *addr  = nx_inner_heap;
    size_t size = nx_inner_heap_size;

    /* Newlib */
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    fake_heap_start = (char *)addr;
    fake_heap_end   = (char *)addr + size;
}

namespace {

    constexpr const SocketInitConfig BcatSocketConfig = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size     = 0x8000,
        .tcp_rx_buf_size     = 0x10000,
        .tcp_tx_buf_max_size = 0x5b000,
        .tcp_rx_buf_max_size = 0x5b000,

        .udp_tx_buf_size = 0x2400,
        .udp_rx_buf_size = 0xA500,

        .sb_efficiency = 3,

        .num_bsd_session  = 2,
        .bsd_service_type = BsdServiceType_User,
    };

}

void __appInit(void) {
    hos::InitializeForStratosphere();

    ams::sm::DoWithSession([] {
        R_ABORT_UNLESS(socketInitialize(&BcatSocketConfig));
        R_ABORT_UNLESS(nifmInitialize(NifmServiceType_System));
        R_ABORT_UNLESS(ams::time::InitializeForSystem());
        //R_ABORT_UNLESS(ommInitialize());
        R_ABORT_UNLESS(accountInitialize(AccountServiceType_System));
        //R_ABORT_UNLESS(npnsInitialize(NpnsServiceType_System));
    });
    ams::CheckApiVersion();
}

void __appExit(void) {
    //npnsExit();
    accountExit();
    //ommExit();
    ams::time::Finalize();
    nifmExit();
    socketExit();
}

int main(int argc, char **argv) {
    /* Set thread name. */
    os::SetThreadNamePointer(os::GetCurrentThread(), AMS_GET_SYSTEM_THREAD_NAME(bcat, Main));

    /* Official bcat changed the main thread priority from 32 in earlier firmwares to 30. */
    os::ChangeThreadPriority(os::GetCurrentThread(), AMS_GET_SYSTEM_THREAD_PRIORITY(bcat, Main));
    AMS_ASSERT(os::GetThreadPriority(os::GetCurrentThread()) == AMS_GET_SYSTEM_THREAD_PRIORITY(bcat, Main));

    /* Initialize the capsrv library. */
    R_ABORT_UNLESS(capsrv::server::InitializeForDecoderServer());

    /* Service the decoder server. */
    capsrv::server::DecoderControlServerThreadFunction(nullptr);

    /* Finalize the capsrv library. */
    capsrv::server::FinalizeForDecoderServer();

    /* Cleanup */
    return 0;
}
