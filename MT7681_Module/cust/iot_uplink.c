#include <stdio.h>
#include "string.h"
#include "queue.h"
#include "iot_api.h"
#include "config.h"

/******************************************************************************
* MODULE NAME:     iot_uplink.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:     
* DESIGNER:        
* DATE:            July 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek Tech. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V1.0.0     July 2014    - Initial Version V1.0
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/
UCHAR	SNAP_802_1H[] = {0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00};
UCHAR	SNAP_BRIDGE_TUNNEL[] = {0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8};
UCHAR	ARP_TYPE[] = {0x08, 0x06};
UCHAR	EAPOL[]  = {0x88, 0x8e};
UCHAR	IPv4[]	 = {0x08, 0x00};
UCHAR	IPv6[]	 = {0x86, 0xdd};

extern UCHAR BCAST_ADDR[MAC_ADDR_LEN];
extern u16_t ipid; 
extern STA_ADMIN_CONFIG *pIoTStaCfg;

INT32 IoT_build_app_response_header(	PUCHAR buffer,
											UINT_8 ProtoType, 
											UINT_8 ProtoSubType, 
											UINT_16 DataType, 
											UINT_16 DataLen, 
											IoTPacketInfo* pPacketInfo)
{
	IoTCtrlProtocolHeader * ProtocolHeader_out = (IoTCtrlProtocolHeader *)(buffer);
	DataHeader * DataHeader_out = (DataHeader *)(buffer+CP_HDR_LEN);
	ProtocolHeader_out->Magic = IOT_MAGIC_NUMBER;				 //protocol magic number
	COPY_MAC_ADDR(ProtocolHeader_out->SendMAC, gCurrentAddress); //exchange send & receive address
	COPY_MAC_ADDR(ProtocolHeader_out->ReceiveMAC, pPacketInfo->SendMAC);
	ProtocolHeader_out->SessionID = pPacketInfo->SessionID;		//reserved
	ProtocolHeader_out->Sequence = pPacketInfo->Sequence;		//sequence number
	ProtocolHeader_out->SubHdr.field.Flag = 0;					//reserved
	ProtocolHeader_out->SubHdr.field.Type = ProtoType;			//command type. 0: management command 1: function command
	ProtocolHeader_out->SubHdr.field.SubType = ProtoSubType;	//subtype
	
	DataHeader_out->Type = DataType;
	DataHeader_out->Length = DataLen;
	return 0;
}

unsigned short IoT_check_sum(PUCHAR addr, int nword, PUCHAR answer)
{
	unsigned long sum;
	unsigned short *buf = (unsigned short *)addr;
	PUCHAR psum;
	nword /= 2;
	for(sum=0;nword>0;nword--)
		sum += *buf++;

	sum = (sum>>16) + (sum & 0xffff);
	sum += (sum>>16);

	sum = ~sum;
	
	psum = (PUCHAR)&sum;

	answer[0] = psum[0];
	answer[1] = psum[1];
	
	return sum;

}



PUCHAR IoT_Udppseudo_check_sum(	PUCHAR pseudohdr,
										INT32 pseudo_len,
										PUCHAR udpdata,
										INT32 datalen,
										PUCHAR answer)  
{ 
	UINT16 a,b;
	PUCHAR a1,b1;
	UCHAR checksum1[2],checksum2[2];
	
	IoT_check_sum((PUCHAR)pseudohdr, pseudo_len, checksum1);
	IoT_check_sum((PUCHAR)udpdata, datalen, checksum2);
	
	a = checksum1[0]+checksum2[0];
	
	b = checksum1[1]+checksum2[1];
	
	a1=(PUCHAR)&a;
	b1=(PUCHAR)&b;
	
	answer[0] = a1[0]+b1[1];
	answer[1] = b1[0]+a1[1];
	
	return(answer);
	
} 

void IoT_build_send_udp_packet( PUCHAR pSrcAddr, 
									  PUCHAR pDstAddr,
									  UINT16 SrcPort,
									  UINT16 DstPort,
									  PUCHAR pPayload, 
									  UINT16  PayloadLen)
{
	UCHAR				Header802_3[14];
	UCHAR				*mpool;
	pBD_t				pBufDesc;
	UCHAR				checksum[2];
	struct udppseudohdr	Udppseudohdr;
	struct ipv4hdr		*pIpv4Hdr;
	struct udphdr		*pUdpHdr;
	struct udppseudohdr	*pUdppseudohdr = &Udppseudohdr;
	UCHAR *pIotpkt;
	
	//	struct  t_IoTCtrlProtocolHeader * ProtocolHeader_out = (struct t_IoTCtrlProtocolHeader *)(pPayload);
	//	DBGPRINT(RT_DEBUG_TRACE,("1=:%02x:%02x:%02x:%02x:%02x:%02x \n",PRINT_MAC(ProtocolHeader_out->SendMAC)));
	//	DBGPRINT(RT_DEBUG_TRACE,("2=:%02x:%02x:%02x:%02x:%02x:%02x \n",PRINT_MAC(ProtocolHeader_out->ReceiveMAC)));
	
	
	//handle_FCE_TxTS_interrupt();
	pBufDesc = apiQU_Dequeue(&gFreeQueue1);
	if(pBufDesc ==NULL)
	{
		DBGPRINT(RT_DEBUG_ERROR,("=>%s DeQ fail\n",__FUNCTION__));
		return;
	}	
	mpool = pBufDesc->pBuf;
	
	/* ether header :14 */	
	/* IPv6 header  :40 */
	/* ICMPv6 header:24 */
	memset(mpool, 0, PKTQU1_SINGLE_BUFFER_SIZE-36); //1500);  //cannot > PKTQU1_SINGLE_BUFFER_SIZE-36
	pIpv4Hdr = (struct ipv4hdr*)(mpool + LENGTH_802_3);
	pUdpHdr   = (struct udphdr*)(mpool  + LENGTH_802_3 + IPV4_HEADER_SIZE);
	pIotpkt = (UCHAR *)(mpool  + LENGTH_802_3 + IPV4_HEADER_SIZE + UDP_HEADER_LENGTH);
	
	/* IPV6 header */
	/* copy from structure to avoid unalignment 4bytes reference to pIpv6Hdr->flow_lbl */
	memset(pIpv4Hdr, 0, sizeof(struct ipv4hdr));
	pIpv4Hdr->version_ihl = 0x45;
	pIpv4Hdr->tos = 0;
	pIpv4Hdr->tot_len = SWAP16(IPV4_HEADER_SIZE + UDP_HEADER_LENGTH + PayloadLen);
	
	pIpv4Hdr->id = 0;
	pIpv4Hdr->frag_off = 0x0040;
	pIpv4Hdr->ttl = 64;
	pIpv4Hdr->protocol = 0x11;
	pIpv4Hdr->check[0] = 0;
	pIpv4Hdr->check[1] = 0;
	memcpy((char*) (&pIpv4Hdr->saddr), pSrcAddr, 4);
	memcpy((char*) (&pIpv4Hdr->daddr), pDstAddr, 4);
	IoT_check_sum((PUCHAR)pIpv4Hdr, IPV4_HEADER_SIZE, checksum);
	pIpv4Hdr->check[0] = checksum[0];
	pIpv4Hdr->check[1] = checksum[1];
	
	/* ICMPv6 Neighbor advertisement */
	pUdpHdr->source_port  = SWAP16(SrcPort);
	pUdpHdr->dest_port = SWAP16(DstPort);
	pUdpHdr->len = SWAP16(UDP_HEADER_LENGTH+PayloadLen);
	pUdpHdr->check[0] = 0;
	pUdpHdr->check[1] = 0;
	
	memcpy((char*) (&pUdppseudohdr->saddr), pSrcAddr, 4);
	memcpy((char*) (&pUdppseudohdr->daddr), pDstAddr, 4);
	pUdppseudohdr->ttl = 0;
	pUdppseudohdr->protocol = pIpv4Hdr->protocol;
	pUdppseudohdr->udpdatalen = pUdpHdr->len;
	pUdppseudohdr->source_port = pUdpHdr->source_port;
	pUdppseudohdr->dest_port = pUdpHdr->dest_port;
	pUdppseudohdr->len = pUdpHdr->len;
	pUdppseudohdr->check[0] =0;
	pUdppseudohdr->check[1] =0;
	
	
	//PUCHAR a=(PUCHAR)(&Udppseudohdr.saddr);
	//printf("pUdppseudohdr->source_port:%x\n",pUdppseudohdr->source_port);
	IoT_Udppseudo_check_sum((PUCHAR)pUdppseudohdr,sizeof(struct udppseudohdr),pPayload, PayloadLen,checksum);
	
	pUdpHdr->check[0] = checksum[0];
	pUdpHdr->check[1] = checksum[1];
	//printf("Len:%d",PayloadLen);
	//	memcpy(pIotpkt, pPayload, PayloadLen);
	memcpy(pIotpkt, pPayload, PayloadLen);
	
	/* init header and Fill Packet and back to sender */
	MAKE_802_3_HEADER(Header802_3, BCAST_ADDR, pIoTStaCfg->MyMacAddr, IPv4);
	
	/* copy 802.3 header */
	memcpy(mpool, Header802_3, sizeof(Header802_3));
	
	/* STA Tx */	
	STA_Legacy_Frame_Tx(pBufDesc,
						sizeof(Header802_3) +IPV4_HEADER_SIZE + UDP_HEADER_LENGTH +PayloadLen, 
						pIoTStaCfg->bClearFrame);
	//STA_Legacy_Frame_Tx(pBufDesc, ARP_MSG_SIZE +sizeof(Header802_3), FALSE);
	return;
	
}



