#include "keyboard/KeyValueOpr.h"

#include "Def.h"
#include "sysMan/ScreenSaver.h"
#include "keyboard/KeyBoard.h"
#include "keyboard/KeyDef.h"

KeyValueOpr g_keyInterface;

int s_fdcom;  // 串口设备号
static int s_enableMouse = 1;

// ---------------------------------------------------------

gboolean GetKeyValue(GIOChannel* source, GIOCondition condition, gpointer data) {
  struct timeval timeout;

  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  unsigned char recvbuf[3] = {0};

  for (int i = 0; i < 3; i++) {
    recvbuf[i] = 0;
  }

  int len = PortRecv(s_fdcom, recvbuf, 1, timeout);

  if(len == 0) {
    return FALSE;
  }

  ScreenSaver::GetInstance()->Reset();

  int lenTotal = 0;

  switch(recvbuf[0]) {
  case 0x00:
  case 0xF5:  // value键
  case 0xF6:  // gain键
    lenTotal = 1;
    break;
  case 0x07:  // tgc
    lenTotal = 2;
    break;
  case 0x39:  // G70 旋钮键
    lenTotal = 2;
    break;
  case 0x08:
  case 0x18:
  case 0x28:
  case 0x38:
    lenTotal = 2;
    break;
  }

  unsigned char* pBuf = recvbuf + 1;

  while (lenTotal) {
    len = PortRecv(s_fdcom, pBuf, lenTotal, timeout);
    lenTotal -= len;
    pBuf += len;
  }

  PRINTF("** getkeyvalue: 0x%x,0x%x,0x%x **\n", recvbuf[0], recvbuf[1], recvbuf[2]);

  KeyValueOpr* pKeyInterface = (KeyValueOpr*)data;
  pKeyInterface->SendKeyValue( recvbuf ); // 发送键值

  return TRUE;
}

// 设置键盘串口并对串口进行监视
void* KeyboardOversee(void* pKeyInterface, bool isHandShake) {
  portinfo_t portinfo = {'0', 19200, '8', '0', '0', '0', '0', '0', '1', 0};

  s_fdcom = PortOpen( &portinfo );

  if(s_fdcom < 0) {
    exit(EXIT_FAILURE);
  }

  PRINTF("s_fdcom= %d\n", s_fdcom );

  PortSet(s_fdcom, &portinfo ); // 键盘串口设置
  PRINTF("set serial port success");

  if (isHandShake) {
    KbdHandShake(s_fdcom);      // 键盘初始化握手
  }

  // 键盘监视
  UartOversee(s_fdcom, pKeyInterface);

  return NULL;
}

void KeyboardRequestTGC() {
  unsigned char init_comm = 0x35;
  PortSend(s_fdcom, &init_comm, 1);
}

void KeyboardSound(bool value) {
  unsigned char soundValue = 0;

  if (value) {
    soundValue = KEY_SOUND_ON;
  } else {
    soundValue = KEY_SOUND_OFF;
  }

  PortSend(s_fdcom, &soundValue, 1);
}

// ---------------------------------------------------------

KeyValueOpr::KeyValueOpr() {
  PRINTF("Creat the keyboard interface thread");

  m_mouseSpeed = 1;
}

void KeyValueOpr::Push(AbsKeyboardEvent* win_opr) {
  m_winOprStack.push_back( win_opr );
}

void KeyValueOpr::Pop() {
  if (m_winOprStack.size() > 1) {
    m_winOprStack.pop_back();
  }
}

bool KeyValueOpr::Empty() {
  return m_winOprStack.empty();
}
int KeyValueOpr::Size() {
  return m_winOprStack.size();
}

