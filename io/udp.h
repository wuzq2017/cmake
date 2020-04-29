#ifndef __UDP_H
#define __UDP_H



extern int udp_init(int local_port, uint16(*recv_callback)(uint8 *, uint16, uint8 *));
extern void udp_send(int client_index, char * remote_ip, int remote_port, uint8 *buf, uint16 length);



#endif
