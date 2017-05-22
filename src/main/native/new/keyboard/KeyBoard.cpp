#include "keyboard/KeyBoard.h"

#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <termios.h>

#include "Def.h"
#include "keyboard/KeyValueOpr.h"

extern bool g_authorizationOn;
extern GIOChannel* keyboard_channel;

guint input_handle = 0;

// ---------------------------------------------------------

static char* GetPtty(const pportinfo_t pportinfo) {
  static char ptty[15] = {0};

  switch(pportinfo->tty) {
  case '0':
    sprintf(ptty, "/dev/ttyS0");
    break;
  case '1':
    sprintf(ptty, "/dev/ttyS1");
    break;
  case '2':
    sprintf(ptty, "/dev/ttyS2");
    break;
  }

  PRINTF ("tty case:%d", pportinfo->tty);
  PRINTF("ptty=%s\n", ptty);

  return ptty;
}

static int convbaud(unsigned int baudrate) {
  switch(baudrate) {
  case 2400:
    return B2400;
  case 4800:
    return B4800;
  case 9600:
    return B9600;
  case 19200:
    return B19200;
  case 38400:
    return B38400;
  case 57600:
    return B57600;
  case 115200:
    return B115200;
  default:
    return B9600;
  }
}

int PortOpen(pportinfo_t pportinfo) {
  char* ptty = GetPtty(pportinfo);
  int fdcom = open(ptty, O_RDWR | O_NOCTTY | O_NDELAY);

  if (fdcom < 0) {
    perror("open_uart error");
    return -1;
  }

  fcntl(fdcom, F_SETFL, 0);

  return fdcom;
}

void PortClose(int fdcom) {
  close(fdcom);
}

int PortSend(int fdcom, unsigned char* data, int datalen) {
  fd_set fs_write;

  FD_ZERO(&fs_write);
  FD_SET(fdcom, &fs_write);

  errno = 0;
  int fs_sel = select(fdcom+1, NULL, &fs_write, NULL, NULL);

  if (fs_sel) {
    int len = write(fdcom, data, datalen);

    if(len == datalen) {
      PRINTF("Send char 0x%x to Comm\n", *data);
      return len;
    } else {
      perror("PortSend write error");
      tcflush(fdcom, TCOFLUSH);
      return -1;
    }
  } else if(fs_sel<0) {
    perror("PortSend select error");
    return -1;
  }

  return 0;
}

int PortRecv(int fdcom, unsigned char* data, int datalen, struct timeval timeout) {
  fd_set fs_read;

  FD_ZERO(&fs_read);
  FD_SET(fdcom, &fs_read);

  errno = 0;
  int fs_sel = select(fdcom+1, &fs_read, NULL, NULL, &timeout);
  PRINTF("fs_sel=%d\n", fs_sel);
  if (fs_sel>0) {
    int len = read(fdcom, data, datalen);
    if(len<0) {
      perror("PortRecv read error");
    }

    return len;
  } else if(fs_sel<0) {
    perror("PortRecv select error");
    return -1;
  }
}

int PortSet(int fdcom, const pportinfo_t pportinfo) {
  struct termios termios_old;
  struct termios termios_new;

  bzero(&termios_old, sizeof(termios_old));
  bzero(&termios_new, sizeof(termios_new));
  cfmakeraw(&termios_new);
  tcgetattr(fdcom, &termios_old);

  int baudrate = convbaud(pportinfo->baudrate);
  cfsetispeed(&termios_new, baudrate);
  cfsetospeed(&termios_new, baudrate);
  termios_new.c_cflag |= CLOCAL;
  termios_new.c_cflag |= CREAD;

  char fctl = pportinfo->fctl;

  switch(fctl) {
  case '0':
    termios_new.c_cflag &= ~CRTSCTS;
    break;
  case '1':
    termios_new.c_cflag |= CRTSCTS;
    break;
  case '2':
    termios_new.c_cflag |= IXON | IXOFF | IXANY;
    break;
  }

  termios_new.c_cflag &= ~CSIZE;
  char databit = pportinfo->databit;

  switch(databit) {
  case '5':
    termios_new.c_cflag |= CS5;
    break;
  case '6':
    termios_new.c_cflag |= CS6;
    break;
  case '7':
    termios_new.c_cflag |= CS7;
    break;
  default:
    termios_new.c_cflag |= CS8;
    break;
  }

  char parity = pportinfo->parity;

  switch(parity) {
  case '0':
    termios_new.c_cflag &= ~PARENB;
    break;
  case '1':
    termios_new.c_cflag |= PARENB;
    termios_new.c_cflag &= ~PARODD;
    break;
  case '2':
    termios_new.c_cflag |= PARENB;
    termios_new.c_cflag |= PARODD;
    break;
  }

  char stopbit = pportinfo->stopbit;

  if(stopbit == '2') {
    termios_new.c_cflag |= CSTOPB;		// 2 stop bits
  } else {
    termios_new.c_cflag &= ~CSTOPB;   // 1 stop bits
  }

  termios_new.c_oflag &= ~OPOST;
  termios_new.c_cc[VMIN] = 1;
  termios_new.c_cc[VTIME] = 1;

  tcflush(fdcom, TCIFLUSH);
  char tmp = tcsetattr(fdcom, TCSANOW, &termios_new);
  tcgetattr(fdcom, &termios_old);
  return tmp;
}