INT32 IoT_send_udp_directly(uip_ipaddr_t *DstAddr,
								PUCHAR DstMAC,
								UINT16 SrcPort,
								UINT16 DstPort,
								PUCHAR pPayload, 
								UINT16  PayloadLen)
{
	UCHAR				Header802_3[14];
	UCHAR				*mpool;
	pBD_t				pBufDesc;
	UCHAR				checksum[2];
	struct udppseudohdr	Udppseudohdr;
	struct ipv4hdr		*pIpv4Hdr;
	struct udphdr		*pUdpHdr;
	struct udppseudohdr	*pUdppseudohdr = &Udppseudohdr;
	UCHAR *pIotpkt;
	UCHAR sAddr[4],dAddr[4];

	if(!DstAddr || !DstMAC || !pPayload)
	  return 1;
	
	printf("%d.%d.%d.%d,SrcPort:%d,DstPort:%d,PayloadLen:%d\n", 
			uip_ipaddr1(DstAddr),uip_ipaddr2(DstAddr),
			uip_ipaddr3(DstAddr),uip_ipaddr4(DstAddr),SrcPort,DstPort,PayloadLen);

	pBufDesc = apiQU_Dequeue(&gFreeQueue1);
	if(pBufDesc ==NULL)
	{
		Printf_High("=>%s DeQ fail\n",__FUNCTION__);
		return 2;
	}	
	mpool = pBufDesc->pBuf;
	
	memset(mpool, 0, PKTQU1_SINGLE_BUFFER_SIZE-36); //1500);  //cannot > PKTQU1_SINGLE_BUFFER_SIZE-36
	pIpv4Hdr = (struct ipv4hdr*)(mpool + LENGTH_802_3);
	pUdpHdr  = (struct udphdr*)(mpool  + LENGTH_802_3 + IPV4_HEADER_SIZE);
	pIotpkt = (UCHAR *)(mpool  + LENGTH_802_3 + IPV4_HEADER_SIZE + UDP_HEADER_LENGTH);
	
	memset(pIpv4Hdr, 0, sizeof(struct ipv4hdr));
	pIpv4Hdr->version_ihl = 0x45;
	pIpv4Hdr->tos = 0;
	pIpv4Hdr->tot_len = SWAP16(IPV4_HEADER_SIZE + UDP_HEADER_LENGTH + PayloadLen);

	++ipid;
	pIpv4Hdr->id = SWAP16(ipid);
	pIpv4Hdr->frag_off = 0;
	pIpv4Hdr->ttl = 64;
	pIpv4Hdr->protocol = 0x11;
	pIpv4Hdr->check[0] = 0;
	pIpv4Hdr->check[1] = 0;

	sAddr[0] = uip_ipaddr1(uip_hostaddr);
	sAddr[1] = uip_ipaddr2(uip_hostaddr);
	sAddr[2] = uip_ipaddr3(uip_hostaddr);
	sAddr[3] = uip_ipaddr4(uip_hostaddr);
	memcpy((char*)(&pIpv4Hdr->saddr), sAddr, 4);

	dAddr[0] = uip_ipaddr1(DstAddr);
	dAddr[1] = uip_ipaddr2(DstAddr);
	dAddr[2] = uip_ipaddr3(DstAddr);
	dAddr[3] = uip_ipaddr4(DstAddr);
	memcpy((char*)(&pIpv4Hdr->daddr), dAddr, 4);

	IoT_check_sum((PUCHAR)pIpv4Hdr, IPV4_HEADER_SIZE, checksum);
	pIpv4Hdr->check[0] = checksum[0];
	pIpv4Hdr->check[1] = checksum[1];
	
	pUdpHdr->source_port  = SWAP16(SrcPort);
	pUdpHdr->dest_port = SWAP16(DstPort);
	pUdpHdr->len = SWAP16(UDP_HEADER_LENGTH+PayloadLen);
	pUdpHdr->check[0] = 0;
	pUdpHdr->check[1] = 0;
	
	memcpy((char*)(&pUdppseudohdr->saddr), sAddr, 4);
	memcpy((char*)(&pUdppseudohdr->daddr), dAddr, 4);
	pUdppseudohdr->ttl = 0;
	pUdppseudohdr->protocol = pIpv4Hdr->protocol;
	pUdppseudohdr->udpdatalen = pUdpHdr->len;
	pUdppseudohdr->source_port = pUdpHdr->source_port;
	pUdppseudohdr->dest_port = pUdpHdr->dest_port;
	pUdppseudohdr->len = pUdpHdr->len;
	pUdppseudohdr->check[0] =0;
	pUdppseudohdr->check[1] =0;
	
	IoT_Udppseudo_check_sum((PUCHAR)pUdppseudohdr,sizeof(struct udppseudohdr),pPayload, PayloadLen,checksum);
	
	pUdpHdr->check[0] = checksum[0];
	pUdpHdr->check[1] = checksum[1];
	memcpy(pIotpkt, pPayload, PayloadLen);
	
	/* init header and Fill Packet and back to sender */
	MAKE_802_3_HEADER(Header802_3, DstMAC, pIoTStaCfg->MyMacAddr, IPv4);
	
	/* copy 802.3 header */
	memcpy(mpool, Header802_3, sizeof(Header802_3));

	/* STA Tx */	
	STA_Legacy_Frame_Tx(pBufDesc,
						sizeof(Header802_3) +IPV4_HEADER_SIZE + UDP_HEADER_LENGTH +PayloadLen, 
						pIoTStaCfg->bClearFrame);
	
	return 0;
}


