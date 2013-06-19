import os

# map attribute names to column indices:
columnIndices = {"id": 0, "culture": 1, "sex": 2, "age": 3, "date_of_birth": 4, "title": 5, "given_name": 6, "surname": 7, "state": 8, "suburb": 9, "postcode": 10, "street_number": 11, "address_1": 12, "address_2": 13, "phone_number": 14}

def clusterFile(fileName, attributeName, prefixLength = 1024):
	
	# get the column index for this attribute:
	columnIndex = columnIndices[attributeName]
	
	# open the file:
	file = open(fileName, "r")
	
	# create the 'clusters' directory, if not existing:
	if not os.path.exists("./clusters"):
		os.makedirs("./clusters")
	
	# check if this cluster group has already been created:
	if os.path.exists("./clusters/" + attributeName):
		print "Skipping " + attributeName
		return
		
	print "Clustering " + attributeName
	
	# create a directory for the new cluster group:
	os.makedirs("./clusters/" + attributeName)
	
	# create a dictionary which assigns each attribute value to a file handle:
	clusterfiles = {}
	
	# parse the input data:
	value = ""
	line = file.readline()
	while len(line) > 0:
		value = line.split("\t")[columnIndex].strip().replace('|','1').replace(':','')[0:prefixLength]
		if not value in clusterfiles:
			clusterfiles[value] = open("./clusters/" + attributeName + "/" + value + ".tsv", "w")
		clusterfiles[value].write(line)
		line = file.readline()







# run clustering:
clusterFile("addresses/addresses.tsv", "culture");
clusterFile("addresses/addresses.tsv", "postcode", 2);
clusterFile("addresses/addresses.tsv", "state", 1);
clusterFile("addresses/addresses.tsv", "given_name", 1);
