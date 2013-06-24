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
resultsFile = open(resultsFileName, "r")
trueDuplicates = []
with open('addresses/results.sample.pairs.tsv', 'rb') as csvfile:
	spamreader = csv.reader(csvfile, delimiter='\t')
	for row in spamreader:
		newRow = []
		for entry in row:
			newRow.append(int(entry))
		trueDuplicates.append(newRow)
# todo: sort and down there seek in sorted list




#columnIndices = {"id": 0, "culture": 1, "sex": 2, "age": 3, "date_of_birth": 4, "title": 5, "given_name": 6, "surname": 7, "state": 8, "suburb": 9, "postcode": 10, "street_number": 11, "address_1": 12, "address_2": 13, "phone_number": 14}
#cluster = {"culture": 1, "date_of_birth": 4, "state": 8, "postcode": 10} #given
# todo: iterated over cluster folder


tp = 0
fp = 0
fn = 0
tn = 0


mypath = "./clusters_complete/culture_given_name/" 
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


		for index2, row2 in enumerate(table[index1+1:]): 
			jaroSum = 0
			levenshteinSum = 0
			for columnIndex in range(1,len(row1)): #skips id column
				firstWord = row1[columnIndex]
				secondWord = row2[columnIndex]
				jaroSum += jellyfish.jaro_distance(firstWord, secondWord)
				levenshteinSum += 1 - jellyfish.levenshtein_distance(firstWord, secondWord) / max(len(firstWord), len(secondWord))

			jaroAvg = jaroSum / len(row1)
			levenshteinAvg = levenshteinSum / len(row1)
			
			duplicate = [int(row1[0]), int(row2[0])]

			#wekaRow = row1[0] + "," + row2[0] + "," + str(jaroAvg) + "," + str(levenshteinAvg) + ","

			

			

			
			isDuplicateInReal = duplicate in trueDuplicates
			if jaroAvg > 0.763689: # we think it is duplicate
				if isDuplicateInReal:
					print 'true positive: %s is similar to %s. Found in file %s. jaroDistance avg measure = %f' % (row1[0], row2[0], fileName, jaroAvg)
					tp += 1
				else:
					#print 'false positive'
					fp += 1
			else: # we think it is not a duplicate		
				if isDuplicateInReal:
					print 'false negative %s is similar to %s. Found in file %s. jaroDistance avg measure = %f' % (row1[0], row2[0], fileName, jaroAvg)
					fn += 1
				else:
					#print 'true negative'
					tn += 1
		

	#not exactly true because I don't compare across clusters
	print "true positives:  " + str(tp)
	print "false positives: " + str(fp)
	print "false negatives: " + str(fn)
	print "true negatives:  " + str(tn)	

	tp = float(tp)
	fp = float(fp)
	fn = float(fn)
	tn = float(tn)

	precision = tp/(tp+fp)
	recall = tp/(tp + fn)
	fmeasure = 2 * precision * recall / (precision + recall)

	print "precision: " + str(precision)
	print "recall: " + str(recall)
	print "fmeasure: " + str(fmeasure)

	print '###############################################################'



