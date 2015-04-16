import random, struct, sched, time

s = sched.scheduler(time.time, time.sleep)
logfile = open("log.txt", "ab")
def write_file():
	print("hi")
	packed = struct.pack('ccbbbbb', b'k', b'u', random.randint(0,10), random.randint(0,10), random.randint(0,10), random.randint(0,10), random.randint(0,10))
	logfile.write(packed)
	logfile.flush()
	s.enter(1, 1, write_file, ())
	s.run()

write_file()
