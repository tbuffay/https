/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*                          (c) Copyright 2009-2015; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          SETUP INSTRUCTIONS
*
*   This demonstration project illustrate a basic HTTP client demo project.
*
*   By default some configuration steps are required to compile this example :
*
*   1. Include the require Micrium software components
*       In the BSP setting dialog in the "overview" section of the left pane the following libraries
*       should be added to the BSP :
*
*           ucos_common
*           ucos_osiii
*           ucos_tcpip
*           ucos_dhcp-c
*           ucos_dns-c
*           ucos_http-c
*           ucos_standalone
*
*   2. Kernel tick source - (Not required on the Zynq-7000 PS)
*       If a suitable timer is available in your FPGA design it can be used as the kernel tick source.
*       To do so, in the "ucos" section select a timer for the "kernel_tick_src" configuration option.
*
*   3. STDOUT configuration
*       Output from the print() and UCOS_Print() functions can be redirected to a supported UART. In
*       the "ucos" section the stdout configuration will list the available UARTs.
*
*   4. Network interface configuration
*       In the BSP network interface section selects the desired network intergace and ip configuration.
*       Leave dhcp enabled if you'd like the embbeded dhcp client to configure the interface.
*
*   Troubleshooting :
*       By default the Xilinx SDK may not have selected the Micrium drivers for the timer and UART.
*       If that is the case they must be manually selected in the drivers configuration section.
*
*       Finally make sure the FPGA is programmed before debugging.
*
*
*   Remember that this example is provided for evaluation purposes only. Commercial development requires
*   a valid license from Micrium.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <Source/os.h>
#include  <ucos_bsp.h>
#include  <Source/net_type.h>
#include  <Source/net_sock.h>
#include  <Source/net_ip.h>
#include  "IF/net_if.h"
#include  <ucos_bsp.h>
#include  <Source/net_util.h>
#include  <string.h>
#include <sys/stat.h>
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  MainTask (void *p_arg);

#define  UDP_SERVER_IP_ADDR   "192.168.19.24"
#define  UDP_SERVER_PORT               2368
#define  UDP_SERVER_TX_STR     "ello World,taojieHello World,taojieHello World,taojie"

void  HTTPs_OS_TxDly(void)
    	{
    	        OS_ERR  os_err;
    	        OSTimeDly(100 * OS_CFG_TICK_RATE_HZ / 1000, OS_OPT_TIME_HMSM_STRICT, &os_err);
    	}
CPU_BOOLEAN  TestUDPClient (void)
{
    NET_SOCK_ID         sock;
    NET_IPv4_ADDR         server_ip_addr;
    NET_SOCK_ADDR_IPv4  server_sock_addr_ip;
    NET_SOCK_ADDR_LEN   server_sock_addr_ip_size;
    CPU_CHAR           *pbuf;
    CPU_INT16S          buf_len;
    NET_SOCK_RTN_CODE   tx_size;
    NET_ERR            err;

    pbuf    = UDP_SERVER_TX_STR;
    buf_len = Str_Len(UDP_SERVER_TX_STR);

    sock = NetSock_Open( NET_SOCK_ADDR_FAMILY_IP_V4,
                         NET_SOCK_TYPE_DATAGRAM,
                         NET_SOCK_PROTOCOL_UDP,
                        &err);
    if (err != NET_SOCK_ERR_NONE) {
    	UCOS_Print ("NetSock_Open err\r\n");
        return (DEF_FAIL);
    }

    //server_ip_addr = NetASCII_Str_to_IPv4(UDP_SERVER_IP_ADDR, &err);
    server_ip_addr = 0xffffffff;
    //if (err != NET_ASCII_ERR_NONE) {
    //	UCOS_Print ("NetASCII_Str_to_IPv4 err\r\n");
    //    NetSock_Close(sock, &err);
    //    return (DEF_FAIL);
    //}

    server_sock_addr_ip_size = sizeof(server_sock_addr_ip);
    Mem_Clr(&server_sock_addr_ip,
             server_sock_addr_ip_size);
    server_sock_addr_ip.AddrFamily = NET_SOCK_ADDR_FAMILY_IP_V4;
    server_sock_addr_ip.Addr       = NET_UTIL_HOST_TO_NET_32(server_ip_addr);
    server_sock_addr_ip.Port       = NET_UTIL_HOST_TO_NET_16(UDP_SERVER_PORT);

    while(1)
    {
    	HTTPs_OS_TxDly();
    tx_size = NetSock_TxDataTo(                  sock,
                                                 pbuf,
                                                 buf_len,
                                                 NET_SOCK_FLAG_NONE,
                               (NET_SOCK_ADDR *)&server_sock_addr_ip,
                                                 sizeof(server_sock_addr_ip),
                                                &err);

    if (err != NET_SOCK_ERR_NONE) {
    	UCOS_Printf ("NetSock_TxDataTo err = %d\r\n",err);
    	NetSock_Close(sock, &err);
            return (DEF_FAIL);
        }
    }
    NetSock_Close(sock, &err);
    if (err != NET_SOCK_ERR_NONE) {
        return (DEF_FAIL);
    }
    return (DEF_TRUE);
}

int main()
{

    UCOSStartup(MainTask);

    return 0;
}


/*
*********************************************************************************************************
*                                             MainTask()
*
* Description : Startup task example code.
*
* Returns     : none.
*
* Created by  : main().
*********************************************************************************************************
*/

void  MainTask (void *p_arg)
{
    OS_ERR              os_err;

    UCOS_Print ("Application start\r\n");
    TestUDPClient();
    while (DEF_TRUE) {

    	//HTTPs_Task(p_arg);
        OSTimeDlyHMSM(0, 0, 10, 0, OS_OPT_TIME_HMSM_STRICT, &os_err);
        UCOS_Print("Periodic output every 10 seconds from the main task\r\n");
    }

}