// 键盘初始化握手
int KbdHandShake(int fdcom) {
  unsigned char init_comm = 0x4b; //0x4b = 'K'
  int sendlen = PortSend(fdcom, &init_comm, 1);

  struct timeval timeout;
  timeout.tv_sec = 0;             // timeout = 5 second.
  timeout.tv_usec = 2000;

  unsigned char recvbuf[10] = {0};
  for(int i=0; i<10; i++) {
    recvbuf[i] = 0;
  }

  int recvlen = 0;
  int count = 2;
  int times = 0;

  while (count) {
    if (times == 10) {
      break;
    }

    recvlen = PortRecv(fdcom, recvbuf, count, timeout);
    count -= recvlen;
    times++;
  }

  if (times == 10 && count == 2) {
    syslog(LOG_INFO, "Wait keyboard self-test timeout!\n");
    printf( "Wait keyboard self-test timeout!\n" );
    return 0;
  }

  if (times == 10 && count > 0) {
    syslog(LOG_INFO, "Keyboard lost data!\n");
    perror( "Keyboard lost data!\n" );
    return 0;
  }

  PRINTF("Keyboard self-test recv=0x%2x%2x\n", recvbuf[0], recvbuf[1]);
  if((recvbuf[0] == 0x00) && (recvbuf[1] == 0xc1)) {
    syslog(LOG_ERR, "Keyboard self-test: mouse error!\n");
    perror("Keyboard self-test: mouse error!");
  }

  if((recvbuf[0] == 0x00) && (recvbuf[1] == 0xc2)) {
    syslog(LOG_ERR, "Keyboard self-test: key & mouse error!\n");
    perror("Keyboard self-test: key & mouse error!");
  }

  if((recvbuf[0] == 0x00) && (recvbuf[1] == 0xc0)) {
    syslog(LOG_ERR, "Keyboard self-test: key & mouse work normally.\n");
    perror("Keyboard self-test: key & mouse work normally.\n");
  }

  init_comm = 0x56;
  sendlen = PortSend(fdcom, &init_comm, 1);

  count = 1;
  timeout.tv_sec = 0;
  timeout.tv_usec = 20000;
  recvlen = PortRecv(fdcom, recvbuf, count, timeout);

  if (recvlen <= 0) {
    syslog(LOG_INFO, "Wait keyboard encrypt information timeout!\n");
    perror( "Wait keyboard encrypt information timeout!\n" );
    return 0;
  }

  if (recvbuf[0] == 0x57) {
    g_authorizationOn = true;
    syslog(LOG_INFO, "Authorization on.\n");
    perror("Authorization on.\n");
  } else {
    g_authorizationOn = false;
    syslog(LOG_INFO, "Authorization off.\n");
    perror("Authorization off.\n");
  }

  return 0;
}

// 键盘串口监视
//
// fdcom:	串口设备号
// pKeyInterface:	KeyValueOpr对象指针
void UartOversee(int fdcom, void* pKeyInterface) {
  if (keyboard_channel) {
    return;
  }

  keyboard_channel = g_io_channel_unix_new(fdcom);
  input_handle = g_io_add_watch_full(keyboard_channel, G_PRIORITY_DEFAULT, G_IO_IN, GetKeyValue, pKeyInterface, 0);
}
