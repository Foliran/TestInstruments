pWidth = 0.1
ratio = 0.5 

tsplink.reset(2)
node[2].smua.reset() 
node[2].tsplink.group = 0
node[2].smua.source.func = node[2].smua.OUTPUT_DCAMPS 
node[2].smua.source.limiti = 1.0  --in A: 1, 5, 10
node[2].smua.source.rangei = 1.0 --in A: 1, 5, 10
node[2].smua.measure.nplc = 0.1  --TODO: nplc anpassen?
node[2].smua.measure.rangev = 1.0
node[2].smua.measure.count = 1
node[2].smua.measure.autozero = node[2].smua.AUTOZERO_ONCE

-- timer[1] steuert die Pulsweite
-- timer[2] steuert, wann das DMM messen soll
node[2].smua.nvbuffer1.clear() 
node[2].smua.nvbuffer1.collectsourcevalues = 1 
node[2].smua.nvbuffer1.fillmode = node[2].smua.FILL_ONCE 

 -- ------------------------------------------------

node[2].trigger.timer[1].count = 1 
node[2].trigger.timer[1].passthrough = false 
node[2].trigger.timer[1].stimulus = node[2].smua.trigger.SOURCE_COMPLETE_EVENT_ID  --Hier evtl Event anpassen, vllt tsplink.trigger[1].EVENT_ID
node[2].trigger.timer[1].clear() 

node[2].trigger.timer[2].count = 1  
node[2].trigger.timer[2].passthrough = false 
node[2].trigger.timer[2].stimulus = node[2].smua.trigger.SOURCE_COMPLETE_EVENT_ID
node[2].trigger.timer[2].clear() 

node[2].smua.trigger.measure.stimulus = node[2].trigger.timer[2].EVENT_ID 
node[2].smua.trigger.source.limiti = 1
node[2].smua.trigger.measure.action = node[2].smua.ENABLE
node[2].smua.trigger.measure.i(node[2].smua.nvbuffer1) 
node[2].smua.trigger.endpulse.action = node[2].smua.SOURCE_IDLE 
node[2].smua.trigger.endsweep.action = node[2].smua.SOURCE_IDLE 
node[2].smua.trigger.endpulse.stimulus = node[2].trigger.timer[1].EVENT_ID 
node[2].smua.trigger.count = 1 
node[2].smua.trigger.arm.count = 1 
node[2].smua.trigger.source.stimulus = 0 
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
node[1].dmm.measure.nplc = 0.1  --TODO: nplc anpassen
node[1].dmm.measure.count = 1 
node[1].dmm.measure.autodelay = node[1].dmm.DELAY_OFF

node[1].trigger.model.load('EMPTY')
node[1].execute('trigger.model.setblock(1, node[1].trigger.BLOCK_BUFFER_CLEAR, node[1].defbuffer1)'..'trigger.model.setblock(1, node[1].trigger.BLOCK_WAIT, node[1].trigger.EVENT_TSPLINK1)'..'trigger.model.setblock(2, node[1].trigger.BLOCK_MEASURE_DIGITIZE, node[1].defbuffer1)')
waitcomplete(0)

node[2].tsplink.trigger[1].reset() 
node[2].tsplink.trigger[1].mode = node[2].tsplink.TRIG_FALLING  
node[2].tsplink.trigger[1].pulsewidth = 3e-6 --TODO: Hier Signaldauer anpassen
node[2].tsplink.trigger[1].stimulus = node[2].trigger.timer[2].EVENT_ID

node[1].tsplink.line[1].reset()
node[1].tsplink.line[1].mode = node[1].tsplink.MODE_TRIGGER_OPEN_DRAIN
node[1].trigger.tsplinkin[1].edge = node[1].trigger.EDGE_FALLING

----------------------------------------------------------------------
node[2].smua.trigger.source.listi({0.1})
node[2].trigger.timer[1].delay = pWidth
node[2].trigger.timer[2].delay = ratio * pWidth

node[2].smua.source.output = node[2].smua.OUTPUT_ON

node[1].trigger.model.initiate()
node[2].smua.trigger.initiate()

waitcomplete()
node[2].smua.source.output = node[2].smua.OUTPUT_OFF

print("Current: "..node[2].smua.nvbuffer1.sourcevalues[1])
node[2].smua.nvbuffer1.clear()

print("Voltage: "..node[1].defbuffer1.readings[1])
node[1].defbuffer1.clear()
