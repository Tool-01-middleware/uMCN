#ifndef MCN_PUB_WORKQUEUE_H__
#define MCN_PUB_WORKQUEUE_H__

#include <rtthread.h>
#include <uMCN.h>
#include "vehicle_angular_acc_temp.h"

#ifdef __cplusplus
extern "C" {
#endif

rt_err_t mcn_pub_workqueue_init(void);
const McnHub* mcn_pub_workqueue_get_hub(void);

#ifdef __cplusplus
}
#endif

#endif /* MCN_PUB_WORKQUEUE_H__ */

