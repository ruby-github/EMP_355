#ifndef _DEFAULTBIOPSYSETTING_H_
#define _DEFAULTBIOPSYSETTING_H_

#include "utils/IniFile.h"

class DefaultBiopsySetting {
public:
    DefaultBiopsySetting();
    ~DefaultBiopsySetting();

    //int GetBiopsyXOfAngleType(const string bioBracketTypeAngle);
    double GetBiopsyXOfAngleType(const string bioBracketTypeAngle);

    //int GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle);
    double  GetBiopsyAngleValueOfAngleType(const string bioBracketTypeAngle);

private:
    IniFile *m_ptrIni;
};

#endif
