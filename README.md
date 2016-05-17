# birthdayparadox
Exploring collisions in a finite numerical world

## Bins

The classic birthday paradox configuration assumes balls are added to a single set of bins. In theory (so we thought), the likelihood of a collision is halved when the values are split across two sets of bins. The script generates random numbers and places them in bins, in an attempt to test this empirically.

And the results are in...

Log y-axis, linear x-axis. Red dotted vertical line indicates sqrt(population):

![alt text](https://raw.githubusercontent.com/llewelld/birthdayparadox/master/results/result.png "Results, log-x, linear-y")

And results with a log y-axis:

![alt text](https://raw.githubusercontent.com/llewelld/birthdayparadox/master/results/log-log.png "Results, log-x, log-y")

## BabyDES

The progenator of the topic of bins was that of block ciphers. How do multiple bins affect the complexity of finding collisions? Exploring this empirically, even with a cryptographically weak cipher like DES, requiress a lot of work. Too much work.

So here you can also find BabyDES. BabyDES is a Feistel Structure block cipher mutable on block and key size. It can be configured to use any key length with any even block size.

As you might imagine, BabyDES is still in its infancy. The current implementation uses [KISS](https://github.com/cmcqueen/simplerandom) as its pseudo-random function. KISS is [not crypographyically secure](http://eprint.iacr.org/2011/007.pdf), so I'll probably change it to use something else in the future.

Implementations are provided in both Python and C for validation against each other. Simple initial benchmarking suggests the C implementation is over three times faster than the Python implementation (tested using a 128-bit block size). The next step will be to generate some graphs of the ciphertext distribution and compare against an expected collision probability distribution, since this is what we're really interested in.
