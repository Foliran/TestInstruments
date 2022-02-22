#include <iostream>
#include "keithleyTSP.h"
#include "keithleyTSPPulse.h"
#include <chrono>
#include <thread>
#include <memory>

std::pair<double, double> measure(keithleyTSP dev)
{
    double voltage =  dev.getVoltageCore();
    double current =  dev.getCurrentCore();
    return std::make_pair(current, voltage);
}

std::pair<double, double> measure(keithleyTSPPulse dev)
{
    double voltage = dev.getVoltageCore();
    double current = dev.getCurrentCore();
    return std::make_pair(current, voltage);
}


int main()
{
    std::cout << "Let's begin! \n";
    auto gpib = std::make_shared<GPIB>();
    int n = 10;
    double currentStart = 0.0;
    double steps = 0.01;
    keithleyTSPPulse device(gpib);
    device.openSystem();
    std::cout << "Before for loop" << std::endl;
    if (device.isOpen())
    {
        std::cout << "Connection is open!" << std::endl;
        for (int i = 0; i < n; i++)
        {
            double value = currentStart + i * steps;
            std::pair<double, double> values = device.setPulseAndMeasure(value, 0.5);
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            std::cout << "Measured current: " << values.first << ", measured voltage (K6500): " << values.second << ", resistance: " << values.second / values.first << std::endl;
        }
    }
    std::cout << "Done! \n";
    return 0;
}
