r = open("addresses/results.sample.tsv", "r")
w = open("addresses/results.sample.pairs.tsv", "w")

line = r.readline()
while len(line) > 0:
	indices = line.replace("\n", "").split("\t")
	for left in range(0, len(indices)):
		for right in range(left+1, len(indices)):
			if len(indices[right]) > 0:
				w.write(indices[left] + "\t" + indices[right] + "\n");
	line = r.readline()

