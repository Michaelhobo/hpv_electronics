# (*) Import plotly package
import plotly
 
# Check plolty version (if not latest, please upgrade)
plotly.__version__

# (*) To communicate with Plotly's server, sign in with credentials file
import plotly.plotly as py  
 
# (*) Useful Python/Plotly tools
import plotly.tools as tls   
 
# (*) Graph objects to piece together plots
from plotly.graph_objs import *
 
import numpy as np  # (*) numpy for math functions and arrays

tls.set_credentials_file(username='michaelhobo', api_key='ohovaaa4gc', stream_ids=[
	"49lgm9y6o4",
	"bn72dolxcj",
	"zrt3xa68wm",
	"hs44q2pj21",
	"9zn37ntt8r",
])

stream_ids = tls.get_credentials_file()['stream_ids']
def init_stream(ID, name):
	# Get stream id from stream id list 
	stream_id = stream_ids[ID]
	# Make instance of stream id object 
	stream = Stream(
		token=stream_id,  # (!) link stream id to 'token' key
		maxpoints=80      # (!) keep a max of 80 pts on screen
	)

	# Initialize trace of streaming plot by embedding the unique stream_id
	trace1 = Scatter(
		x=[],
		y=[],
		mode='lines+markers',
		stream=stream         # (!) embed stream id, 1 per trace
	)

	data = Data([trace1])

	# Add title to layout object
	layout = Layout(title='Time Series')

	# Make a figure object
	fig = Figure(data=data, layout=layout)

	# (@) Send fig to Plotly, initialize streaming plot, open new tab
	unique_url = py.plot(fig, filename=name)

	# (@) Make instance of the Stream link object, 
	#     with same stream id as Stream id object
	s = py.Stream(stream_id)

	# (@) Open the stream
	s.open()
	return s

print("initializing")

k_spd_stream = init_stream(0, "Klondike Speed")
k_cad_stream = init_stream(1, "Klondike Cadence")
k_tmp_stream = init_stream(2, "Klondike Inner Temperature")
k_brt_stream = init_stream(3, "Klondike Outdoor Brightness")
k_occ_stream = init_stream(4, "Klondike Occupancy")

print("done initializing")

# (*) Import module keep track and format current time
import datetime 
import time   
import sys
import struct

f = open('log.txt', 'rb')
f.read()
k_numsensors = 5
while True:
	print("read")
	c = f.read(1)
	if c == b'k':
		print("k")
		c = f.read(1)
		if c == b'u':
			print("u")
			x = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
			c = f.read(k_numsensors)
			formatstr = 'b' * k_numsensors
			print("data read")
			data = struct.unpack(formatstr, c[0:k_numsensors])
			if data[0] != 255:
				k_spd_stream.write(dict(x=x, y=data[0] * 3 / 10))
			if data[1] != 255:
				k_cad_stream.write(dict(x=x, y=data[1]))
			if data[2] != 255:
				k_tmp_stream.write(dict(x=x, y=data[2]))
			if data[3] != 255:
				k_brt_stream.write(dict(x=x, y=data[3]))
			if data[4] != 255:
				k_occ_stream.write(dict(x=x, y=data[4]))
	elif len(c) == 0:
		time.sleep(0.25)
# (@) Close the stream when done plotting
s.close() 
