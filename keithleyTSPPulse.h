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
    std::pair<double, double> setPulseAndMeasure(double value, double pWidth, double ratio);


private:
    int address;
    std::shared_ptr<GPIB> gpib;
    int handle;
};

#endif // KEITHLEYTSP_H
