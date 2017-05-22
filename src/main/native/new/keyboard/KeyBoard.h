#ifndef __KEY_BOARD_H__
#define __KEY_BOARD_H__

typedef struct {
  char prompt;        // prompt after receiving data
  int baudrate;       // baudrate default is 9600bps
  char databit;       // data bits, 5,6,7,8
  char debug;         // debug mode, 0:none, 1:debug
  char echo;          // echo mode, 0:none, 1:echo
  char fctl;          // flow control, 0:none, 1:hardware, 2:software
  char tty;           // tty: 0,1,2,3,4,5,6,7
  char parity;        // parity mode, 0:none, 1:odd, 2:even
  char stopbit;       // stop bits, 1,2
  const int reserved; // reserved, must be zero
} portinfo_t;

typedef portinfo_t* pportinfo_t;

int PortOpen(pportinfo_t pportinfo);
void PortClose(int fdcom);

int PortSend(int fdcom, unsigned char* data, int datalen);
int PortRecv(int fdcom, unsigned char* data, int datalen, struct timeval timeout);

int PortSet(int fdcom, const pportinfo_t pportinfo);
int KbdHandShake(int fdcom);
void UartOversee(int fdcom, void* keyValue);

#endif
