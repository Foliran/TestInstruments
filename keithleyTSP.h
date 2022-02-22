#ifndef KEITHLEYTSP_H
#define KEITHLEYTSP_H
#include "gpib.h"
#include<memory>
//#include <QString>


class keithleyTSP // : public QObject
{
//    Q_OBJECT
public:
    keithleyTSP(std::shared_ptr<GPIB> gpibNew);
    void openSystem();
    bool isOpen();
    //void printHello();
    void setCurrentCore(double setpoint);
    //void setCurrentSweepCore(double start, double end, double rate);
    //void setCurrentPulseCore(double start, double end, double rate);
    //double setPulseAndMeasure(double value, double pWidth);
    double setCurrentAndMeasure(double value);
    double getCurrentCore();
    double getVoltageCore();


private:
    int address;
    std::shared_ptr<GPIB> gpib;
    int handle;
};

#endif // KEITHLEYTSP_H
