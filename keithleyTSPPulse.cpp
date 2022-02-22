#include "keithleyTSPPulse.h"
//#include <QDebug>
//#include <QObject>
#include<memory>
#include<iostream>

//Pulse Example:github tektronix\keithley completePulse_CaseI_2601B_PULSE_DMM.tsp

const int GPIBADDRESS = 26;
const int DELAYGPIB = 0; //in ms
const bool TERMCHAR = false;

//Merke Node Nr. 1 = 2651!, Node Nr. 2 = DMM6500

//TODO: Autorange rausnehmen, range einstellen

/*
keithleyTSPPulse::keithleyTSPPulse(std::shared_ptr<GPIB> gpibNew) : //Timer[1] = Zeit zum Messen, Timer[2] = Pulsdauer, Timer[3] = 
    address(GPIBADDRESS)
    , gpib(gpibNew)
{
    std::cout << "Initialize constructor" << std::endl;
    gpib->cmd(address, "pulse_timer = trigger.timer[1]", DELAYGPIB, TERMCHAR); //Der pulse_timer kontrolliert die Pulsweite
    gpib->cmd(address, "meas_timer = trigger.timer[2]", DELAYGPIB, TERMCHAR); // Der meas_timer kontrolliert, wann das dmm messen soll
    handle = 1;

    //Einstellungen DMM
    gpib->cmd(address, "tsplink.reset()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].reset()", DELAYGPIB, true);
    gpib->cmd(address, "node[2].tsplink.group = 1", DELAYGPIB, true);

    gpib->cmd(address, "node[2].defbuffer1.clear()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].defbuffer1.capacity = 100000", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].defbuffer1.fillmode = node[2].buffer.FILL_ONCE", DELAYGPIB, TERMCHAR);

    gpib->cmd(address, "node[2].dmm.measure.func = node[2].dmm.FUNC_DC_VOLTAGE", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.autozero.once()", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.autozero.enable = 0", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.nplc = 0.01", DELAYGPIB, true); //TODO: nplc anpassen
    gpib->cmd(address, "node[2].dmm.measure.count = 1", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.autodelay = 0", DELAYGPIB, true);

    //Set trigger model for node[2]
    std::string quote = "\"";
    std::string s2 = "node[2].trigger.model.load(" + quote + "EMPTY" + quote + ")";
    gpib->cmd(address, "node[2].trigger.model.setblock(1, node[2].trigger.BLOCK_BUFFER_CLEAR, defbuffer1)", DELAYGPIB, true);
    gpib->cmd(address, "node[2].trigger.model.setblock(2, node[2].trigger.BLOCK_WAIT, node[2].trigger.EVENT_TSPLINK1, node[2].trigger.CLEAR_NEVER)", DELAYGPIB, true);
    gpib->cmd(address, "node[2].trigger.model.setblock(3, node[2].trigger.BLOCK_DELAY_CONSTANT, 0)", DELAYGPIB, true);
    gpib->cmd(address, "node[2].trigger.model.setblock(4, node[2].trigger.NOTIFY, node[2].trigger.EVENT_NOTIFY1)", DELAYGPIB, true);
    gpib->cmd(address, "node[2].trigger.model.setblock(5, node[2].trigger.BLOCK_BRANCH_COUNTER, 1, 2)", DELAYGPIB, true);
    gpib->cmd(address, "node[2].trigger.model.setblock(6, node[2].trigger.BLOCK_BRANCH_ALWAYS, 0)", DELAYGPIB, true);
    
    gpib->cmd(address, "node[2].tsplink.line[1].reset()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].tsplink.line[1].mode = node[2].tsplink.MODE_TRIGGER_OPEN_DRAIN", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].trigger.tsplinkin[1].edge = node[2].trigger.EDGE_FALLING", DELAYGPIB, TERMCHAR);

    gpib->cmd(address, "node[2].dmm.trigger.measure.stimulus = node[2].trigger.EVENT_TSPLINK1", DELAYGPIB, TERMCHAR);


    gpib->cmd(address, "trigger.timer[1].count = 1 ", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "trigger.timer[1].passthrough = false", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "trigger.timer[1].stimulus = trigger.timer[2].EVENT_ID ", DELAYGPIB, TERMCHAR); 

    gpib->cmd(address, "trigger.timer[2].reset()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "trigger.timer[2].delay = 1", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "trigger.timer[2].count = 1 ", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "trigger.timer[2].passthrough = true", DELAYGPIB, TERMCHAR);

    gpib->cmd(address, "node[1].tsplink.trigger[1].reset()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[1].tsplink.trigger[1].mode = node[1].tsplink.TRIG_FALLING", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[1].tsplink.trigger[1].pulsewidth = 3e-6", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[1].tsplink.trigger[1].stimulus = trigger.timer[2].EVENT_ID", DELAYGPIB, TERMCHAR);

    //Einstellungen SMU
    gpib->cmd(address, "node[1].tsplink.group = 1", DELAYGPIB, true);

    //TODO: Krieg ich irgendwie den Kanal oder muss ich halt Kanal 1 (A) nehmen
    gpib->cmd(address, "node[1].reset()", DELAYGPIB, TERMCHAR);
    if (handle == 1) {
        //Allgemeine Einstellungen SMU
        gpib->cmd(address, "node[1].smua.nvbuffer1.clear()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.nvbuffer1.capacity = 10", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.nvbuffer1.fillmode = node[2].smua.FILL_ONCE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.nvbuffer1.collectsourcevalues = 1", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.nvbuffer1.appendmode = 1", DELAYGPIB, TERMCHAR);

        gpib->cmd(address, "node[1].smua.source.func = node[1].smua.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);

        gpib->cmd(address, "node[1].smua.measure.delay = 0", DELAYGPIB, TERMCHAR); 
        gpib->cmd(address, "node[1].smua.measure.nplc = 0.1", DELAYGPIB, TERMCHAR); 
        gpib->cmd(address, "node[1].smua.measure.count = 1", DELAYGPIB, TERMCHAR); 
        gpib->cmd(address, "node[1].smua.trigger.source.action = smua.ENABLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.measure.action = node[1].smua.ASYNC", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.measure.stimulus = trigger.timer[2].EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.measure.i(smua.nvbuffer1)", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.measure.nplc = 0.001", DELAYGPIB, TERMCHAR); //TODO: nplc anpassen?

        /*
        //Timer 3 und 4 für genaue Messung
        gpib->cmd(address, "trigger.timer[3].reset()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[3].delay = 50e-6", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[3].count = 1 ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[3].passthrough = false", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[3].stimulus = trigger.timer[2].EVENT_ID", DELAYGPIB, TERMCHAR);

        gpib->cmd(address, "trigger.timer[4].reset()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[4].delay = 50e-6", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[4].count = 1 ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[4].passthrough = false", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[4].stimulus = trigger.timer[2].EVENT_ID", DELAYGPIB, TERMCHAR);
        
        gpib->cmd(address, "trigger.timer[2].stimulus = smua.trigger.ARMED_EVENT_ID", DELAYGPIB, TERMCHAR); 

        gpib->cmd(address, "node[1].smua.trigger.source.stimulus = trigger.timer[2].EVENT_ID ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.endpulse.action = node[1].smua.SOURCE_IDLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.count = 1", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.trigger.arm.count = 1", DELAYGPIB, TERMCHAR);
    }/*
    else if (handle == 2) {
        //TODO: Hier Befehle von oben anpassen, damit das auch mit Channel B geht
        gpib->cmd(address, "node[1].smub.source.func = node[1].smub.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);
    }
}
*/

