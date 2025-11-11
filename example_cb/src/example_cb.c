#include "example_cb.h"

#include <rtthread.h>
#include <finsh.h>
#include <string.h>
#include <uMCN.h>
#include <ipc/workqueue.h>

#define LOG_TAG "example_cb"
#define LOG_LVL LOG_LVL_INFO
#include <ulog.h>

#define EXAMPLE_CB_PUBLISHER_STACK_SIZE    1024
#define EXAMPLE_CB_WORKQUEUE_STACK_SIZE    1024
#define EXAMPLE_CB_THREAD_PRIORITY         (RT_THREAD_PRIORITY_MAX / 3)
#define EXAMPLE_CB_THREAD_TIMESLICE        10

typedef struct {
    rt_uint32_t seq;
    char payload[16];
} example_primary_msg_t;

struct example_subscriber_ctx {
    example_primary_msg_t data;
};

static rt_thread_t example_publisher_thread = RT_NULL;
static struct rt_workqueue* example_workqueue = RT_NULL;
static struct rt_work example_subscriber_work;

static McnNode_t example_subscriber_node = RT_NULL;

static struct example_subscriber_ctx example_subscriber_context;

MCN_DEFINE(example_primary_topic, sizeof(example_primary_msg_t));

static void example_publisher_entry(void* parameter);
static void example_subscriber_job(void* parameter);
static void example_subscriber_work_handler(struct rt_work* work, void* parameter);
static void example_subscriber_async_cb(const void* data, void* user_data);

static void example_publisher_entry(void* parameter)
{
    (void)parameter;

    example_primary_msg_t msg;
    rt_memset(&msg, 0, sizeof(msg));
    rt_strncpy(msg.payload, "uMCN-callback", sizeof(msg.payload) - 1);

    while (1) {
        msg.seq++;

        if (mcn_publish(MCN_HUB(example_primary_topic), &msg) != RT_EOK) {
            LOG_E("publish primary topic failed");
        } else {
            LOG_I("published primary topic seq %u payload %s", msg.seq, msg.payload);
        }
        rt_thread_mdelay(3000);
    }
}

static void example_subscriber_job(void* parameter)
{
    struct example_subscriber_ctx* ctx = (struct example_subscriber_ctx*)parameter;

    if (ctx == RT_NULL) {
        return;
    }

    LOG_I("subscriber handle seq %u payload %s",
        ctx->data.seq, ctx->data.payload);
}

static void example_subscriber_work_handler(struct rt_work* work, void* parameter)
{
    (void)work;
    example_subscriber_job(parameter);
}

static void example_subscriber_async_cb(const void* data, void* user_data)
{
    struct example_subscriber_ctx* ctx = (struct example_subscriber_ctx*)user_data;

    rt_memcpy(&ctx->data, data, sizeof(ctx->data));

    if (rt_workqueue_dowork(example_workqueue, &example_subscriber_work) != RT_EOK) {
        LOG_E("submit work failed");
    }
}

int example_cb_init(void)
{
    rt_err_t result;

    rt_memset(&example_subscriber_context, 0, sizeof(example_subscriber_context));

    result = mcn_advertise(MCN_HUB(example_primary_topic), RT_NULL);
    if (result != RT_EOK) {
        LOG_E("advertise primary topic failed (%d)", result);
        return result;
    }

    example_workqueue = rt_workqueue_create("cb_wq",
        EXAMPLE_CB_WORKQUEUE_STACK_SIZE,
        EXAMPLE_CB_THREAD_PRIORITY);
    if (example_workqueue == RT_NULL) {
        LOG_E("create workqueue failed");
        return -RT_ENOMEM;
    }

    rt_work_init(&example_subscriber_work, example_subscriber_work_handler, &example_subscriber_context);

    example_publisher_thread = rt_thread_create("cb_pub",
        example_publisher_entry,
        RT_NULL,
        EXAMPLE_CB_PUBLISHER_STACK_SIZE,
        EXAMPLE_CB_THREAD_PRIORITY,
        EXAMPLE_CB_THREAD_TIMESLICE);
    if (example_publisher_thread == RT_NULL) {
        LOG_E("create publisher thread failed");
        return -RT_ENOMEM;
    }
    rt_thread_startup(example_publisher_thread);

    example_subscriber_node = mcn_subscribe(MCN_HUB(example_primary_topic), RT_NULL, RT_NULL);
    if (example_subscriber_node == RT_NULL) {
        LOG_E("subscribe primary topic failed");
        return -RT_ERROR;
    }

    result = mcn_register_async_cb(example_subscriber_node, example_subscriber_async_cb, &example_subscriber_context);
    if (result != RT_EOK) {
        LOG_E("register subscriber callback failed (%d)", result);
        return result;
    }

    LOG_I("initialized");
    return RT_EOK;
}
MSH_CMD_EXPORT_ALIAS(example_cb_init, example_cb_init, start example cb demo);

