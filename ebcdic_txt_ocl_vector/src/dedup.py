import csv

vals = []
with open('idxs.out', newline='') as csvfile:
	reader = csv.reader(csvfile, delimiter=' ', quotechar='|')
	"""for row in reader:	
		my_val = row[0]	
		my_val = int(my_val)
		print(my_val + 5)
		vals.append(row[0])"""
	vals = [int(row[0]) for row in reader]

#print(vals[0] + vals[1])
#print(vals)

		 	
#print(vals)
seen = set()
uniq = []
for idx in vals:
	if idx not in seen:
		uniq.append(idx)
		seen.add(idx)
	else:
		print(idx, "is a duplicate")
print(seen)
