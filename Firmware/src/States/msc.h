#pragma once

#include <memory>

#include <libriccore/fsm/state.h>
#include <libriccore/systemstatus/systemstatus.h>
#include <libriccore/commands/commandhandler.h>

#include "Config/pinmap_config.h"
#include "Config/systemflags_config.h"
#include "Config/types.h"

#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "tinyusb_msc.h"
#include "esp_log.h"
#include "tinyusb_cdc_acm.h"
#include "tinyusb_console.h"   // esp_tusb_init_console()
#include "Storage/msc_nand_glue.h" // msc_nand_attach()

class MSC : public Types::CoreTypes::State_t
{
    public:
        MSC(System& system);

        void initialize() override;

        Types::CoreTypes::State_ptr_t update() override;

        void exit() override;

    private:
        System& _system;

};

static void msc_evt_cb(tinyusb_msc_storage_handle_t h,
                       tinyusb_msc_event_t *e, void *arg)
{
    // Optional: react to mount/unmount/format events
    // e->id: TINYUSB_MSC_EVENT_MOUNT_START / _COMPLETE / _FAILED / _FORMAT_REQUIRED / _FORMAT_FAILED
}