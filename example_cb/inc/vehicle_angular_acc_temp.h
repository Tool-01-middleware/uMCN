#ifndef VEHICLE_ANGULAR_ACC_TEMP_H__
#define VEHICLE_ANGULAR_ACC_TEMP_H__

#include <rtthread.h>
#include <uMCN.h>
#include <ipc/workqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x;
    float y;
    float z;
    rt_uint32_t seq;
} vehicle_angular_acc_msg_t;

#ifdef __cplusplus
const McnHub* vehicle_angular_acc_filter_hub(void);
#else
const struct McnHub* vehicle_angular_acc_filter_hub(void);
#endif

#ifdef __cplusplus
class VehicleAngularAccTemp {
public:
    VehicleAngularAccTemp();

    rt_err_t init();

private:
    static void workHandler(struct rt_work* work, void* parameter);
    static void asyncCallback(const void* data, void* user_data);

    void handleWork();

    struct rt_work work_;
    McnNode_t node_;
    vehicle_angular_acc_msg_t latest_msg_;
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* VEHICLE_ANGULAR_ACC_TEMP_H__ */

