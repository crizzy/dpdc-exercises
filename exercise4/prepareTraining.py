import jellyfish

# --- HOWTO Install Jellyfish ---
# sudo easy_install jellyfish
# if it doesn't already work:
# http://stackoverflow.com/questions/7446187/no-module-named-pkg-resources
# run distribute_setup.py

from os import listdir
from os.path import isfile, join
import csv
import random

random.seed()

def similarityMeasures(row1, row2):
	jaroSum = 0
	levenshteinSum = 0
	for columnIndex in range(1,len(row1)): #skips id column
		firstWord = row1[columnIndex]
		secondWord = row2[columnIndex]
		jaroSum += jellyfish.jaro_distance(firstWord, secondWord)
		levenshteinSum += 1 - jellyfish.levenshtein_distance(firstWord, secondWord) / float(max(len(firstWord), len(secondWord)))

	jaroAvg = jaroSum / len(row1)
	levenshteinAvg = levenshteinSum / len(row1)
	return [jaroAvg, levenshteinAvg]

wekaFile = open('weka/culture.arff', 'w')
wekaFile.write("@relation 'dpdc-ex4'\n")
wekaFile.write("@attribute id1 real\n")
wekaFile.write("@attribute id2 real\n")
wekaFile.write("@attribute jaroAvg real\n")
wekaFile.write("@attribute levenshteinSum real\n")
wekaFile.write("@attribute class {0,1}\n")
wekaFile.write("@data\n")

true_pairs = []
print "Analyzing gold standard..."

# add all true examples to the training corpus:
with open('addresses/results.sample.pairs.tsv', 'rb') as gold_standard_file:
	true_pairs_file = csv.reader(gold_standard_file, delimiter='\t')
	for pair in true_pairs_file:
		# read the data of the pair from the addresses file:
		f = open('addresses/addresses.tsv', 'rb')
		for i in xrange(int(pair[0])):
			f.readline() # skip lines until we reach the desired row
		row1 = f.readline().split('\t')
		f.close()
		f = open('addresses/addresses.tsv', 'rb')
		for i in xrange(int(pair[1])):
			f.readline() # skip lines until we reach the desired row
		row2 = f.readline().split('\t')
		f.close()
		
		wekaFile.write(pair[0] + "," + pair[1] + ",");
		for value in similarityMeasures(row1, row2):
			wekaFile.write("%.5f," % value)
		wekaFile.write("1\n");
		true_pairs.append([int(pair[0]), int(pair[1])])

# add some negative examples to the training corpus:
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
	
	for index1, row1 in enumerate(table[::100]):
		for index2, row2 in enumerate(table[index1+1::100]):
		
			pair = [int(row1[0]), int(row2[0])]
			if pair in true_pairs:
				continue

			wekaRow = row1[0] + "," + row2[0] + ","
			
			for value in similarityMeasures(row1, row2):
				wekaRow += "%.5f," % value

			wekaRow += "0\n"

			wekaFile.write(wekaRow)


