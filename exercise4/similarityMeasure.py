import jellyfish

# --- HOWTO Install Jellyfish ---
# sudo easy_install jellyfish
# if it doesn't already work:
# http://stackoverflow.com/questions/7446187/no-module-named-pkg-resources
# run distribute_setup.py

from os import listdir
from os.path import isfile, join
import csv

#print jellyfish.levenshtein_distance('jellyfish', 'smellyfish')
#print jellyfish.jaro_distance('jellyfish', 'smellyfish')
#print jellyfish.damerau_levenshtein_distance('jellyfish', 'jellyfihs')


# prepare goldstandard evaluation
resultsFileName = 'addresses/results.sample.pairs.tsv'
#resultsFile = open(resultsFileName, "r")
trueDuplicates = []
with open('addresses/results.sample.tsv', 'rb') as csvfile:
	spamreader = csv.reader(csvfile, delimiter='\t')
	for row in spamreader:
		newRow = []
		for entry in row:
			newRow.append(int(entry))
		trueDuplicates.append(newRow)
# todo: sort and down there seek in sorted list


cluster = ['culture', 'state', 'given_name' 'post_code'] # todo: not finished yet: relate to index so 1 column less has to be compared


mypath = "./clusters/culture/" 
files = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]
for fileName in files:
	if fileName == ".tsv":
		continue # temp # doesn't work

	print "Reading file: %s " % fileName
	# open the file:
	file = open(mypath + fileName, "r")

	table = []

	line = file.readline()
	while len(line) > 0:
		row = line.split("\t") #idea: column-based
		table = table + [row]
		line = file.readline()
	
	#print "Building table done."

	for index1, row1 in enumerate(table):
		for index2, row2 in enumerate(table[index1:]): 
			perPairSum = 0
			for columnIndex in range(len(row1)):
				perPairSum += jellyfish.jaro_distance(row1[columnIndex],row2[columnIndex])
			perPairAvg = perPairSum / len(row1)
			
			if perPairAvg > 0.8:
				duplicate = [int(row1[0]), int(row2[0])]
				#todo 3er combinations #maybe create manually when creating true duplicates
				if (duplicate in trueDuplicates):
					print '%s is similar to %s. Found in file %s. similarity measure = %f' % (row1[0], row2[0], fileName, perPairAvg)
				#else:
				#	print 'false positive'
				
	print ''	

	




