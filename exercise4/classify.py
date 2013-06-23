import jellyfish

from os import listdir
from os.path import isfile, join
import csv

# map attribute names to column indices:
ci = {"id": 0, "culture": 1, "sex": 2, "age": 3, "date_of_birth": 4, "title": 5, "given_name": 6, "surname": 7, "state": 8, "suburb": 9, "postcode": 10, "street_number": 11, "address_1": 12, "address_2": 13, "phone_number": 14}

def get_jaro_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += jellyfish.jaro_distance(a, b)
	return sum / 14.0

def get_jaro_winkler_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += jellyfish.jaro_winkler(a, b)
	return sum / 14.0

def get_levenshtein_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += 1 - jellyfish.levenshtein_distance(a, b) / float(max(len(a), len(b)))
	return sum / 14.0

def get_damerau_levenshtein_avg(row1, row2):
	sum = 0
	for columnIndex in xrange(1,15):
		a = row1[columnIndex]
		b = row2[columnIndex]
		sum += 1 - jellyfish.damerau_levenshtein_distance(a, b) / float(max(len(a), len(b)))
	return sum / 14.0




def similarityMeasures(row1, row2):
	return "%.6f,%.6f,%.6f,%.6f" % (
		get_jaro_avg(row1,row2),
		get_jaro_winkler_avg(row1,row2),
		get_levenshtein_avg(row1,row2),
		get_damerau_levenshtein_avg(row1,row2))
		
mypath = "./clusters_complete/culture_given_name/"
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
	
		for index1 in xrange(len(table)):
			for index2 in xrange(index1+1,len(table)):
			
				jaro_avg = 0
				jaro_winkler_avg = 0
				damerau_levenshtein_avg = 0
				for columnIndex in xrange(1,15):
					a = table[index1][columnIndex]
					b = table[index2][columnIndex]
					jaro_avg += jellyfish.jaro_distance(a, b)
					jaro_winkler_avg += jellyfish.jaro_winkler(a, b)
					damerau_levenshtein_avg += 1 - jellyfish.damerau_levenshtein_distance(a, b) / float(max(len(a), len(b)))
				jaro_avg /= 14.0
				jaro_winkler_avg /= 14.0
				damerau_levenshtein_avg /= 14.0

				# apply the learned rules from the trained model:
				#if jaro_winkler_avg >= 0.844955 or ((damerau_levenshtein_avg >= 0.650227) and (jaro_winkler_avg >= 0.833977)):
				#	results_file.write(table[index1][0] + "\t" + table[index2][0] + "\n")
				
				# apply the learned rules from the trained model:
				if jaro_avg >= 0.837464 or ((jaro_winkler_avg >= 0.830973) and (damerau_levenshtein_avg >= 0.660897)):
					results_file.write(table[index1][0] + "\t" + table[index2][0] + "\n")

results_file.close()