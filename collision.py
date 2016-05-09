#!bin/python

import random
from sortedcontainers import SortedSet
from array import array
import matplotlib.pyplot as plt
from math import sqrt

# Generate a list of randomly chosen numbers
# stop after the first collision returning
# when it occurred
def birthdayCollision(population):
	firstCollision = -1
	selected = SortedSet()
	testCount = 0
	while firstCollision < 0:
		randomChoice = random.randint(0, population)
		try:
			if selected.index(randomChoice) >= 0:
				firstCollision = testCount
		except:
			testCount = testCount
		selected.add(randomChoice)
		testCount += 1
	return firstCollision

# NOT USED
# Generate a list of randomly chosen numbers
# Final list size will be smaller than number
# of selections due to collisions
def generateList(population, selections):
	selected = SortedSet()
	for count in range(selections):
		randomChoice = random.randint(0, population)
		selected.add(randomChoice)
	return selected

# NOT USED
# Select random numbers until a colision occurs
# with one of the entries in the separate list
# provided
def testUntilCollision(population, selected):
	collision = False
	testCount = 0
	while not collision:
		test = random.randint(0, population)
		testCount += 1
		try:
			if selected.index(test) >= 0:
				collision = True
		except:
			pass
	return testCount

# Select random numbers placed in two separate
# bins and continue until a collision occurs
# between the two bins (i.e. same value in
# each bin)
def twoBinCollision(population):
	collision = False
	testCount = 0
	binLeft = SortedSet()
	binRight = SortedSet()
	while not collision:
		testLeft = random.randint(0, population)
		testRight = random.randint(0, population)

		testCount += 1
		# Check collision of right value in left bin
		try:
			if binLeft.index(testRight) >= 0:
				collision = True
		except:
			pass

		# Check collision of left value in right bin
		try:
			if binRight.index(testLeft) >= 0:
				collision = True
		except:
			pass

		# Add the values to their respective bins
		binLeft.add(testLeft)
		binRight.add(testRight)
	return testCount

# Perform a series of tests in order to generate
# a cummulative PDF
def performTests():
	population = 10000
	selections = int(round(sqrt(population)))
	experimentTotal = 10000
	countsBirthday = array("i", [0] * population)
	countsBin = array("i", [0] * population)
	countsDouble = array("i", [0] * population)

	# Perform multiple experiments in order to 
	# producce the PDF
	for experimentCount in range(experimentTotal):
		# Experiment using two bins
		collision = twoBinCollision(population)
		countsBin[collision] += 1
		countsDouble[collision * 2] += 1

		# Experiment using one bin
		firstCollision = birthdayCollision(population)
		countsBirthday[firstCollision] += 1


	# Convert into a cummultaive distribiutiion
	cumulativeBin = countsBin
	for experimentCount in range(1, population):
		cumulativeBin[experimentCount] += cumulativeBin[experimentCount - 1]

	# Convert into a cummultaive distribiutiion
	cumulativeBirthday = countsBirthday
	for experimentCount in range(1, population):
		cumulativeBirthday[experimentCount] += cumulativeBirthday[experimentCount - 1]

	# Convert into a cummultaive distribiutiion
	cumulativeDouble = countsDouble
	for experimentCount in range(1, population):
		cumulativeDouble[experimentCount] += cumulativeDouble[experimentCount - 1]

	# Normalise the values to generate probabilties
	cumulativeBin = [(float(x) / experimentTotal) for x in cumulativeBin]
	cumulativeBirthday = [(float(x) / experimentTotal) for x in cumulativeBirthday]
	cumulativeDouble = [(float(x) / experimentTotal) for x in cumulativeDouble]

	# Plot the resulting PDF
	plt.xscale('log')
	# The next line determins whether the y-axis uses a log scale
	#plt.yscale('log')
	plt.xlabel('First collision')
	plt.ylabel('Frequency')
	plt.title('Selections before first collision with {} items from population of {}'.format(selections, population))
	plt.grid(True)
	expected = sqrt(experimentTotal)
	plt.plot([expected, expected], [0, 1], color='red', linestyle=':')

	# Plot the three lines
	plt.plot(cumulativeBin, color='blue', linestyle='--', label=u'Two bins (1 \xd7 count)')
	plt.plot(cumulativeDouble, color='purple', linestyle='--', label=u'Two bins (2 \xd7 count)')
	plt.plot(cumulativeBirthday, color='green', linestyle='-', label='Birthday')
	plt.legend(loc=4)
	plt.show()

performTests()

