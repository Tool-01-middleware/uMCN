#ifndef WORKQUEUE_MANAGE_H__
#define WORKQUEUE_MANAGE_H__

#include <rtthread.h>
#include <ipc/workqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

rt_err_t workqueue_manage_init(void);
rt_err_t add_dowork_rateCtrl(struct rt_work* work);
struct rt_workqueue* workqueue_manage_get(void);

#ifdef __cplusplus
}
#endif

#endif /* WORKQUEUE_MANAGE_H__ */

