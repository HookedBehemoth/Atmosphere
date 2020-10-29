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
#pragma once
#include <stratosphere.hpp>

namespace ams::npns {

    template <typename Base, size_t StackSize>
    class ThreadTemplate {
        alignas(os::MemoryPageSize) u8 stack[StackSize];
        os::ThreadType thread;

      public:
        const char *thread_name;
        bool should_exit;
        u32 thread_prio;

        static void ThreadStarter(void *userdata) {
            reinterpret_cast<ThreadTemplate<Base, StackSize> *>(userdata)->ThreadFunction();
        }

      public:
        ThreadTemplate(const char *name, u32 prio)
            : thread_name(name),
              thread_prio(prio) {
            std::memset(&this->thread, 0, 0x1c0);
        }
        virtual ~ThreadTemplate() = default;

        Result Initialize() {
            this->should_exit = false;
            R_TRY(os::CreateThread(&this->thread, ThreadStarter, this, this->stack, StackSize, this->thread_prio));

            os::SetThreadNamePointer(&this->thread, this->thread_name);
            os::StartThread(&this->thread);

            return ResultSuccess();
        }

        virtual void Finalize() {
            this->SignalExit();
            os::WaitThread(&this->thread);
            os::DestroyThread(&this->thread);
        }

        virtual void SignalExit() {
            this->should_exit = true;
        }

        virtual void ThreadFunction() = 0;
    };

}
