#include "ch.h"
#include "hal.h"
#include <serial.h>

// ��� ������ lwip � chibios
#include <lwipthread.h>

// � ��� ��� ��� lwip. ��� ����� ����� ����, �� � ���� ��� �������
#include <lwip/netif.h>
#include <lwip/api.h>


void write_data(struct netconn *conn){
    // ��� ������ ������ ���������� ��������� �� ����� � ���������� ����. ��������� �������� - ���� ��� ������. �� � ���� ��� ��� ������ �� �������
    netconn_write(conn, "hi there!", 9, NETCONN_NOCOPY);
}

void read_data(struct netconn *conn){
    // ��� ������ ������� �������
    struct netbuf *inbuf;
    uint8_t *buf;
    uint16_t buflen;

    // ���� ��� ��� �������. ������� �����������. � inbuf ����� ���������� ��� � ��� �������
    netconn_recv(conn, &inbuf);
    // ������ �� ���� �� ��������, ��� ������ lwip. ������ ����������� ��������� ���, ����� ��� ������ ��������� �� ������ ������. ������ ������ ���������� ����
    netbuf_data(inbuf, (void **)&buf, &buflen);
    // ������� �� ����� ���������� ������. � ����������� tcp ���������� � ��������� �����, ��� ��� �������� ������������ �� ���� ����
    sdWrite(&SD3, buf, buflen);
    // ������� ������. ���� ����� �� ������ ��� ����� ������ ����������
    netbuf_delete(inbuf);
}


THD_WORKING_AREA(wa_tcp_server, 1024);
THD_FUNCTION(tcp_server, p) {

// ����� ������ ��������
  (void)p;

// ��������� ����������. conn - ���������� ����������, ����� ������ ������� ����
// newconn - ������� ���������� � ��� ��� ����������
  struct netconn *conn, *newconn;
  err_t err;

// ��������� ���������� � ������ TCP
  conn = netconn_new(NETCONN_TCP);

// ��������� ��������� ��������� ���� �����, ������� ��������� ��� ������. �� ������ ���� ���� �� ��� � � �����
  struct ip4_addr ip;
  IP4_ADDR(&ip, 192, 168, 1, 123);
// ������������ � 80 �����
  netconn_bind(conn, &ip, 80);

// � �������� ��� �������
  netconn_listen(conn);

  while (true) {
    palToggleLine(LINE_LED3);

    // ���� ��� ���-�� �����������. ������� �����������
    err = netconn_accept(conn, &newconn);
    // ���� ���-�� �� ��� - ������ �� ������
    if (err != ERR_OK)
      continue;
    // ��������� ��� ���-�� �����������
    // ������� ���-������
    write_data(newconn);
    // ��������� ���-������
    read_data(newconn);
    // �� ��������� ������ ��������� ���������� � ������� �����������
    netconn_close(newconn);
    netconn_delete(newconn);
    // ����� ����� ������ ����� �����������
  }

}



int main(void) {

    halInit();
    chSysInit();
    debug_stream_init();
    dbgprintf("start\r\n");


// ������ ����� �����
    lwipthread_opts_t opts;
    struct ip4_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 192, 168, 1, 123);
    IP4_ADDR(&gateway, 192, 168, 1, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
// ��������� ����� ����� ���� ��������� �����. ������� ����� � ���� �� ���� ����������
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;

// ��������� ������� �������. � ����� ������� �� ������� �������� ��������� � ��� ����� �����������, ���� ������ ����� ����� � ��� �� ����
// ����� �� ���������� ���������, ����������� NULL � �������� ���������. ����� ����� ������������ ��������� �� ��������� �� ����� lwipthread.h
    lwipInit(&opts);
    chThdSleepSeconds(2);

// �������� ����� �������
    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, NULL);

// �������� ���������, ����� �������� ��� �� �������
    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}
