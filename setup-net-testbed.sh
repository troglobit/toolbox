#!/bin/bash
#
# Copyright (c) 2014  Joachim Nilsson <troglobit@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Setup very simple testbed for mrouted/pimd/smcroute using:
#  - network namespaces
#  - veth pairs
#  - standard Linux bridge to connect several namespaces
#
# Example:
#          sudo ./setup-net-testbed.sh /path/to/mrouted --some --args
#
BRIDGE1=br-test1
BRIDGE2=br-test2
BRIDGE3=br-test3
routed=$*

# Setup veth port pair, move to unique namespace, and add to bridge
# setup(ns, tap, br, ifaddr)
function setup()
{
        ns=$1
        tap=$2
        peer="br-$2"
        br=$3
        ifaddr=$4

        printf "Creating new veth pair %s<-->%s in %s bridge %s with address %s\n" $tap $peer $ns $br $ifaddr

        # create a port pair
        ip link add $tap type veth peer name br-$tap

        # attach one side to Linux bridge
        brctl addif $br $peer

        # attach the other side to namespace
        ip link set $tap netns $ns

        # set the ports to up
        ip netns exec $ns ip link set dev $tap up
        ip link set dev $peer up

        # setup loopback
        ip netns exec $ns ip addr add 127.0.0.1/8 dev lo
        ip netns exec $ns ip link set dev lo up

        # setup tap address in namespace
#        ip netns exec $ns ip addr add $ifaddr dev $tap
        ip netns exec $ns ifconfig $tap $ifaddr 
}

function cleanup()
{
        ip netns exec ns1 ip link set tap1 netns 1

        ip netns exec ns2 ip link set tap2 netns 1
        ip netns exec ns2 ip link set tap3 netns 1

        ip netns exec ns3 ip link set tap4 netns 1
        ip netns exec ns3 ip link set tap5 netns 1

        ip netns exec ns4 ip link set tap6 netns 1

        ip link delete tap1 type veth
        ip link delete tap2 type veth
        ip link delete tap3 type veth
        ip link delete tap4 type veth
        ip link delete tap5 type veth
        ip link delete tap6 type veth

        ip netns delete ns1
        ip netns delete ns2
        ip netns delete ns3
        ip netns delete ns4
        
        ip link set dev $BRIDGE1 down
        brctl delbr $BRIDGE1

        ip link set dev $BRIDGE2 down
        brctl delbr $BRIDGE2

        ip link set dev $BRIDGE3 down
        brctl delbr $BRIDGE3
	exit
}

trap cleanup SIGHUP SIGINT SIGTERM

# add the namespaces
ip netns add ns1
ip netns add ns2
ip netns add ns3
ip netns add ns4

# create the switches
brctl addbr $BRIDGE1
brctl stp   $BRIDGE1 off
ip link set dev $BRIDGE1 up

brctl addbr $BRIDGE2
brctl stp   $BRIDGE2 off
ip link set dev $BRIDGE2 up

brctl addbr $BRIDGE3
brctl stp   $BRIDGE3 off
ip link set dev $BRIDGE3 up

#### NS1
setup ns1 tap1 $BRIDGE1 172.16.1.1/24

#### NS2
setup ns2 tap2 $BRIDGE1 172.16.1.2/24
setup ns2 tap3 $BRIDGE2 172.16.2.1/24

#### NS3
setup ns3 tap4 $BRIDGE2 172.16.2.2/24
setup ns3 tap5 $BRIDGE3 172.16.3.1/24

#### NS4
setup ns4 tap6 $BRIDGE3 172.16.3.2/24

### Start test
sleep 2
# Ping multicast group with big enough TTL
ip netns exec ns1 ping -I tap1 -t 10 225.1.2.3 &

# Start multicast router daemon
ip netns exec ns2 $routed &
ip netns exec ns3 $routed &

# Wait for daemons to start
sleep 5

# Join multicast group in right-most NS
ip netns exec ns4 mcjoin -i tap6 -r 10 225.1.2.3 &

# Listen for the multicast data
ip netns exec ns4 tcpdump -i tap6 -n -v icmp

# Wait for input
read
cleanup
