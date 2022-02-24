#include "keithleyTSPPulse.h"
//#include <QDebug>
//#include <QObject>
#include<memory>
#include<iostream>

//Pulse Example:github tektronix\keithley completePulse_CaseI_2601B_PULSE_DMM.tsp

const int GPIBADDRESS = 26;
const int DELAYGPIB = 2; //in ms
const bool TERMCHAR = false;

//Merke Node Nr. 1 = 2651!, Node Nr. 2 = DMM6500

//TODO: Autorange rausnehmen, range einstellen


keithleyTSPPulse::keithleyTSPPulse(std::shared_ptr<GPIB> gpibNew) :
    address(GPIBADDRESS)
    , gpib(gpibNew)
{
    std::cout << "Initialize constructor" << std::endl;
    gpib->cmd(address,

        " tsplink.reset(2) "
        " node[2].tsplink.group = 1 "
        " node[2].smua.reset() "
        " node[2].beeper.enable = 0 "
        " node[2].smua.source.func = node[2].smua.OUTPUT_DCAMPS "
        " node[2].smua.source.limiti = 1.0  --in A : 1, 5, 10 "
        " node[2].smua.source.rangei = 1.0 --in A : 1, 5, 10 "
        " node[2].smua.measure.nplc = 0.001  --TODO : nplc anpassen ? "
        " node[2].smua.measure.rangei = 1.0 "
        " node[2].smua.measure.count = 1 "
        " node[2].smua.measure.autozero = node[2].smua.AUTOZERO_ONCE "

        //timer[1] steuert die Pulsweite
        //timer[2] steuert, wann das DMM messen soll
        " node[2].smua.nvbuffer1.clear() "
        " node[2].smua.nvbuffer1.collectsourcevalues = 1 "
        " node[2].smua.nvbuffer1.fillmode = node[2].smua.FILL_ONCE "
        " node[2].smua.nvbuffer2.clear() "
        " node[2].smua.nvbuffer2.fillmode = node[2].smua.FILL_ONCE "


        " node[2].trigger.timer[1].count = 1 "
        " node[2].trigger.timer[1].passthrough = false "
        " node[2].trigger.timer[1].stimulus = node[2].smua.trigger.SOURCE_COMPLETE_EVENT_ID  --Hier evtl Event anpassen, vllt tsplink.trigger[1].EVENT_ID "
        " node[2].trigger.timer[1].clear() "

        " node[2].trigger.timer[2].count = 1 "
        " node[2].trigger.timer[2].passthrough = false "
        " node[2].trigger.timer[2].stimulus = node[2].smua.trigger.SOURCE_COMPLETE_EVENT_ID "
        " node[2].trigger.timer[2].clear() "

        " node[2].smua.trigger.measure.stimulus = node[2].trigger.timer[2].EVENT_ID "
        " node[2].smua.trigger.source.limiti = 1 "
        " node[2].smua.trigger.measure.action = node[2].smua.ENABLE "
        " node[2].smua.trigger.measure.iv(node[2].smua.nvbuffer1, node[2].smua.nvbuffer2) "
        " node[2].smua.trigger.endpulse.action = node[2].smua.SOURCE_IDLE "
        " node[2].smua.trigger.endsweep.action = node[2].smua.SOURCE_IDLE "
        " node[2].smua.trigger.endpulse.stimulus = node[2].trigger.timer[1].EVENT_ID "
        " node[2].smua.trigger.count = 1 "
        " node[2].smua.trigger.arm.count = 1 "
        " node[2].smua.trigger.source.stimulus = 0 "
        " node[2].smua.trigger.source.action = node[2].smua.ENABLE "

        " node[1].dmm.reset() "

        " node[1].tsplink.group = 2 "
        " node[1].defbuffer1.clear() "
        " node[1].defbuffer1.capacity = 10 "
        " node[1].defbuffer1.fillmode = node[1].buffer.FILL_CONTINUOUS "

        " node[1].dmm.measure.func = node[1].dmm.FUNC_DC_VOLTAGE "
        " node[1].dmm.measure.autozero.once() "
        " node[1].dmm.measure.autozero.enable = 0 "
        " node[1].dmm.measure.nplc = 0.01  --TODO: nplc anpassen "
        " node[1].dmm.measure.count = 1 "
        " node[1].dmm.measure.autodelay = node[1].dmm.DELAY_OFF "

        " node[2].tsplink.trigger[1].reset() "
        " node[2].tsplink.trigger[1].mode = node[2].tsplink.TRIG_FALLING "
        " node[2].tsplink.trigger[1].pulsewidth = 3e-6 --TODO: Hier Signaldauer anpassen "
        " node[2].tsplink.trigger[1].stimulus = node[2].trigger.timer[2].EVENT_ID "

        " node[1].execute('trigger.model.load('EMPTY')') "
        " node[1].execute('trigger.model.setblock(1, node[1].trigger.BLOCK_BUFFER_CLEAR, node[1].defbuffer1)') "
        " node[1].execute('trigger.model.setblock(2, node[1].trigger.BLOCK_WAIT, node[1].trigger.EVENT_TSPLINK1, trigger.CLEAR_ENTER)') "
        " node[1].execute('trigger.model.setblock(3, node[1].trigger.BLOCK_MEASURE_DIGITIZE, node[1].defbuffer1)') "

        " node[1].tsplink.line[1].reset() "
        " node[1].tsplink.line[1].mode = node[1].tsplink.MODE_TRIGGER_OPEN_DRAIN "
        " node[1].trigger.tsplinkin[1].edge = node[1].trigger.EDGE_FALLING "
        " waitcomplete(0) ",
        DELAYGPIB, TERMCHAR);
    
}



void keithleyTSPPulse::openSystem() {
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

bool keithleyTSPPulse::isOpen() {
    bool check = false;
    std::string s = gpib->query(address, "print(tsplink.state)", DELAYGPIB, TERMCHAR);
    if (s == "online")
    {
        //std::cout << s << std::endl;
        check = true;
    }
    return check;
}


std::pair<double, double> keithleyTSPPulse::setPulseAndMeasure(double value, double pWidth, double ratio)
{
    if (!gpib->isOpen(address))
    {
        return std::make_pair(0, 0);
    }
    double curr, volt = 0;
    gpib->cmd(address,
        " node[2].trigger.timer[1].delay = " + std::to_string(pWidth) + 
        " node[2].trigger.timer[2].delay = " + std::to_string(ratio * pWidth) +
        " node[2].smua.trigger.source.listi({"+ std::to_string(value)+ "}) "

        " node[2].smua.source.output = node[2].smua.OUTPUT_ON "

        " node[1].trigger.model.initiate() "
        " node[2].smua.trigger.initiate() "

        " waitcomplete() "
        " node[2].smua.source.output = node[2].smua.OUTPUT_OFF ",
        DELAYGPIB, TERMCHAR);

    curr = std::stod(gpib->query(address, " print(node[2].smua.nvbuffer1.readings[1]) ", DELAYGPIB, TERMCHAR));
    gpib->cmd(address, " node[2].smua.nvbuffer1.clear() ", DELAYGPIB, TERMCHAR);
    volt = std::stod(gpib->query(address, " print(node[1].defbuffer1.readings[1]) ", DELAYGPIB, TERMCHAR));
    gpib->cmd(address, " node[1].defbuffer1.clear() ", DELAYGPIB, TERMCHAR);
    return std::make_pair(curr, volt);
}
