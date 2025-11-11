#include "mc_rate_control.h"

extern "C" {
#include "workqueue_manage.h"
#define LOG_TAG "mc_rate_ctrl"
#define LOG_LVL LOG_LVL_INFO
#include <ulog.h>
}

#include <cstring>

McRateControl::McRateControl()
    : node_(RT_NULL)
{
    std::memset(&latest_msg_, 0, sizeof(latest_msg_));
    rt_work_init(&work_, McRateControl::workHandler, this);
}

rt_err_t McRateControl::init()
{
    rt_err_t ret = workqueue_manage_init();
    if (ret != RT_EOK) {
        LOG_E("workqueue init failed (%d)", ret);
        return ret;
    }

    const McnHub* hub = vehicle_angular_acc_filter_hub();
    if (hub == RT_NULL) {
        LOG_E("filter hub null");
        return -RT_ERROR;
    }

    node_ = mcn_subscribe((McnHub*)hub, RT_NULL, RT_NULL);
    if (node_ == RT_NULL) {
        LOG_E("subscribe filter topic failed");
        return -RT_ERROR;
    }

    ret = mcn_register_async_cb(node_, McRateControl::asyncCallback, this);
    if (ret != RT_EOK) {
        LOG_E("register async cb failed (%d)", ret);
        return ret;
    }

    LOG_I("initialized");
    return RT_EOK;
}

void McRateControl::workHandler(struct rt_work* work, void* parameter)
{
    (void)work;
    if (parameter == RT_NULL) {
        return;
    }

    static_cast<McRateControl*>(parameter)->handleWork();
}

void McRateControl::asyncCallback(const void* data, void* user_data)
{
    if ((data == RT_NULL) || (user_data == RT_NULL)) {
        return;
    }

    McRateControl* instance = static_cast<McRateControl*>(user_data);
    std::memcpy(&instance->latest_msg_, data, sizeof(instance->latest_msg_));

    if (add_dowork_rateCtrl(&instance->work_) != RT_EOK) {
        LOG_E("submit work failed");
    }
}

void McRateControl::handleWork()
{
    LOG_I("process seq:%u angular_acc(%.2f, %.2f, %.2f)",
        latest_msg_.seq,
        latest_msg_.x,
        latest_msg_.y,
        latest_msg_.z);
}

