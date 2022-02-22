#ifndef KEITHLEYTSPPULSE_H
#define KEITHLEYTSPPULSE_H
#include "gpib.h"
#include<memory>
//#include <QString>


class keithleyTSPPulse // : public QObject
{
    //    Q_OBJECT
public:
    keithleyTSPPulse(std::shared_ptr<GPIB> gpibNew);
    void openSystem();
    bool isOpen();
    void setCurrentCore(double setpoint);
    //void setCurrentSweepCore(double start, double end, double rate);
    //void setCurrentPulseCore(double start, double end, double rate);
    std::pair<double, double> setPulseAndMeasure(double value, double pWidth);
    double getCurrentCore();
    double getVoltageCore();


private:
    int address;
    std::shared_ptr<GPIB> gpib;
    int handle;
};

#endif // KEITHLEYTSP_H
