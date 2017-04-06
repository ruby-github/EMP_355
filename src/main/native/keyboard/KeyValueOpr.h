//*************operator.h

#ifndef __KEY_OPERATOR_H_
#define __KEY_OPERATOR_H_
#include "AbsUpdate2D.h"
#include <gtk/gtk.h>
#include "Def.h"
#include <vector>
#include "keyboard/KeyBoard.h"
#include "AbsKeyboardEvent.h"

using std::vector;

/************************************
 *KeyValueOpr:		键盘接口类
 *.WinOptStack:		鐢ㄤ簬淇濆瓨鍚勪釜绐楀彛瀵归敭鐩樼殑鍝嶅簲鍑芥暟鎸囬拡鐨剉ector
 *.SendLightValue: 	接收键盘灯值,并将灯值发送给键盘
 *.Push: 			鍦ㄦ瘡涓獥鍙ｅ缓绔嬫椂璋冪敤姝ゅ嚱鏁板皢鏈獥鍙ｅ閿洏鐨勫搷搴斿嚱鏁版寚閽堝帇鍏ector
 *.Pop:				鍦ㄦ瘡涓獥鍙ｉ€€鍑烘槸璋冪敤姝ゅ嚱鏁板皢鏈獥鍙ｅ閿洏鐨勫搷搴斿嚱鏁版寚閽堜粠vector鎾ら攢
 *.GetElement:		获得vector的第n个元素，此函数暂时没用
 *.SendKeyValue:	灏嗘帴鏀跺埌鐨勯敭鍊煎彂閫佺粰鍚勪釜绐楀彛
 ***********************************/

class KeyValueOpr
{
	public:
		KeyValueOpr( );

		void CtrlLight(bool on, unsigned char lightValue );
		void Push( AbsKeyboardEvent * win_opr );
		void Pop( );
		bool Empty();
		int Size();
		AbsKeyboardEvent * GetElement( int n );
		void SendKeyValue( unsigned char *keyValue );
		void SetMouseSpeed( int s);
		int GetMouseSpeed(void);
		bool IsLighten(unsigned char lightValue);
		void ListLighten(); //for debug
	private:
		int m_mouseSpeed;
		vector<AbsKeyboardEvent *> WinOprStack;
		void SendLightValue( unsigned char light_value );

		vector<unsigned char> m_vecLighten;
		void AddLighten(unsigned char lightValue);
		void RemoveLighten(unsigned char lightValue);
};

extern KeyValueOpr g_keyInterface;

//鑾峰彇閿€煎苟鍙戦€佺粰鍚勪釜绐楀彛
extern gboolean GetKeyValue(GIOChannel *source, GIOCondition condition, gpointer data);

//閿洏鐩戣
extern void *KeyboardOversee( void * pKeyInterface, bool isHandShake = true);

//请求获得键盘TGC值
void KeyboardRequestTGC(void);

//閿洏澹伴煶鎺у埗
void KeyboardSound(bool value);

#endif //__OPERATOR_H_
