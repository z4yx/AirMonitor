#ifndef __CONFIG_H___
#define	__CONFIG_H___

#define NTP_SERVER "17.82.253.7" /* time.asia.apple.com */

#define YEELINK_APPKEY_HDR    "U-ApiKey: 91fd94cd65221c40add6d451accfb9b0\r\n"
#define YEELINK_HOSTNAME      "api.yeelink.net"
#define YEELINK_GET_SENSORS   "/v1.0/device/5992/sensors"

#define YEELINK_TEMP_SENSOR   "/v1.0/device/5992/sensor/11571/datapoints"
#define YEELINK_HUMI_SENSOR   "/v1.0/device/5992/sensor/11570/datapoints"
#define YEELINK_AIR_SENSOR    "/v1.0/device/5992/sensor/11572/datapoints"

#define REPORT_INTERVAL  300000

#define DATE_SYNC_TIMEOUT 30000
#define REPORT_TIMEOUT    15000

#endif
