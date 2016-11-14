#!/usr/bin/python
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import scipy.io.wavfile as spio
import sys
from scipy.fftpack import fft
import csv

rawdata = spio.read(sys.argv[1])
split = rawdata[1]

i=0
ii=0
iii=0
jj=0
history=[]
total=[]
average=0
results=[]
prev = 43
ct=43
add=[]
lowfreq=np.ndarray(shape=(124,1))
a=0
#setup the buffer
while (i<43):
	# compute energy of the 1024 samples
	j=0
	while (j<1024):
		# get the 1024 samples
		total.append(split[jj][0]+split[jj][1])
		j+=1
		jj+=1
	# calculate freq distribution + energy
	fftArr = fft(total)
	energy = np.power(fftArr.real, 2) + np.power(fftArr.imag, 2)
	# just take the low freq energies
	energy = np.array(energy)
	ii=900
	iii=0
	# split the array in half
	while(ii<1024):
		lowfreq[iii] = energy[ii]
		ii+=1
		iii+=1
	add = np.sum(lowfreq)
	history.append(add)
	i+=1
	
# now it is an nd array
avg = np.array(history)

#process the whole file - ct is each sample (least 1024)
while (ct<((len(split)-44032)/1024)):

	# compute energy of the 1024 samples
	j=0
	total=[]
	if((len(split)-jj)>1024):
		while (j<1024):
			# get the 1024 samples
			total.append(split[jj][0]+split[jj][1])
			j+=1
			jj+=1
		# calculate freq distribution + energy
		fftArr = fft(total)
		energy = np.power(fftArr.real, 2) + np.power(fftArr.imag, 2)
		# just take the low freq energies
		energy = np.array(energy)
		ii=900
		iii=0
		while(ii<1024):
			lowfreq[iii] = energy[ii]
			ii+=1
			iii+=1
		add = np.sum(lowfreq)
		#average energy
		average = (np.sum(avg))/43

		if(add>average):
			if((ct-prev)*50>500):
				results.append((ct-prev)*50)
				prev = ct

		avg[ct%43]=add
		ct+=1

string = sys.argv[1]
res = string.split(".", 1)
count = 1
with open(res[0]+".txt", 'wb') as f:
	f.write("[")
	for s in results:
		if (count<len(results)):
			f.write(str(s))
			f.write(", ")
		else:
			f.write(str(s))
		count+=1
	f.write("]")









