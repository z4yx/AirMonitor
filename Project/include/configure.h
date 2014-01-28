#ifndef __CONFIG_H___
#define	__CONFIG_H___

#define ETH_MAC_ADDR {0x54, 0x55, 0x58, 0x10, 0x00, 0x22}
#define ETH_IP_ADDR  {192,168,1,8}
#define ETH_NETMASK  {255,255,255,0}
#define ETH_GW_ADDR  {192,168,1,1}

#define ETH_DNS_ADDR {8,8,8,8}

#define YEELINK_APPKEY_HDR    "U-ApiKey: 91fd94cd65221c40add6d451accfb9b0\r\n"
#define YEELINK_HOSTNAME      "api.yeelink.net"
#define YEELINK_GET_SENSORS   "/v1.0/device/5992/sensors"

#endif
