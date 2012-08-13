#!/usr/bin/python
import sys
import os
import random
import math
import array
import subprocess
import time

def fuzz_file(seed,infile,outfile,bytes,fuzz):
	input_file = open(infile,"rb")
	output_file = open(outfile,"wb")

	# exceptions should be handled outside
	
	random.seed(seed)

	buf = array.array('c')
	buf.fromfile(input_file,max(bytes)+1)

	numwrites = random.randrange(math.ceil(float(len(bytes)) * fuzz)) + 1
	for j in range(numwrites):
		rbyte = random.randrange(256)
		rn = random.randrange(len(bytes))
		buf[bytes[rn]] = "%c"%(rbyte);
			
	buf.tofile(output_file)

	chunk = "chunk"

	while len(chunk):
		chunk = input_file.read(256)
		output_file.write(chunk)

	input_file.close()
	output_file.close()	

def cycle(rseed):
	fuzz_file(rseed,sys.argv[1],"test.sm3",[0,1] + range(34,300),FuzzFactor)
	
	testp = subprocess.Popen(
		["testGenerator","test.sm3","--quiet"],
		executable="./testGenerator",
		stdout=subprocess.PIPE, stderr = subprocess.STDOUT)
	
	result = str(rseed).rjust(6,'0')+" "
	if testp.poll() is None:
		time.sleep(0.2)
	if testp.poll() is None:
		time.sleep(0.4)
	if testp.poll() is None:
		time.sleep(0.8)
	if testp.poll() is None:
		time.sleep(1.6)
	if testp.poll() is None:
		try:
			testp.terminate()
		except:
			pass
		finally:
			result += "Hung"
	else:
		result += " ".join(testp.stdout.readlines()).strip()
		
	print result
	
if __name__ == "__main__":

	if len(sys.argv) < 2:
		quit()

	filename = sys.argv[1]

	FuzzFactor = 0.20

	if len(sys.argv) > 2:
		try:
			cycle(int(sys.argv[2]))
		except:
			with open(sys.argv[2]) as ns:
				for seed in ns:
					cycle(int(seed.split()[0]))
	else:
		for i in xrange(10000):
			random.seed(int(time.time()*random.random()))
			cycle(random.randint(0,1000000))
