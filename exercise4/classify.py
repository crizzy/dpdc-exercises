import jellyfish

# --- HOWTO Install Jellyfish ---
# sudo easy_install jellyfish
# if it doesn't already work:
# http://stackoverflow.com/questions/7446187/no-module-named-pkg-resources
# run distribute_setup.py

from os import listdir
from os.path import isfile, join
import csv

# map attribute names to column indices:
ci = {"id": 0, "culture": 1, "sex": 2, "age": 3, "date_of_birth": 4, "title": 5, "given_name": 6, "surname": 7, "state": 8, "suburb": 9, "postcode": 10, "street_number": 11, "address_1": 12, "address_2": 13, "phone_number": 14}

def jaro_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += jellyfish.jaro_distance(a, b)
	return sum / 14.0

def jaro_winkler_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += jellyfish.jaro_winkler(a, b)
	return sum / 14.0

def levenshtein_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += 1 - jellyfish.levenshtein_distance(a, b) / float(max(len(a), len(b)))
	return sum / 14.0

def damerau_levenshtein_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += 1 - jellyfish.damerau_levenshtein_distance(a, b) / float(max(len(a), len(b)))
	return sum / 14.0




def similarityMeasures(row1, row2):
	return "%.6f,%.6f,%.6f,%.6f" % (
		jaro_avg(row1,row2),
		jaro_winkler_avg(row1,row2),
		levenshtein_avg(row1,row2),
		damerau_levenshtein_avg(row1,row2))

mypath = "./clusters_complete/culture/" 
files = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]
results_file = open('results.pairs.tsv', 'w')
for fileName in files:
	if fileName == ".tsv":
		continue # temp # doesn't work

	print "Reading file: %s " % fileName
	# open the file:
	with open(mypath + fileName, "r") as file:
	
		table = []

		line = file.readline()
		while len(line) > 0:
			row = line.split("\t") #idea: column-based
			table += [row]
			line = file.readline()
	
		for index1, row1 in enumerate(table):
			for index2, row2 in enumerate(table[index1+1:]):
			
				# apply the learned rules from the trained model:
				if jaro_winkler_avg(row1, row2) >= 0.828265 and (damerau_levenshtein_avg >= 0.652218):
					results_file.write(row1[0] + "\t" + row2[0] + "\n")
