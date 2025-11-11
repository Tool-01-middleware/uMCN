#ifndef MC_RATE_CONTROL_H__
#define MC_RATE_CONTROL_H__

#include <rtthread.h>
#include <uMCN.h>
#include <ipc/workqueue.h>

#include "vehicle_angular_acc_temp.h"

class McRateControl {
public:
    McRateControl();

    rt_err_t init();

private:
    static void workHandler(struct rt_work* work, void* parameter);
    static void asyncCallback(const void* data, void* user_data);

    void handleWork();

    struct rt_work work_;
    McnNode_t node_;
    vehicle_angular_acc_msg_t latest_msg_;
};

#endif /* MC_RATE_CONTROL_H__ */

