import os

# map attribute names to column indices:
ci = {"id": 0, "culture": 1, "sex": 2, "age": 3, "date_of_birth": 4, "title": 5, "given_name": 6, "surname": 7, "state": 8, "suburb": 9, "postcode": 10, "street_number": 11, "address_1": 12, "address_2": 13, "phone_number": 14}

def clusterFile(fileName, attributeNames, prefixLength = 1024):
	
	columnIndices = []
	
	# get the column index for this attribute:
	for attributeName in attributeNames:
		columnIndices.append(ci[attributeName])
		
	# join attribute names to a single specifier:
	attributeNamesStr = '_'.join(attributeNames)
	
	# open the file:
	file = open(fileName, "r")
	
	# create the 'clusters' directory, if not existing:
	if not os.path.exists("./clusters"):
		os.makedirs("./clusters")
	
	# check if this cluster group has already been created:
	if os.path.exists("./clusters/" + attributeNamesStr):
		print "Skipping " + attributeNamesStr
		return
		
	print "Clustering " + attributeNamesStr
	
	# create a directory for the new cluster group:
	os.makedirs("./clusters/" + attributeNamesStr)
	
	# create a dictionary which assigns each attribute value to a file handle:
	clusters = {}
	
	# parse the input data:
	value = ""
	line = file.readline()
	#while len(line) > 0:
	for i in xrange(100000):
		values = []
		for columnIndex in columnIndices:
			values.append(line.split("\t")[columnIndex].strip().replace('|','1').replace(':','')[0:prefixLength])
		value = '_'.join(values)
		if not value in clusters:
			clusters[value] = []
		clusters[value].append(line)
		line = file.readline()
		
	for clusterName, cluster in clusters.iteritems():
		w = open("./clusters/" + attributeNamesStr + "/" + clusterName.replace('?','') + ".tsv", "w")
		for line in cluster:
			w.write(line)
		w.close()

# run clustering:
clusterFile("addresses/addresses.tsv", ["culture", "given_name"], 2)
#clusterFile("addresses/addresses.tsv", "postcode", 2);
#clusterFile("addresses/addresses.tsv", "state", 1);
#clusterFile("addresses/addresses.tsv", "given_name", 1);