keithleyTSPPulse::keithleyTSPPulse(std::shared_ptr<GPIB> gpibNew) :
    address(GPIBADDRESS)
    , gpib(gpibNew)
{
    std::cout << "Initialize constructor" << std::endl;
    gpib->cmd(address, "pulse_timer = trigger.timer[1]", DELAYGPIB, TERMCHAR); //Der pulse_timer kontrolliert die Pulsweite
    gpib->cmd(address, "meas_timer = trigger.timer[2]", DELAYGPIB, TERMCHAR); // Der meas_timer kontrolliert, wann das dmm messen soll
    handle = 1;

    //Einstellungen DMM
    gpib->cmd(address, "tsplink.reset(2)", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].smm.reset()", DELAYGPIB, true);
    //gpib->cmd(address, "node[2].tsplink.group = 1", DELAYGPIB, true);

    gpib->cmd(address, "node[2].defbuffer1.clear()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].defbuffer1.capacity = 100000", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].defbuffer1.fillmode = node[2].buffer.FILL_ONCE", DELAYGPIB, TERMCHAR);

    gpib->cmd(address, "node[2].dmm.measure.func = node[2].dmm.FUNC_DC_VOLTAGE", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.autozero.once()", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.autozero.enable = 0", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.nplc = 0.01", DELAYGPIB, true); //TODO: nplc anpassen
    gpib->cmd(address, "node[2].dmm.measure.count = 10", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.autodelay = 0", DELAYGPIB, true);

    gpib->cmd(address, "node[2].dmm.measure.read(defbuffer1)", DELAYGPIB, true);

    gpib->cmd(address, "node[2].tsplink.line[1].reset()", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].tsplink.line[1].mode = node[2].tsplink.MODE_TRIGGER_OPEN_DRAIN", DELAYGPIB, TERMCHAR);
    gpib->cmd(address, "node[2].trigger.tsplinkin[1].edge = node[2].trigger.EDGE_FALLING", DELAYGPIB, TERMCHAR);

    gpib->cmd(address, "node[2].dmm.trigger.measure.stimulus = node[2].trigger.EVENT_TSPLINK1", DELAYGPIB, TERMCHAR);

    //Einstellungen SMU
   // gpib->cmd(address, "node[1].tsplink.group = 1", DELAYGPIB, true);

    //TODO: Krieg ich irgendwie den Kanal oder muss ich halt Kanal 1 (A) nehmen
    if (handle == 1) {
        //Allgemeine Einstellungen SMU
        gpib->cmd(address, "smua.reset()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.nvbuffer1.clear()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.nvbuffer1.capacity = 10", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.nvbuffer1.fillmode = node[2].smua.FILL_ONCE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.nvbuffer1.collectsourcevalues = 1", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.nvbuffer1.appendmode = 1", DELAYGPIB, TERMCHAR);

        gpib->cmd(address, "smua.source.func = node[1].smua.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);

        //Probieren
        gpib->cmd(address, "smua.trigger.measure.i(smua.nvbuffer1)", DELAYGPIB, TERMCHAR);

        //Einstellungen Triggering für Pulse
        gpib->cmd(address, "tsplink.trigger[1].reset()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "tsplink.trigger[1].clear()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "tsplink.trigger.mode = node[1].tsplink.EVENT_ID", DELAYGPIB, TERMCHAR); //Hier evtl Event anpassen

        gpib->cmd(address, "smua.trigger.source.action = smua.ENABLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.trigger.measure.action = smua.ENABLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.source.limiti = 1", DELAYGPIB, TERMCHAR); //in A: 1, 5, 10
        gpib->cmd(address, "smua.measure.nplc = 0.001", DELAYGPIB, TERMCHAR); //TODO: nplc anpassen?

        gpib->cmd(address, "trigger.timer[1].count = 1 ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[1].passthrough = false", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[1].stimulus = smua.trigger.COMPLETE_SOURCE_EVENT_ID", DELAYGPIB, TERMCHAR); //Hier evtl Event anpassen ALT smua.trigger.ARMED_EVENT_ID
        gpib->cmd(address, "trigger.timer[1].clear()", DELAYGPIB, TERMCHAR);

        gpib->cmd(address, "trigger.timer[2].count = 1 tsplink.trigger[1].EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[2].passthrough = false", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "trigger.timer[2].stimulus = smua.trigger.COMPLETE_SOURCE_EVENT_ID", DELAYGPIB, TERMCHAR); //Evtl Event anpassen ALT SWEEPING
        gpib->cmd(address, "trigger.timer[2].clear()", DELAYGPIB, TERMCHAR);

        gpib->cmd(address, "node[2].dmm.measure.stimulus = trigger.timer[2].EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.trigger.source.stimulus = 0 ", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.trigger.endpulse.action = node[1].smua.SOURCE_IDLE", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.trigger.endpulse.action = trigger.timer[1].EVENT_ID", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.trigger.count = 1", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "smua.trigger.arm.count = 1", DELAYGPIB, TERMCHAR);
    }/*
    else if (handle == 2) {
        //TODO: Hier Befehle von oben anpassen, damit das auch mit Channel B geht
        gpib->cmd(address, "node[1].smub.source.func = node[1].smub.OUTPUT_DCAMPS", DELAYGPIB, TERMCHAR);
    }*/
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
        std::cout << s << std::endl;
        check = true;
    }
    return check;
}

void keithleyTSPPulse::setCurrentCore(double setpoint)
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

std::pair<double, double> keithleyTSPPulse::setPulseAndMeasure(double value, double pWidth)
{
    if (!gpib->isOpen(address))
    {
        return std::make_pair(0, 0);
    }
    double curr, volt = 0;

    if (handle == 1)
    {
        if (pWidth < 0.0001) {
            gpib->cmd(address, "trigger.timer[1].delay = 10e-6", DELAYGPIB, TERMCHAR);
        }
        else {
            std::string s = "trigger.timer[1].delay = " + std::to_string(0.1 * pWidth);
            gpib->cmd(address, s, DELAYGPIB, TERMCHAR);
        }
        std::string setCurrentString = "node[1].smua.trigger.source.listi({" + std::to_string(value) + "})";
        gpib->cmd(address, setCurrentString, DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.source.output = node[1].smua.OUTPUT_ON", DELAYGPIB, TERMCHAR);
        std::string text = "trigger.timer[2].delay = " + std::to_string(0.5 * pWidth);// +" - ((1/node[1].linefreq)*node[1].smua.measure.nplc) - 50e-6";
        gpib->cmd(address, text, DELAYGPIB, TERMCHAR);
        //gpib->cmd(address, "node[2].trigger.model.initiate()", DELAYGPIB, TERMCHAR);        
        gpib->cmd(address, "smua.trigger.initiate()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "waitcomplete()", DELAYGPIB, TERMCHAR);
        gpib->cmd(address, "node[1].smua.source.output = node[1].smua.OUTPUT_OFF", DELAYGPIB, TERMCHAR);
        //curr = std::stof(gpib->query(address, "print(node[1].smua.nvbuffer1.readings[1])", DELAYGPIB, true));
        gpib->cmd(address, "node[1].smua.nvbuffer1.clear()", DELAYGPIB, TERMCHAR);
        //volt = std::stof(gpib->query(address, "print(node[2].defbuffer1.readings[1])", DELAYGPIB, true));
        gpib->cmd(address, "node[2].defbuffer1.clear()", DELAYGPIB, true);
    }
    else if (handle == 2)
    {
        //TODO: Code für Channel B anpassen
    }

    return std::make_pair(curr, volt);
}


double keithleyTSPPulse::getVoltageCore()
{
    if (!gpib->isOpen(address))
    {
        return 0;
    }

    double volt = 0;
    gpib->cmd(address, "node[2].defbuffer1.clear()", DELAYGPIB, true);
    gpib->cmd(address, "node[2].dmm.measure.read()", DELAYGPIB, true);
    return std::stof(gpib->query(address, "print(node[2].defbuffer1.readings[1])", DELAYGPIB, true));
}

double keithleyTSPPulse::getCurrentCore() {
    if (!gpib->isOpen(address))
    {
        return 0;
    }
    gpib->cmd(address, "node[1].smua.nvbuffer1.clear()", DELAYGPIB, true);
    gpib->cmd(address, "node[1].smua.measure.i(node[1].smua.nvbuffer1)", DELAYGPIB, true);
    return std::stof(gpib->query(address, "print(node[1].smua.nvbuffer1.readings[1])", DELAYGPIB, true));
}
