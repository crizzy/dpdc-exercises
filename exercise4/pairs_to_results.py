r = open("results.pairs.tsv", "r")
w = open("DPDCAssignment4Group01.tsv", "w")

pairs =[]
line = r.readline()
while len(line) > 0:
	pairs.append(line.replace("\n", "").split("\t"))
	line = r.readline()
r.close();

finished = False
while not finished:
	finished = True
	for pos1 in xrange(len(pairs)):
		for pos2 in xrange(pos1+1,len(pairs)):
			if len(list(set(pairs[pos1]) & set(pairs[pos2]))) > 0:
				pairs[pos1] = list(set(pairs[pos1]) | set(pairs[pos2]))
				pairs[pos2] = []
				finished = False

for pair in pairs:
	if len(pair) > 0:
		pair_as_int = sorted([int(n) for n in pair])
		pair_as_str = [str(n) for n in pair_as_int]
		w.write('\t'.join(pair_as_str) + "\n")
w.close()