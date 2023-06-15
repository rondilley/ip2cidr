# IP To CIDR (ip2cidr)

by Ron Dilley <ron.dilley@uberadmin.com>

You can find the latest information on ip2cidr [here](http://www.uberadmin.com/Projects/ip2cidr/ "IP To CIDR")

## What is IP To CIDR (ip2cidr)?

IP To CIDR is a small and fast XXX

## Why use it?

I built this tool to solve a XXX

## Implementation

IP To CIDR has a simple command lines interface.  In it's
simplest form, pass a text file as an argument and the output
will be XXX

```
ip2cidr v0.1 [Jun 15 2023 - 12:51:44]

syntax: ip2cidr [options] filename [filename ...]
 -d|--debug (0-9)       enable debugging info
 -h|--help              this info
 -H|--hbit {bits}       max network bits (default: 31)
 -l|--lbit {bits}       min network bits (default: 24)
 -t|--thold {percent}   consolidation threshold (default: 51)
 -v|--version           display version information
 filename               one or more files to process, use '-' to read from stdin
```

The debug option is most useful when the tool is compiled
with the --ENABLE-DEBUG switch.

A typical run of iop2cidr is to pass the IP file as an
argument and XXX


## Security Implications

Assume that there are errors in the ip2cidr source that
would allow a specially crafted logs to allow an attacker
to exploit this to gain access to the computer that it is
running on!!!  Don't trust this software and install and use
it at your own risk.

## Bugs

I am not a programmer by any stretch of the imagination.  I
have attempted to remove the obvious bugs and other
programmer related errors but please keep in mind the first
sentence.  If you find an issue with code, please send me
an e-mail with details and I will be happy to look into
it.

Ron Dilley
ron.dilley@uberadmin.com
