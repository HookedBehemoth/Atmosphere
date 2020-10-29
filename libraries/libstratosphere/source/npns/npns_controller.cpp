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
#include "npns_controller.hpp"

namespace ams::npns {

    namespace {

        char environment[4];

        Result npns_save_mount() {
            fs::DisableAutoSaveDataCreation();
            R_TRY_CATCH(fs::MountSystemSaveData("npns_save", 0x110)) {
                R_CATCH(fs::ResultTargetNotFound) {
                    R_TRY(fs::CreateSystemSaveData(0x110, 0x20000, 0x10000, fs::SaveDataFlags_KeepAfterResettingSystemSaveDataWithoutUserSaveData));
                    R_TRY(fs::MountSystemSaveData("npns_save", 0x110));
                }
            }
            R_END_TRY_CATCH;

            return ResultSuccess();
        }

        void npns_save_unmount() {
            fs::Unmount("npns_save");
        }

        Result npns_persistent_read(JabberHelper &jabber) {
            std::memset(&jabber, 0, sizeof(jabber));
            jabber.unk = 1;
            fs::FileHandle persistent;
            R_TRY(fs::OpenFile(&persistent, "npns_save:/persistent.bin", fs::OpenMode_Read));
            /* TODO: On official software this is a try catch but it doesn't really make a lot of sense. */
            /*R_TRY_CATCH(fs::OpenFile(&persistent, "npns_save:/persistent.bin", fs::OpenMode_Read))) {
                R_CATCH_RETHROW(fs::ResultPathNotFound);
                R_CONVERT_ALL(ResultSuccess);
            } R_END_TRY_CATCH;*/

            fs::ReadFile(persistent, 0, &jabber, sizeof(jabber));
            fs::CloseFile(persistent);

            return ResultSuccess();
        }

        Result npns_persistent_write(JabberHelper &jabber) {
            fs::FileHandle persistent;
            R_TRY_CATCH(fs::OpenFile(&persistent, "npns_save:/persistent.bin", fs::OpenMode_Write | fs::OpenMode_AllowAppend)) {
                R_CATCH(fs::ResultPathNotFound) {
                    R_TRY(fs::CreateFile("npns_save:/persistent.bin", 0x8c));
                    R_TRY(fs::OpenFile(&persistent, "npns_save:/persistent.bin", fs::OpenMode_Write | fs::OpenMode_AllowAppend));
                }
            } R_END_TRY_CATCH;

            ON_SCOPE_EXIT { fs::CloseFile(persistent); };

            R_TRY(fs::WriteFile(persistent, 0, &jabber, sizeof(jabber), fs::WriteOption::Flush));
            return fs::CommitSaveData("npns_save");
        }

    }

    Controller::Controller(ClientThread *client_thread)
        : event_0x0(os::EventClearMode_AutoClear),
          client_thread(client_thread),
          /* connection_broker(), */
          timeout_helper() {
        /* DAuth, Notification Token and Jid http helper. */
    }

    Result Controller::Initialize() {
        /* nn::nifm::Request::Request */
        /* nn::nifm::RequestClient::RequestClient */
        /* nn::nifm::RequestClient::SetRequiredPreset */
        /* nn::nifm::RequestClient::FUN_7100031dec */
        /* nn::nifm::RequestClient::FUN_7100031e90 */
        /* nn::nifm::RequestClient::FUN_7100031e9c */
        R_TRY(npns_save_mount());
        R_TRY(npns_persistent_read(this->jabber));

        if (*environment == '\0')
            nsdGetEnvionmentIdentifier(environment, sizeof(environment));

        this->jabber.has_jid = std::memcmp(this->jabber.env, &environment, 4) == 0;

        return ResultSuccess();
    }

    void Controller::Finalize() {
        npns_save_unmount();
        /* nn::http::ConnectionBroker::Finalize() */
        /* nn::nifm::RequestClient::~RequestClient */
    }

    Result Controller::Connect() {
        return this->client_thread->Connect(this->jabber.jid.jid, this->jabber.jid.pass);
    }

    void Controller::Restart() {
        return this->client_thread->Restart();
    }

    bool Controller::IsAuthenticated() {
        return this->client_thread->IsAuthenticated();
    }

    bool Controller::IsUnauthenticated() {
        return this->client_thread->IsUnauthenticated();
    }

    Result Controller::GetResult() {
        return this->client_thread->GetResult();
    }

    Result Controller::CreateJid() {
        /* TODO */
        return ResultSuccess();
    }

    Result Controller::DestroyJid() {
        /* TODO */
        return ResultSuccess();
    }

    Result Controller::SetJid(JabberIdentifier &jid) {
        R_UNLESS(!this->jabber.has_jid, npns::ResultJidAlreadySet());

        std::memcpy(&this->jabber.jid, &jid, sizeof(jid));
        this->jabber.has_jid = true;

        return ResultSuccess();
    }

    Result Controller::ClearJid() {
        this->jabber.has_jid = false;
        std::memset(&this->jabber.jid, 0, sizeof(JabberIdentifier));
        return npns_persistent_write(this->jabber);
    }

    void Controller::SetHasJid() {
        this->jabber.has_jid = true;
    }

    Result Controller::CreateTokenWithApplicationId() {
        /* TODO */
        return ResultSuccess();
    }

    bool Controller::HasJid() {
        return this->jabber.has_jid;
    }

    Result Controller::DestroyTokenWithApplicationId() {
        /* TODO */
        return ResultSuccess();
    }

    Result Controller::WaitForNetworkAvailability() {
        /* TODO */
        return ResultSuccess();
    }

    bool Controller::IsNetworkAvailable() {
        /* TODO */
        return false;
    }
    Result Controller::FUN_710001688c() {
        /* TODO */
        return ResultSuccess();
    }

    Result Controller::FUN_7100016968() {
        /* TODO */
        return ResultSuccess();
    }

    void Controller::Submit() {
        nifmRequestSubmit(&this->request_client);
    }

    void Controller::Cancel() {
        nifmRequestCancel(&this->request_client);
    }

    Result Controller::GetJid(JabberIdentifier &out) {
        R_UNLESS(this->jabber.has_jid, npns::ResultJidNotSet());

        std::memcpy(&out, &this->jabber.jid, sizeof(out));

        return ResultSuccess();
    }

}
