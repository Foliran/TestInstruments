--[[Herausfinden der Messverzögerung: ratio auf 5ms, dann fahre nplc hoch bis error
Ergebnis: ab 0.211 nplc fängt es an zu fallen, hier misst er also drüber raus, also ab 4.2 ms wirds schwierig
Es gibt also ca. 4ms Verzögerung
Flanke ist circa 0.05 plc lang -> 1ms
]]
tsplink.reset(2)
node[2].smua.reset() 
node[2].tsplink.group = 1
node[2].smua.source.func = node[2].smua.OUTPUT_DCAMPS 
node[2].smua.source.limiti = 1.0  --in A: 1, 5, 10
node[2].smua.source.rangei = 1.0 --in A: 1, 5, 10
node[2].smua.measure.nplc = 1
node[2].smua.measure.rangei = 1.0
node[2].smua.measure.count = 1
node[2].smua.measure.autozero = node[2].smua.AUTOZERO_ONCE

node[2].smua.nvbuffer1.clear() 
node[2].smua.nvbuffer1.collectsourcevalues = 1 

--------------------------------------------------
-- timer[1] steuert die Pulsweite
-- timer[2] steuert den Zeitpunkt der Messung
-- timer[3] soll steuren, wann der Puls anfängt

node[2].trigger.timer[1].reset()
node[2].trigger.timer[1].count = 1 
node[2].trigger.timer[1].passthrough = false 
node[2].trigger.timer[1].stimulus = node[2].trigger.timer[3].EVENT_ID-- node[2].smua.trigger.SOURCE_COMPLETE_EVENT_ID  --Hier evtl Event anpassen, vllt COMPLETE_SOURCE
node[2].trigger.timer[1].clear() 

node[2].trigger.timer[2].reset()
node[2].trigger.timer[2].count = 1  
node[2].trigger.timer[2].passthrough = false 
node[2].trigger.timer[2].stimulus = node[2].smua.trigger.ARMED_EVENT_ID--node[2].trigger.timer[3].EVENT_ID
node[2].trigger.timer[2].clear() 

--timer[3] wird getriggert, wenn SMU aus der Armlayer kommt und triggert dann die anderen beiden Trigger,
--vllt synchronisiert das dann besser
node[2].trigger.timer[3].count = 1  
node[2].trigger.timer[3].passthrough = false 
node[2].trigger.timer[3].stimulus = node[2].smua.trigger.ARMED_EVENT_ID
node[2].trigger.timer[3].delay = 0.1
node[2].trigger.timer[3].clear() 

node[2].smua.trigger.measure.stimulus = node[2].trigger.timer[1].EVENT_ID 
node[2].smua.trigger.source.limiti = 1
node[2].smua.trigger.measure.action = node[2].smua.ENABLE
node[2].smua.trigger.measure.i(node[2].smua.nvbuffer1) 
node[2].smua.trigger.endpulse.action = node[2].smua.SOURCE_IDLE 
node[2].smua.trigger.endpulse.stimulus = node[2].trigger.timer[1].EVENT_ID 
node[2].smua.trigger.count = 1 
node[2].smua.trigger.arm.count = 1 
node[2].smua.trigger.source.stimulus = node[2].trigger.timer[3].EVENT_ID
node[2].smua.trigger.source.action = node[2].smua.ENABLE 

----------------------------------------------------------------------
--HIER KOMMT DAS DMM HIN
node[1].dmm.reset() 

node[1].tsplink.group = 2
node[1].defbuffer1.clear() 
node[1].defbuffer1.capacity = 10 
node[1].defbuffer1.fillmode = node[1].buffer.FILL_CONTINUOUS

node[1].dmm.measure.func = node[1].dmm.FUNC_DC_VOLTAGE 
node[1].dmm.measure.autozero.once() 
node[1].dmm.measure.autozero.enable = 0 
node[1].dmm.measure.nplc = 1.0
node[1].dmm.measure.count = 1 
node[1].dmm.measure.autodelay = node[1].dmm.DELAY_OFF
node[1].dmm.measure.range = 100e-3

node[1].execute("trigger.model.load('EMPTY')"..'trigger.model.setblock(1, node[1].trigger.BLOCK_WAIT, node[1].trigger.EVENT_TSPLINK1)'..'trigger.model.setblock(2, node[1].trigger.BLOCK_MEASURE_DIGITIZE, node[1].defbuffer1)')
waitcomplete(2)

node[2].tsplink.trigger[1].reset() 
node[2].tsplink.trigger[1].mode = node[2].tsplink.TRIG_FALLING  
node[2].tsplink.trigger[1].pulsewidth = 1e-6 --TODO: Hier Signaldauer anpassen
node[2].tsplink.trigger[1].stimulus = node[2].trigger.timer[2].EVENT_ID

node[1].tsplink.line[1].reset()
node[1].tsplink.line[1].mode = node[1].tsplink.MODE_TRIGGER_OPEN_DRAIN
node[1].trigger.tsplinkin[1].edge = node[1].trigger.EDGE_FALLING