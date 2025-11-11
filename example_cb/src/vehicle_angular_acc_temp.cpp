#include "vehicle_angular_acc_temp.h"

extern "C" {
#include "workqueue_manage.h"
#include "mcn_pub_workqueue.h"
#define LOG_TAG "vehicle_acc"
#define LOG_LVL LOG_LVL_INFO
#include <ulog.h>
}

#include <cstring>

MCN_DEFINE(angular_acc_filter, sizeof(vehicle_angular_acc_msg_t));

extern "C" const McnHub* vehicle_angular_acc_filter_hub(void)
{
    return MCN_HUB(angular_acc_filter);
}

VehicleAngularAccTemp::VehicleAngularAccTemp()
    : node_(RT_NULL)
{
    std::memset(&latest_msg_, 0, sizeof(latest_msg_));
    rt_work_init(&work_, VehicleAngularAccTemp::workHandler, this);
}

rt_err_t VehicleAngularAccTemp::init()
{
    rt_err_t ret;
    const McnHub* source_hub = mcn_pub_workqueue_get_hub();
    if (source_hub == RT_NULL) {
        LOG_E("source hub null");
        return -RT_ERROR;
    }

    ret = mcn_advertise(MCN_HUB(angular_acc_filter), RT_NULL);
    if (ret != RT_EOK && ret != -RT_EBUSY) {
        LOG_E("advertise filter topic failed (%d)", ret);
        return ret;
    }

    node_ = mcn_subscribe((McnHub*)source_hub, RT_NULL, RT_NULL);
    if (node_ == RT_NULL) {
        LOG_E("subscribe source topic failed");
        return -RT_ERROR;
    }

    ret = mcn_register_async_cb(node_, VehicleAngularAccTemp::asyncCallback, this);
    if (ret != RT_EOK) {
        LOG_E("register async cb failed (%d)", ret);
        return ret;
    }

    LOG_I("initialized");
    return RT_EOK;
}

void VehicleAngularAccTemp::workHandler(struct rt_work* work, void* parameter)
{
    (void)work;
    if (parameter == RT_NULL) {
        return;
    }

    static_cast<VehicleAngularAccTemp*>(parameter)->handleWork();
}

void VehicleAngularAccTemp::asyncCallback(const void* data, void* user_data)
{
    if ((data == RT_NULL) || (user_data == RT_NULL)) {
        return;
    }

    VehicleAngularAccTemp* instance = static_cast<VehicleAngularAccTemp*>(user_data);
    std::memcpy(&instance->latest_msg_, data, sizeof(instance->latest_msg_));

    if (add_dowork_rateCtrl(&instance->work_) != RT_EOK) {
        LOG_E("submit work failed");
    }
}

void VehicleAngularAccTemp::handleWork()
{
    LOG_I("seq:%u angular_acc(%.2f, %.2f, %.2f)",
        latest_msg_.seq,
        latest_msg_.x,
        latest_msg_.y,
        latest_msg_.z);

    if (mcn_publish(MCN_HUB(angular_acc_filter), &latest_msg_) != RT_EOK) {
        LOG_E("publish filter topic failed");
    }
}

