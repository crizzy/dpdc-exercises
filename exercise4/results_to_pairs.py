r = open("results.sample.tsv", "r")
w = open("results.sample.pairs.tsv", "w")

line = r.readline()
while len(line) > 0:
	indices = line.replace("\n", "").split("\t")
	for left in range(0, len(indices)):
		for right in range(left+1, len(indices)):
			w.write(indices[left] + "\t" + indices[right] + "\n");
	line = r.readline()

