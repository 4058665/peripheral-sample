/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-25     misonyo      first edition.
 */
/*
 * 程序清单：这是一个独立看门狗设备使用例程
 * 例程导出了 iwdg_sample 命令到控制终端
 * 命令调用格式：iwdg_sample iwg
 * 命令解释：命令第二个参数是要使用的看门狗设备名称，为空则使用例程默认的看门狗设备
 * 程序功能：程序通过设备名称查找看门狗设备，然后初始化设备并设置看门狗设备溢出时间。
 *           然后设置空闲线程回调函数，在回调函数里会喂狗。
*/

#include <rtthread.h>
#include <rtdevice.h>

#define IWDG_DEVICE_NAME    "iwg"

static rt_device_t wdg_dev;

static void idle_hook(void)
{
    /* 在空闲线程的回调函数里喂狗 */
    rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_KEEPALIVE, NULL);
}

static int iwdg_sample(int argc, char *argv[])
{
    rt_err_t result = RT_EOK;
    rt_uint32_t timeout = 1000;    /* 超时时间为1000ms*/
    char device_name[RT_NAME_MAX];

    /* 判断命令行参数是否给定了设备名称 */
    if (argc == 2)
    {
        rt_strncpy(device_name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(device_name, IWDG_DEVICE_NAME, RT_NAME_MAX);
    }
    /* 根据设备名称查找看门狗设备，获取设备句柄 */
    wdg_dev = rt_device_find(device_name);
    if (!wdg_dev)
    {
        rt_kprintf("find %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 初始化设备 */
    result = rt_device_init(wdg_dev);
    if (result != RT_EOK)
    {
        rt_kprintf("initialize %s failed!\n", device_name);
        return RT_ERROR;
    }
    /* 设置看门狗溢出时间 */
    result = rt_device_control(wdg_dev, RT_DEVICE_CTRL_WDT_SET_TIMEOUT, (void *)timeout);
    if (result != RT_EOK)
    {
        rt_kprintf("set %s timeout failed!\n", device_name);
        return RT_ERROR;
    }
    /* 设置空闲线程回调函数 */
    rt_thread_idle_sethook(idle_hook);

    return result;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(iwdg_sample, iwdg sample);
