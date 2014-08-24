/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  项目的公共函数和参数定义
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __COMMON__H__
#define __COMMON__H__

#include "types.h"
#include "stdio.h"

#define true                    TRUE
#define false                   FALSE
typedef int8                    int8_t;
typedef uint8                   uint8_t;
typedef int16                   int16_t;
typedef uint16                  uint16_t;
typedef uint32                  uint32_t;

#define LOG_D(format, ...) Printf_High("[Debug]%s: " format "\r\n", __func__, ##__VA_ARGS__)
#define LOG_E(format, ...) Printf_High("[Error]%s: " format "\r\n", __func__, ##__VA_ARGS__)

#define YEELINK_APPKEY_HDR    "U-ApiKey: 91fd94cd65221c40add6d451accfb9b0\r\n"
#define YEELINK_HOSTNAME      "api.yeelink.net"
#define YEELINK_GET_SENSORS   "/v1.0/device/5992/sensors"

#define YEELINK_TEMP_SENSOR   "/v1.0/device/5992/sensor/11571/datapoints"
#define YEELINK_HUMI_SENSOR   "/v1.0/device/5992/sensor/11570/datapoints"
#define YEELINK_AIR_SENSOR    "/v1.0/device/5992/sensor/11572/datapoints"

#endif /* __COMMON__H__ */