/******************************************************************************
 * Main queue controller for uMCN example components.
 */

extern "C" {
#define LOG_TAG "main_queue"
#define LOG_LVL LOG_LVL_INFO
#include <finsh.h>
#include <rtthread.h>
#include <ulog.h>
#include "workqueue_manage.h"
#include "mcn_pub_workqueue.h"
#include "example_cb.h"
}

#include "vehicle_angular_acc_temp.h"
#include "mc_rate_control.h"

static VehicleAngularAccTemp g_vehicle_acc;
static McRateControl g_mc_rate_ctrl;

static int main_queue_mcn_entry(int argc, char** argv)
{
    RT_UNUSED(argc);
    RT_UNUSED(argv);

    static rt_bool_t initialized = RT_FALSE;
    if (initialized) {
        LOG_I("already initialized");
        return RT_EOK;
    }

    if (workqueue_manage_init() != RT_EOK) {
        LOG_E("workqueue init failed");
        return -RT_ERROR;
    }

    if (mcn_pub_workqueue_init() != RT_EOK) {
        LOG_E("mcn publisher init failed");
        return -RT_ERROR;
    }

    if (g_vehicle_acc.init() != RT_EOK) {
        LOG_E("vehicle angular acc init failed");
        return -RT_ERROR;
    }

    if (g_mc_rate_ctrl.init() != RT_EOK) {
        LOG_E("mc rate control init failed");
        return -RT_ERROR;
    }

    initialized = RT_TRUE;
    LOG_I("initialized successfully");
    return RT_EOK;
}

MSH_CMD_EXPORT_ALIAS(main_queue_mcn_entry, main_queue_mcn, start main queue uMCN demo);

