#ifndef GPIB_H
#define GPIB_H

#include <string>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif
#include <windows.h>
#include "ni488.h"
#ifdef __cplusplus
}
#endif

class GPIB
{
public:
    GPIB();

    void openDevice(int deviceAddress);
    void closeDevice(int deviceAddress);
    bool isOpen(int deviceAddress) const;

    void cmd(int deviceAddress, std::string command, int delay, bool termchar);
    std::string query(int deviceAddress, std::string queryStr, int delay, bool termchar);
    void checkstatus();

    std::string getError() const;
    int getHandle(int address) const;
private:
    void init();

private:
    int* ibsta_;
    long* ibcntl_;
    int* iberr_;
    int(__stdcall* ibclr_)(int ud);
    int(__stdcall* ibdev_)(int ud, int pad, int sad, int tmo, int eot, int eos);
    int(__stdcall* ibconfig_)(int ud, int option, int v);
    int(__stdcall* ibrd_)(int ud, PVOID buf, LONG cnt);
    int(__stdcall* ibwrt_)(int ud, PVOID buf, LONG cnt);
    int(__stdcall* ibonl_)(int ud, int v);

    std::map<int, int> deviceHandles_;
    char readBuffer_[512];

    std::string statusGpib(int ibsta);
    std::string errorCode(int iberr);

    std::string errormessage_;
};

#endif // GPIB_H
