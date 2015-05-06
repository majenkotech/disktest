# disktest
Disk Test program for testing fake flash drives

Compilation results in two programs: `dfill` and `dtest`.

`dfill` fills a device with 64-bit incrementing numbers.  `dtest` reads those numbers and checks they are right.

== Theory of operation ==

The idea behind this program is to fill a flash disk with numbers.  Starting at 0 and counting up the numbers are
written to the raw disk as 64-bit values.  The first 8 bytes of the device should therefore be 

    0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
    
The second 8 bytes should be (little endian):

    0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00

And so on up to the end of the disk.

The testing program then performs the same counting through the disk, but instead of writint to the disk it reads
the numbers from the disk and compares them with what they should be.  As soon as a number is found that is wrong
the test program terminates reporting where the error was, what the expected number was, and what number was
actually read.

== Usage ==

First you need to fill a disk with numbers.  Caution should be used to ensure you get the right device name, since
it's possible to completely distroy your Linux installation if you get the wrong device.  In this example the
flash drive is detected as /dev/sde - use `dmesg` when you insert the drive to find out the device name you should use.

The command must be run as root, so using `sudo` is a good idea:

    $ sudo ./dfill /dev/sde
    
Once that has finished (it will take some time, and it does a basic verify test as it runs) you can test the data:

    $ sudo ./dtest /dev/sde
    
You can expect, if the drive is fake, for it to fail with output similar to:

    Read 7727 MiB
    Read error at 8103247872 (1016755668 != 1012905984)

That was taken from a fake 32GB drive.  It appears to only have about 8GB of real flash in it, and everything above
that gets squashed into the last few meg of the flash.
