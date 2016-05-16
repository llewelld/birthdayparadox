# birthdayparadox
Generate empirical probabilities for collisions across two bins

The classic birthday paradox configuration assumes balls are added to a single set of bins. In theory (so we thought), the likelihood of a collision is halved when the values are split across two sets of bins. The script generates random numbers and places them in bins, in an attempt to test this empirically.

And the results are in...

Log y-axis, linear x-axis. Red dotted vertical line indicates sqrt(population):

![alt text](https://raw.githubusercontent.com/llewelld/birthdayparadox/master/results/result.png "Results, log-x, linear-y")

And results with a log y-axis:

![alt text](https://raw.githubusercontent.com/llewelld/birthdayparadox/master/results/log-log.png "Results, log-x, log-y")
