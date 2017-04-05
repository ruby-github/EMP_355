#ifndef _BATTERY__H_
#define _BATTERY__H_

class Battery
{
    public:
        Battery(){  }
        ~Battery(){  }
        int GetCapacity();

    private:
        const unsigned char ADDR = 0x0b;
        const unsigned char GET_CHARGE = 0x0d;
};

#endif

