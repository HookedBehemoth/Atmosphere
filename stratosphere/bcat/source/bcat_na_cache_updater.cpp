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
#include "bcat_na_cache_updater.hpp"

namespace ams::bcat::server {

    namespace {

        os::ThreadType g_CacheUpdaterThread;
        alignas(os::MemoryPageSize) u8 g_CacheUpdaterThreadStack[0x4000];
        os::EventType g_CacheUpdaterExitEvent;

        void NaCacheUpdater(void *) {
        }

    }

    void CacheUpdaterInitialize() {
        os::CreateThread(&g_CacheUpdaterThread, NaCacheUpdater, nullptr, g_CacheUpdaterThreadStack, sizeof(g_CacheUpdaterThreadStack), AMS_GET_SYSTEM_THREAD_PRIORITY(bcat, NaCacheUpdater));
        os::SetThreadNamePointer(&g_CacheUpdaterThread, AMS_GET_SYSTEM_THREAD_NAME(bcat, NaCacheUpdater));
        os::ClearEvent(&g_CacheUpdaterExitEvent);
        os::StartThread(&g_CacheUpdaterThread);
    }

    void CacheUpdaterFinalize() {
        os::SignalEvent(&g_CacheUpdaterExitEvent);
        os::WaitThread(&g_CacheUpdaterThread);
        os::DestroyThread(&g_CacheUpdaterThread);
    }

}