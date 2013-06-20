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



wekaFile = open('weka/culture.arff', 'w')
#todo header


#columnIndices = {"id": 0, "culture": 1, "sex": 2, "age": 3, "date_of_birth": 4, "title": 5, "given_name": 6, "surname": 7, "state": 8, "suburb": 9, "postcode": 10, "street_number": 11, "address_1": 12, "address_2": 13, "phone_number": 14}
#cluster = {"culture": 1, "date_of_birth": 4, "state": 8, "postcode": 10} #given
# todo: iterated over cluster folder

wekaFile.write("@relation 'dpdc-ex4'\n")
wekaFile.write("@attribute id1 real\n")
wekaFile.write("@attribute id2 real\n")
wekaFile.write("@attribute jaroAvg real\n")
wekaFile.write("@attribute levenshteinSum real\n")
wekaFile.write("@attribute class {0,1}\n")
wekaFile.write("@data\n")



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

			wekaRow = row1[0] + "," + row2[0] + "," + str(jaroAvg) + "," + str(levenshteinAvg) + ","

			

			if (duplicate in trueDuplicates):
				wekaRow += "1\n"
			else:
				wekaRow += "0\n"

			wekaFile.write(wekaRow)


			
			#	duplicate = [int(row1[0]), int(row2[0])]
				#todo 3er combinations #maybe create manually when creating true duplicates
			#if jaroAvg > 0.8:
			#	if (duplicate in trueDuplicates):
			#		print '%s is similar to %s. Found in file %s. jaroDistance avg measure = %f' % (row1[0], row2[0], fileName, jaroAvg)
				#else:
				#	print 'false positive'
				
	#print ''	

	




