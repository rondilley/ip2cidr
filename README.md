# IP To CIDR (ip2cidr)

by Ron Dilley <ron.dilley@uberadmin.com>

You can find the latest information on ip2cidr [here](http://www.uberadmin.com/Projects/ip2cidr/ "IP To CIDR")

## What is IP To CIDR (ip2cidr)?

IP To CIDR is a small and fast utility for consolidating lists of IP addresses to
CIDR blocks.  It is pretty fast and was built to work with 10M+ IP lists.  If you
have a large IP address list and you want to summarize to network ranges based on
how densely populated the CIDR is, this tool may be able to help.

## Why use it?

I built this tool to solve a specific problem with large lists of known bad IP
addresses that needed to be loaded into an ACL for blocking.  Unfortunately, the
firewall I was using only supported a limited number of CIDRs.  To make it work,
I needed to consolidate IP addresses into CIDR blocks based on the number of
IP addresses in a given CIDR.

## Implementation

IP To CIDR has a simple command lines interface.  In it's
simplest form, pass a text file as an argument and the output
will be all of the lines that are not recognized, any consolidated
CIDRs and whatever IPs are left over (converted to /32 notation).

```
ip2cidr v0.5 [Jul 21 2023 - 21:50:24]

syntax: ip2cidr [options] filename [filename ...]
 -d|--debug (0-9)       enable debugging info
 -h|--help              this info
 -H|--hbit {bits}       max network bits (default: 31)
 -l|--lbit {bits}       min network bits (default: 24)
 -t|--thold {percent}   consolidation threshold (default: 51)
 -v|--version           display version information
 -V|--verbose           show additional information
 filename               one or more files to process, use '-' to read from stdin
```

The debug option is most useful when the tool is compiled
with the --ENABLE-DEBUG switch.

ip2cidr processes and consolidates IPv4 addresses in octet and CIDR format.  IPv6
and malformed IP addresses are not consolidated but are passed on to STDOUT without
any changes.  CIDR blocks that are larger than the minimum mask (see -l|--lbit) are
not consolidated but are passed on to STDOUT without any changes.  If a CIDR block
does not start with the first address (e.g., the host id is non-zero), ip2cidr will
not consolidate and instead will pass the CIDR block to STDOUT without any changes.
IPv4 CIDR blocks with /32 will be converted to a host IP address in octet format
and will be consolidated.

Use the verbose switch (see -V|--verbose) to get more information about which entries
in a file are consolidated and which are passed to STDOUT without any change.

A typical run of ip2cidr is to pass the IP file as an
argument.

```
% ./ip2cidr ip_list.txt > consolidated_ip_list.txt
```

Or you can pass the IP addresses via STDIN.

```
% cat ip_list.txt | ./ip2cidr > consolidated_ip_list.txt
```

You control the minimum and maximum bitmask along with the percentage of IPs
in a particular CIDR using command line arguments as indicated in the --help.

To consolidate CIDRs starting at /20 and ending at /30 when at least 75% of
the IP addresses exist in a given CIDR, use the following arguments:

```
% ./ip2cidr -l 20 -H 30 -t 75 ip_list.txt > consolidated_ip_list_20_to_30_at_75.txt
```

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
