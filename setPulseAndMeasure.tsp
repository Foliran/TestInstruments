node[2].smua.trigger.source.listi({value})
node[2].trigger.timer[1].delay = pWidth
node[2].trigger.timer[2].delay = ratio
node[2].smua.source.output = node[2].smua.OUTPUT_ON
	
node[1].trigger.model.initiate()
node[2].smua.trigger.initiate()
	
waitcomplete()
node[2].smua.source.output = node[2].smua.OUTPUT_OFF
	