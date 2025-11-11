#include "workqueue_manage.h"

#define RATE_CTRL_WQ_NAME            "rate_ctrl"
#define RATE_CTRL_WQ_STACK_SIZE      1536
#define RATE_CTRL_WQ_PRIORITY        (RT_THREAD_PRIORITY_MAX / 3)

static struct rt_workqueue* rate_ctrl_wq = RT_NULL;

rt_err_t workqueue_manage_init(void)
{
    if (rate_ctrl_wq != RT_NULL) {
        return RT_EOK;
    }

    rate_ctrl_wq = rt_workqueue_create(RATE_CTRL_WQ_NAME,
        RATE_CTRL_WQ_STACK_SIZE,
        RATE_CTRL_WQ_PRIORITY);
    if (rate_ctrl_wq == RT_NULL) {
        return -RT_ENOMEM;
    }

    return RT_EOK;
}

rt_err_t add_dowork_rateCtrl(struct rt_work* work)
{
    rt_err_t ret;

    if (work == RT_NULL) {
        return -RT_EINVAL;
    }

    ret = workqueue_manage_init();
    if (ret != RT_EOK) {
        return ret;
    }

    return rt_workqueue_dowork(rate_ctrl_wq, work);
}

struct rt_workqueue* workqueue_manage_get(void)
{
    return rate_ctrl_wq;
}

