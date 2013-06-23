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

# map attribute names to column indices:
ci = {"id": 0, "culture": 1, "sex": 2, "age": 3, "date_of_birth": 4, "title": 5, "given_name": 6, "surname": 7, "state": 8, "suburb": 9, "postcode": 10, "street_number": 11, "address_1": 12, "address_2": 13, "phone_number": 14}

def similarityMeasures(row1, row2):
	jaro_sum = 0
	jaro_winkler_sum = 0
	levenshtein_sum = 0
	damerau_levenshtein_sum = 0
	
	for columnIndex in range(1,15): #skips id column
		a = row1[columnIndex]
		b = row2[columnIndex]
		jaro_sum += jellyfish.jaro_distance(a, b)
		jaro_winkler_sum += jellyfish.jaro_winkler(a, b)
		levenshtein_sum += 1 - jellyfish.levenshtein_distance(a, b) / float(max(len(a), len(b)))
		damerau_levenshtein_sum += 1 - jellyfish.damerau_levenshtein_distance(a, b) / float(max(len(a), len(b)))

	return "%.6f,%.6f,%.6f,%.6f" % (
		jaro_sum / 14.0,
		jaro_winkler_sum / 14.0,
		levenshtein_sum / 14.0,
		damerau_levenshtein_sum / 14.0)
		
clusterName = "culture_given_name"

wekaFile = open('weka/' + clusterName + '.arff', 'w')
wekaFile.write("@relation 'dpdc-ex4'\n")
#wekaFile.write("@attribute id1 real\n")
#wekaFile.write("@attribute id2 real\n")
wekaFile.write("@attribute jaro_avg real\n")
wekaFile.write("@attribute jaro_winkler_avg real\n")
wekaFile.write("@attribute levenshtein_avg real\n")
wekaFile.write("@attribute damerau_levenshtein_avg real\n")

wekaFile.write("@attribute is_duplicate {0,1}\n")
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

		wekaFile.write(similarityMeasures(row1, row2) + ",1\n");
		true_pairs.append([int(pair[0]), int(pair[1])])

# add some negative examples to the training corpus:
mypath = "./clusters/" + clusterName + "/" 
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
	
	for index1, row1 in enumerate(table):
		for index2, row2 in enumerate(table[index1+1+random.randrange(100)::100]):
		
			pair = [int(row1[0]), int(row2[0])]
			if pair in true_pairs:
				continue

			wekaFile.write(similarityMeasures(row1, row2) + ",0\n")


