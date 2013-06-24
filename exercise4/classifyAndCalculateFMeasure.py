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
	
'''
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
print trueDuplicates
# todo: sort and down there seek in sorted list


tp = 0
fp = 0
fn = 0
tn = 0
'''




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
				
				
				jaro = [0]*15 # data in first cell will be 0 and not be changed

				numColumns = 14.0

				jaro_avg = 0
				jaro_winkler_avg = 0
				damerau_levenshtein_avg = 0
				levenshtein_avg = 0
				for columnIndex in xrange(1,15):
					a = table[index1][columnIndex][1:]
					b = table[index2][columnIndex][1:]
					


					if a=="" or b=="":
						numColumns -= 1
					else:
						jaro_tmp = jellyfish.jaro_distance(a, b) 	
						jaro[columnIndex] = jaro_tmp
						jaro_avg += jaro_tmp
						jaro_winkler_avg += jellyfish.jaro_winkler(a, b)
						damerau_levenshtein_avg += 1 - jellyfish.damerau_levenshtein_distance(a, b) / float(max(len(a), len(b)))
						levenshtein_avg += 1 - jellyfish.levenshtein_distance(a, b) / float(max(len(a), len(b)))


				jaro_avg /= numColumns
				jaro_winkler_avg /= numColumns
				damerau_levenshtein_avg /= numColumns

				# apply the learned rules from the trained model:
				#if jaro_winkler_avg >= 0.844955 or ((damerau_levenshtein_avg >= 0.650227) and (jaro_winkler_avg >= 0.833977)):
				#	results_file.write(table[index1][0] + "\t" + table[index2][0] + "\n")
				
				'''
				#duplicate = [int(table[index1][0]), int(table[index2][0])]

				isDuplicateInReal = duplicate in trueDuplicates'''
				# apply the learned rules from the trained model:
				#if jaro_avg >= 0.837464 or ((jaro_winkler_avg >= 0.830973) and (damerau_levenshtein_avg >= 0.660897)):
				#	results_file.write(table[index1][0] + "\t" + table[index2][0] + "\n")
				
				if (jaro_avg >= 0.828424) and (damerau_levenshtein_avg >= 0.730869) or \
					(jaro_avg >= 0.821857) and (jaro[12] >= 0.83683) or \
					(jaro_avg >= 0.830184) and (jaro[14] >= 0.924908) or \
					(jaro[9] >= 0.867133) and (jaro_avg >= 0.804137) and (levenshtein_avg >= 0.635941):
					

					results_file.write(table[index1][0] + "\t" + table[index2][0] + "\n")
				'''	
					if isDuplicateInReal:
						print 'true positive: %s is similar to %s.' % (duplicate[0], duplicate[1])
						tp += 1
					else:
						#print 'false positive'
						fp += 1
				else: # we think it is not a duplicate		
					if isDuplicateInReal:
						print 'false negative %s is similar to %s.' % (duplicate[0], duplicate[1])
						fn += 1
					else:
						#print 'true negative'
						tn += 1
				'''
	#not exactly true because I don't compare across clusters
'''	print "true positives:  " + str(tp)
	print "false positives: " + str(fp)
	print "false negatives: " + str(fn)
	print "true negatives:  " + str(tn)	

	tp = float(tp)
	fp = float(fp)
	fn = float(fn)
	tn = float(tn)

	if tp > 0:
		precision = tp/(tp+fp)
		recall = tp/(tp + fn)
		fmeasure = 2 * precision * recall / (precision + recall)

		print "precision: " + str(precision)
		print "recall: " + str(recall)
		print "fmeasure: " + str(fmeasure)

	print '###############################################################'
'''

results_file.close()