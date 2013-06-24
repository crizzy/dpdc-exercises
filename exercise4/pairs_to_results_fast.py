r = open("results.pairs.tsv", "r")
w = open("test2.tsv", "w")

pairs =[]
line = r.readline()
while len(line) > 0:
	pairs.append(line.replace("\n", "").split("\t"))
	line = r.readline()
r.close();

dict = {}
rdict = {}
eclasses = []

already_seen = set()

for pair in pairs:
	if pair[0] in dict:
		dict[pair[0]].append(pair[1])
	else:
		dict[pair[0]] = [pair[1]]
	
	if pair[1] in rdict:
		rdict[pair[1]].append(pair[0])
	else:
		rdict[pair[1]] = [pair[0]]	
	
for key, value in dict.iteritems():
	#case 0:
	if key in already_seen:
		continue
	
	eclass = set([key]+value)

	#case 1: key is in none of the equivalent classes:
	added_values = eclass
	while True:
		eclass |= added_values
		new_added_values = set()
		
		for v in added_values:
			if v in dict:
				new_added_values |= set(dict[v])
			if v in rdict:
				new_added_values |= set(rdict[v])
		new_added_values -= eclass

		if len(new_added_values) == 0:
			break
		added_values = new_added_values

	already_seen |= eclass
	pair_as_int = sorted([int(n) for n in eclass])
	pair_as_str = [str(n) for n in pair_as_int]
	w.write('\t'.join(pair_as_str) + "\n")
				

#for key, value in dict.iteritems():
#	if len(value) > 0:
#		pair_as_int = sorted([int(n) for n in value])
#		pair_as_str = [str(n) for n in pair_as_int]
#		w.write('\t'.join(pair_as_str) + "\n")
w.close()