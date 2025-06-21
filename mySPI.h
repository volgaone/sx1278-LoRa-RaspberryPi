/*D
This function returns a handle for the SPI device on the channel.
Data will be transferred at baud bits per second.  The flags may
be used to modify the default behaviour of 4-wire operation, mode 0,
active low chip select.

The Pi has two SPI peripherals: main and auxiliary.

The main SPI has two chip selects (channels), the auxiliary has
three.

The auxiliary SPI is available on all models but the A and B.

The GPIO used are given in the following table.

         @ MISO @ MOSI @ SCLK @ CE0 @ CE1 @ CE2
Main SPI @    9 @   10 @   11 @   8 @   7 @   -
Aux SPI  @   19 @   20 @   21 @  18 @  17 @  16

. .
 spiChan: 0-1 (0-2 for the auxiliary SPI)
    baud: 32K-125M (values above 30M are unlikely to work)
spiFlags: see below
. .

Returns a handle (>=0) if OK, otherwise PI_BAD_SPI_CHANNEL,
PI_BAD_SPI_SPEED, PI_BAD_FLAGS, PI_NO_AUX_SPI, or PI_SPI_OPEN_FAILED.

spiFlags consists of the least significant 22 bits.

. .
21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 b  b  b  b  b  b  R  T  n  n  n  n  W  A u2 u1 u0 p2 p1 p0  m  m
. .

mm defines the SPI mode.

Warning: modes 1 and 3 do not appear to work on the auxiliary SPI.

. .
Mode POL PHA
 0    0   0
 1    0   1
 2    1   0
 3    1   1
. .

px is 0 if CEx is active low (default) and 1 for active high.

ux is 0 if the CEx GPIO is reserved for SPI (default) and 1 otherwise.

A is 0 for the main SPI, 1 for the auxiliary SPI.

W is 0 if the device is not 3-wire, 1 if the device is 3-wire.  Main
SPI only.

nnnn defines the number of bytes (0-15) to write before switching
the MOSI line to MISO to read data.  This field is ignored
if W is not set.  Main SPI only.

T is 1 if the least significant bit is transmitted on MOSI first, the
default (0) shifts the most significant bit out first.  Auxiliary SPI
only.

R is 1 if the least significant bit is received on MISO first, the
default (0) receives the most significant bit first.  Auxiliary SPI
only.

bbbbbb defines the word size in bits (0-32).  The default (0)
sets 8 bits per word.  Auxiliary SPI only.

The [*spiRead*], [*spiWrite*], and [*spiXfer*] functions
transfer data packed into 1, 2, or 4 bytes according to
the word size in bits.

For bits 1-8 there will be one byte per word. 
For bits 9-16 there will be two bytes per word. 
For bits 17-32 there will be four bytes per word.

Multi-byte transfers are made in least significant byte first order.

E.g. to transfer 32 11-bit words buf should contain 64 bytes
and count should be 64.

E.g. to transfer the 14 bit value 0x1ABC send the bytes 0xBC followed
by 0x1A.

The other bits in flags should be set to zero.
D*/
int spiOpen(unsigned spiChan, unsigned baud, unsigned spiFlags);


/*D
This function transfers count bytes of data from txBuf to the SPI
device associated with the handle.  Simultaneously count bytes of
data are read from the device and placed in rxBuf.

. .
handle: >=0, as returned by a call to [*spiOpen*]
 txBuf: the data bytes to write
 rxBuf: the received data bytes
 count: the number of bytes to transfer
. .

Returns the number of bytes transferred if OK, otherwise
PI_BAD_HANDLE, PI_BAD_SPI_COUNT, or PI_SPI_XFER_FAILED.
D*/
int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned count);
/*D
This functions closes the SPI device identified by the handle.

. .
handle: >=0, as returned by a call to [*spiOpen*]
. .

Returns 0 if OK, otherwise PI_BAD_HANDLE.
D*/
int spiClose(unsigned handle);