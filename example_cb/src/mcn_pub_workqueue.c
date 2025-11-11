#include "mcn_pub_workqueue.h"

#include <string.h>

#define LOG_TAG "mcn_pub_wq"
#define LOG_LVL LOG_LVL_INFO
#include <ulog.h>

#define PUB_THREAD_NAME          "ang_pub"
#define PUB_THREAD_STACK_SIZE    1024
#define PUB_THREAD_PRIORITY      (RT_THREAD_PRIORITY_MAX / 2)
#define PUB_THREAD_TIMESLICE     20

static rt_thread_t pub_thread = RT_NULL;

MCN_DEFINE(vehicle_angular_acc, sizeof(vehicle_angular_acc_msg_t));

static void vehicle_angular_acc_pub_entry(void* parameter)
{
    vehicle_angular_acc_msg_t msg = { 0 };

    RT_UNUSED(parameter);

    while (1) {
        msg.seq++;
        msg.x = 0.1f * msg.seq;
        msg.y = 0.2f * msg.seq;
        msg.z = 0.3f * msg.seq;

        if (mcn_publish(MCN_HUB(vehicle_angular_acc), &msg) != RT_EOK) {
            LOG_E("publish failed");
        } else {
            LOG_I("publish seq:%u angular_acc(%.2f, %.2f, %.2f)",
                msg.seq,
                msg.x,
                msg.y,
                msg.z);
        }

        rt_thread_mdelay(3000);
    }
}

rt_err_t mcn_pub_workqueue_init(void)
{
    rt_err_t ret;

    ret = mcn_advertise(MCN_HUB(vehicle_angular_acc), RT_NULL);
    if (ret != RT_EOK) {
        return ret;
    }

    if (pub_thread == RT_NULL) {
        pub_thread = rt_thread_create(PUB_THREAD_NAME,
            vehicle_angular_acc_pub_entry,
            RT_NULL,
            PUB_THREAD_STACK_SIZE,
            PUB_THREAD_PRIORITY,
            PUB_THREAD_TIMESLICE);
        if (pub_thread == RT_NULL) {
            LOG_E("create publisher thread failed");
            return -RT_ENOMEM;
        }
        rt_thread_startup(pub_thread);
    }

    return RT_EOK;
}

const McnHub* mcn_pub_workqueue_get_hub(void)
{
    return MCN_HUB(vehicle_angular_acc);
}

