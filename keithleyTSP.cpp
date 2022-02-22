#include "keithleyTSP.h"
//#include <QDebug>
//#include <QObject>
#include<memory>
#include<iostream>


const int GPIBADDRESS = 26;
const int DELAYGPIB = 0; //in ms
const bool TERMCHAR = false;

// Merke Node Nr. 1 = 2651!, Node Nr. 2 = DMM6500

keithleyTSP::keithleyTSP(std::shared_ptr<GPIB> gpibNew):
      address(GPIBADDRESS)
    , gpib(gpibNew)
{
    std::cout << "Initialize constructor" << std::endl;
    handle = 1;
    //Einstellungen DMM
    gpib->cmd(address, "num = tsplink.reset()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].reset()", DELAYGPIB, true);
    gpib->cmd(address, "node[2].tsplink.group = 1", DELAYGPIB, true);

    gpib->cmd(address, "node[2].defbuffer1.clear()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].defbuffer1.capacity = 10", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].defbuffer1.fillmode = node[2].buffer.FILL_ONCE", DELAYGPIB, TERMCHAR);

    gpib->cmd(address, "node[2].dmm.measure.func = node[2].dmm.FUNC_DC_VOLTAGE", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.autozero.enable = node[2].dmm.ON", DELAYGPIB, true);

    gpib->cmd(address, "node[2].tsplink.line[1].reset()", DELAYGPIB, TERMCHAR);
    //gpib->cmd(address, "node[2].tsplink.line[1].mode = node[2].tsplink.MODE_TRIGGER_OPEN_DRAIN", DELAYGPIB, TERMCHAR);
    //gpib->cmd(address, "node[2].trigger.tsplinkin[1].edge = node[2].trigger.EDGE_FALLING", DELAYGPIB, TERMCHAR);

    //gpib->cmd(address, "node[2].dmm.trigger.measure.stimulus = node[2].trigger.EVENT_TSPLINK1", DELAYGPIB, TERMCHAR);

    //Einstellungen SMU
    gpib->cmd(address, "node[1].tsplink.group = 1", DELAYGPIB, true);
    
    //TODO: Krieg ich irgendwie den Kanal oder muss ich halt Kanal 1 (A) nehmen
    gpib->cmd(address, "node[1].reset()", DELAYGPIB, TERMCHAR);
    if(handle == 1) {
        gpib->cmd(address, "node[1].smua.nvbuffer1.clear()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.nvbuffer1.capacity = 10", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.nvbuffer1.fillmode = node[1].smua.FILL_ONCE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.nvbuffer1.collectsourcevalues = 1", DELAYGPIB, TERMCHAR);

        gpib->cmd(address, "node[1].smua.source.func = node[1].smua.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);


        //gpib->cmd(address, "node[1].tsplink.trigger[1].reset()", DELAYGPIB, TERMCHAR);
        //gpib->cmd(address, "node[1].tsplink.trigger.mode = node[1].tsplink.EVENT_ID", DELAYGPIB, TERMCHAR);
    } else if (handle == 2) {
        //TODO: Hier Befehle von oben anpassen, damit das auch mit Channel B geht
        gpib->cmd(address, "node[1].smub.source.func = node[1].smub.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);
    }
}

void keithleyTSP::openSystem() {
    if (gpib == nullptr)
    {
        return;
    }
    //qDebug() << "open TSP-Link";
    gpib->openDevice(address);

    gpib->cmd(address, "tsplink.reset()", DELAYGPIB, TERMCHAR);

    std::string idn = gpib->query(address, "print(tsplink.state)", DELAYGPIB, TERMCHAR);
    if (idn.find("online") == std::string::npos)
    {
        gpib->closeDevice(address);
    }

    if (!gpib->isOpen(address))
    {
        std::string errormessage = "TSP-Link: ";
        if (gpib->getError().size() == 0)
        {
            errormessage.append("Not connected");
        }
        else
        {
            errormessage.append(gpib->getError().c_str());
        }
        //emit newErrorTSP(errormessage);
	    std::cout << "Error: System kann nicht geöffnet werden." << std::endl;
        return;
    }
}

bool keithleyTSP::isOpen() {
    bool check = false;
    std::string s = gpib->query(address, "print(tsplink.state)", DELAYGPIB, TERMCHAR);
    if(s == "online")
    {
        std::cout << s << std::endl;
        check = true;
    }
    return check;
}

void keithleyTSP::setCurrentCore(double setpoint)
{
    if (!gpib->isOpen(address))
    {
        return;
    }

    if (handle == 1)
    {
        std::string setCurrentString = "node[1].smua.source.leveli = " + std::to_string(setpoint);
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.source.output = node[1].smua.OUTPUT_ON", DELAYGPIB, TERMCHAR);
    }
    else if (handle == 2)
    {
        std::string setCurrentString = "node[1].smub.source.leveli = " + std::to_string(setpoint);
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.source.output = node[1].smub.OUTPUT_ON", DELAYGPIB, TERMCHAR);
    }
}

/*void keithleyTSP::setCurrentSweepCore(double start, double end, double rate)
{
    if (!gpib->isOpen(address))
    {
        return;
    }
    //n zählt die Anzahl an Messpunkten, da diese an sie SMU übergeben werden muss
    int n = 0;
    //Liste mit Werten erstellen:
    std::string stringList = "{";
    if(start < end) {
        double x = start;
        while(x < (end-rate)) {
            stringList.append(std::to_string(x));
            stringList.append(", ");
            x+=rate;
            n++;
        }
        stringList.append(std::to_string(x));
        stringList.append("}");
        n++;
    } else if(start > end) {
        double x = start;
        while(x > (end+rate)) {
            stringList.append(std::to_string(x));
            stringList.append(", ");
            x-=rate;
            n++;
        }
        stringList.append(std::to_string(x));
        stringList.append("}");
        n++;
    }


    if (handle == 1)
    {
        gpib->cmd(address, "node[1].smua.reset()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.source.func = node[1].smua.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);
        std::string s = "node[1].smua.trigger.source.listi(";
        std::string setCurrentString = s.append(stringList).append(")");
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.source.action = node[1].smua.ENABLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.measure.action = node[1].smua.DISABLE", DELAYGPIB, TERMCHAR);
    }
    else if (handle == 2)
    {
        gpib->cmd(address, "node[1].smub.reset()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.source.func = node[1].smub.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);
        std::string s = "node[1].smub.trigger.source.listi(";
        std::string setCurrentString = s.append(stringList).append(")");
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.source.action = node[1].smub.ENABLE", DELAYGPIB, TERMCHAR);
    }
}
*/

/*void keithleyTSP::setCurrentPulseCore(double start, double end, double rate)
{
    if (!gpib->isOpen(address))
    {
        return;
    }
    //n zählt die Anzahl an Messpunkten, da diese an sie SMU übergeben werden muss
    double pulsewidth = 0.1; // in s
    double pulseperiod = 1; // in s
    int n = 0;
    //Liste mit Werten erstellen:
    std::string stringList = "{";
    if(start < end) {
        double x = start;
        while(x < (end-rate)) {
            stringList.append(std::to_string(x));
            stringList.append(", ");
            x+=rate;
            n++;
        }
        stringList.append(std::to_string(x));
        stringList.append("}");
        n++;
    } else if(start > end) {
        double x = start;
        while(x > (end+rate)) {
            stringList.append(std::to_string(x));
            stringList.append(", ");
            x-=rate;
            n++;
        }
        stringList.append(std::to_string(x));
        stringList.append("}");
        n++;
    }
    // Befehle sind nach dem Vorbild des K2651A Reference Manuals, Abschnitt 3-47 (Seite 155 im pdf) geschrieben
    gpib->cmd(address, "period_timer = trigger.timer[1]", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "pulse_timer = trigger.timer[2]", DELAYGPIB, TERMCHAR);

    if (handle == 1)
    {
        gpib->cmd(address, "node[1].smua.reset()", DELAYGPIB, TERMCHAR);
        // Evtl. noch node[1].smua.source.range setzen?
        std::string setCurrentString = "node[1].smua.trigger.source.listi(" + stringList + ")";
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.source.action = node[1].smua.ENABLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.measure.action = node[1].smua.DISABLE", DELAYGPIB, TERMCHAR);
        std::string text = "pulse_timer.delay = " + std::to_string(pulsewidth);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "pulse_timer.stimulus = period_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "pulse_timer.count = 1", DELAYGPIB, TERMCHAR);
        text = "period_timer.delay = " + std::to_string(pulseperiod);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        text = "period_timer.count = " + std::to_string(n-1);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "period_timer.stimulus = node[1].smua.trigger.SWEEPING_EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "period_timer.passthrough = true", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.source.stimulus = period_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.endpulse.action = node[1].smua.SOURCE_IDLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.endpulse.action = pulse_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.count = 1", DELAYGPIB, TERMCHAR);
        text = "node[1].smua.trigger.arm.count = " + std::to_string(n);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.source.output = node[1].smua.OUTPUT_ON", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.initiate()", DELAYGPIB, TERMCHAR);
    }
    else if (handle == 2)
    {
        gpib->cmd(address, "node[1].smub.reset()", DELAYGPIB, TERMCHAR);
        //gpib->cmd(address, "node[1].smub.reset()", DELAYGPIB, TERMCHAR);
        // Evtl. noch node[1].smub.source.range setzen?
        std::string s = "node[1].smub.trigger.source.listi(";
        std::string setCurrentString = s.append(stringList).append(")");
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.source.action = node[1].smua.ENABLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.measure.action = node[1].smua.DISABLE", DELAYGPIB, TERMCHAR);
        std::string text = "pulse_timer.delay = " + std::to_string(pulsewidth);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "pulse_timer.stimulus = period_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "pulse_timer.count = 1", DELAYGPIB, TERMCHAR);
        text = "period_timer.delay = " + std::to_string(pulseperiod);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        text = "period_timer.count = " + std::to_string(n-1);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "period_timer.stimulus = node[1].smub.trigger.SWEEPING_EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "period_timer.passthrough = true", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.source.stimulus = period_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.endpulse.action = node[1].smub.SOURCE_IDLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.endpulse.action = pulse_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.count = 1", DELAYGPIB, TERMCHAR);
        text = "node[1].smub.trigger.arm.count = " + std::to_string(n);
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.source.output = node[1].smua.OUTPUT_ON", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smub.trigger.initiate()", DELAYGPIB, TERMCHAR);
    }

}
*/

/*
double keithleyTSP::setPulseAndMeasure(double value, double pWidth)
{
    if (!gpib->isOpen(address))
    {
        return 0 ;
    }

    gpib->cmd(address, "pulse_timer = trigger.timer[1]", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "meas_timer = trigger.timer[2]", DELAYGPIB, TERMCHAR);
    if (handle == 1)
    {
        std::string setCurrentString = "node[1].smua.trigger.source.listi(" + std::to_string(value) + ")";
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.source.action = node[1].smua.ENABLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.measure.action = node[1].smua.DISABLE", DELAYGPIB, TERMCHAR);
	    std::string text = "pulse_timer.delay = " + std::to_string(pWidth) + " - ((1/node[1].linefreq)*node[1].smua.nplc) - 50e-6";
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "pulse_timer.count = 1 ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "pulse_timer.passthrough = false", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "pulse_timer.stimulus = node[1].smua.trigger.ARMED_EVENT_ID ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "meas_timer.delay ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "meas_timer.count = 1 ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "meas_timer.passthrough = false", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "meas_timer.stimulus = node[1].smua.trigger.SWEEPING_EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[2].dmm.measure.stimulus = meas_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.source.stimulus = 0 ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.endpulse.action = node[1].smua.SOURCE_IDLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.endpulse.action = pulse_timer.EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.count = 1", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.arm.count = 1", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.source.output = node[1].smua.OUTPUT_ON", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.initiate()", DELAYGPIB, TERMCHAR);
    }
    else if (handle == 2)
    {
    }
}
*/

double keithleyTSP::setCurrentAndMeasure(double value) {
    if (!gpib->isOpen(address))
    {
        return 0 ;
    }
    if (handle == 1)
    {
        return 0;
    }
    return 0;
}

double keithleyTSP::getVoltageCore()
{
    if (!gpib->isOpen(address))
    {
        return 0;
    }
    double volt = 0;
    gpib->cmd(address, "node[2].defbuffer1.clear()", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.read(node[2].defbuffer1)", DELAYGPIB, true);
    volt = std::stof(gpib->query(address, "print(node[2].defbuffer1.readings[1])", DELAYGPIB, true));
    //volt = std::stof(gpib->query(address, "print(node[2].dmm.measure.read(node[2].defbuffer1))", DELAYGPIB, true)); //Funktioniert, brauch ich aber nachdem das andere funktioniert eigentlich nicht mehr
    return volt;
}

double keithleyTSP::getCurrentCore() {
    if (!gpib->isOpen(address))
    {
        return 0;
    }
    gpib->cmd(address, "node[1].smua.nvbuffer1.clear()", DELAYGPIB, true);
    gpib->cmd(address, "node[1].smua.measure.i(node[1].smua.nvbuffer1)", DELAYGPIB, true);
    double current = std::stof(gpib->query(address, "print(node[1].smua.nvbuffer1.readings[1])", DELAYGPIB, true));
    //double current = std::stof(gpib->query(address, "print(node[1].smua.measure.i())", DELAYGPIB, true)); //Funktioniert! Brauch ich aber nicht mehr
    return current;
}
