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
#include <curl/curl.h>

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;

    #define INNER_HEAP_SIZE 589824
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

    ncm::ProgramId CurrentProgramId = ncm::SystemProgramId::Npns;

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

void __appInit(void) {
    hos::InitializeForStratosphere();

    /* Curl requires an sm session for sfdnsres, so we'll keep it. */
    R_ABORT_UNLESS(smInitialize());
    R_ABORT_UNLESS(nifmInitialize(NifmServiceType_System));
    R_ABORT_UNLESS(socketInitialize(&npns::g_SocketConfig));
    AMS_ABORT_UNLESS(curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK);
    R_ABORT_UNLESS(accountInitialize(AccountServiceType_System));

    ams::CheckApiVersion();
}

void __appExit(void) {
    accountExit();
    curl_global_cleanup();
    socketExit();
    nifmExit();
    smExit();
}

int main(int argc, char **argv) {
    /* Set thread name. */
    os::SetThreadNamePointer(os::GetCurrentThread(), AMS_GET_SYSTEM_THREAD_NAME(npns, Main));

    /* Official npns changed the main thread priority from 35 in earlier firmwares to 33. */
    os::ChangeThreadPriority(os::GetCurrentThread(), AMS_GET_SYSTEM_THREAD_PRIORITY(npns, Main));
    AMS_ASSERT(os::GetThreadPriority(os::GetCurrentThread()) == AMS_GET_SYSTEM_THREAD_PRIORITY(npns, Main));

    /* Initialize the npns server daemon. */
    npns::InitializeForNotificationDaemon();

    /* Run the npns server daemon. */
    npns::RunNotificationDaemon();

    /* Finalize npns server daemon. */
    npns::FinalizeForNotificationDaemon();

    /* Cleanup */
    return 0;
}
