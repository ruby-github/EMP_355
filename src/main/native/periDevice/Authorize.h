/**
* @brief Authorize.h锛氬姞瀵嗙嫍鎺堟潈淇濇姢鍔熻兘,鍙€傜敤浜嶴afeNet鍏徃鐨凷entinel HASP

* @author: hehao
* @version: v1.0
* @data: 2012.10.25
*/

#ifndef AUTHORIZE_H
#define AUTHORIZE_H
#include <gtk/gtk.h>

class CAuthorize
{
	public:
		static CAuthorize *GetInstance();
		virtual ~CAuthorize();
		void Excute(int argc, char *argv[]);
		void AddTimeout();

		/*
		 * 函数功能：验证key的有效性
		 * 参数：
		 *    feature锛氬姛鑳絀D
		 *  返回值:
		 *    0:授权   -1:未找到设备   -2：过期  -3：未授权
		 */
		int CheckAuthorize(unsigned int feature);

	private:
		CAuthorize();
		static gboolean CallbackUKey(gpointer data);

		int m_timeoutIdUkey;
};

#endif //AUTHORIZE_H