void KeyValueOpr::SendKeyValue(unsigned char* keyValue) {
  int total;

  if (m_winOprStack.empty()) {
    PRINTF( "there is no window in stack!" );
    return;
  }

  total = m_winOprStack.size();
  PRINTF("** sendkeyvalue: 0x%x,0x%x,0x%x **\n", keyValue[0], keyValue[1], keyValue[2]);

  switch(keyValue[0]) {
  case 0x07:  // 接收到TGC发送给vector底窗口即主窗口
    if ((keyValue[2]>=0xA1)&&(keyValue[2]<=0xA9)) {
      m_winOprStack[0]->SliderEvent(keyValue[1], keyValue[2]);
    }
    break;
  case 0x0:   // 接收到其他发送给vector顶窗口即当前活动窗口
    m_winOprStack[total-1]->KeyEvent(keyValue[1]);
    break;
  case 0x39:
    m_winOprStack[total-1]->KnobEvent(keyValue[1], keyValue[2]);
    break;
  case 0xF5:
  case 0xF6:
    m_winOprStack[total-1]->KnobEvent(keyValue[0], keyValue[1]);
    break;
  case 0x08:
  case 0x18:
  case 0x28:
  case 0x38:
    {
      PRINTF("m_mouseSpeed = %d, s_enableMouse = %d\n", m_mouseSpeed, s_enableMouse);

      if (m_mouseSpeed == 0) {
        int x = ((int)(char)keyValue[1]) * 2;
        int y = ((int)(char)keyValue[2]) * 2;

        if (x > 127) {
          x = 127;
        } else if (x < -128) {
          x = -128;
        } else {
        }

        if (y > 127) {
          y = 127;
        } else if (y < -128) {
          y = -128;
        } else {
        }

        m_winOprStack[total-1]->MouseEvent(-x, -y);
      } else if (s_enableMouse == m_mouseSpeed) {
        m_winOprStack[total-1]->MouseEvent(-keyValue[1], -keyValue[2]);

        s_enableMouse = 1;
      } else {
        s_enableMouse ++;
      }

      break;
    }
  default:
    break;
  }

  return;
}

// 控制灯,点亮或熄灭
//
// on true-亮灯, false-灭灯
// lightValue light value to be control
void KeyValueOpr::CtrlLight(bool on, unsigned char lightValue ) {
  unsigned char value = 0;

  if (on) {
    AddLighten(lightValue);
    value = lightValue | 0x80;
  } else {
    RemoveLighten(lightValue);
    value = lightValue & 0x7F;

    if (lightValue == 12) {
      value = 0x0c;
    }
  }

  PRINTF("ligth value and value are :%d %d\n", lightValue, value);

  SendLightValue(value);
  usleep(20000);
}

AbsKeyboardEvent* KeyValueOpr::GetElement(int n) {
  if(n > m_winOprStack.size() - 1) {
    PRINTF(" The element want to get is out of range!\n" );
    return NULL;
  }

  return m_winOprStack[n];
}

int KeyValueOpr::GetMouseSpeed() {
  return m_mouseSpeed;
}

void KeyValueOpr::SetMouseSpeed(int s) {
  if (s <= 0) {
    m_mouseSpeed = 0;
  } else if (s > 10) {
    m_mouseSpeed = 10;
  } else {
    m_mouseSpeed = s;
  }

  s_enableMouse = 1;
}

// 检查灯点亮或熄灭
//
// lightValue light value to be check
// on true-亮灯, false-灭灯
bool KeyValueOpr::IsLighten(unsigned char lightValue) {
  vector<unsigned char>::iterator iter = find(m_vecLighten.begin(), m_vecLighten.end(), lightValue);

  if (iter != m_vecLighten.end()) {
    return true;
  } else {
    return false;
  }
}

void KeyValueOpr::ListLighten() {
  vector<unsigned char>::iterator iter;

  for(iter = m_vecLighten.begin(); iter < m_vecLighten.end(); iter++) {
    PRINTF("Lighten: %d\n", *iter);
  }
}

// ---------------------------------------------------------

// lightValue: 灯值
void KeyValueOpr::SendLightValue(unsigned char lightValue) {
  int valueLength = 1;
  PortSend(s_fdcom, &lightValue, valueLength);
}

// 在容器中记录点亮的灯
void KeyValueOpr::AddLighten(unsigned char lightValue) {
  vector<unsigned char>::iterator iter = find(m_vecLighten.begin(), m_vecLighten.end(), lightValue);

  if(iter == m_vecLighten.end()) {
    m_vecLighten.push_back(lightValue);
  }
}

// 将熄灭的灯从容器中删除
void KeyValueOpr::RemoveLighten(unsigned char lightValue) {
  vector<unsigned char>::iterator iter = find(m_vecLighten.begin(), m_vecLighten.end(), lightValue);

  if(iter != m_vecLighten.end()) {
    m_vecLighten.erase(iter);
  }
}
