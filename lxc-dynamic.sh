#!/bin/bash
#
# Create an LXC container dynamically to house a routing daemon with one
# or two veth interfaces, 'left' and 'right', that can then be bridged
# outside the lxc to connect a pair of lxc's in a chain.  This can then
# be used to test routing protocols like RIP, DVMRP, PIM-SM.
#

if [ $# -lt 3 ]; then
        echo "Usage: $0 path/to/CONTAINER IPADDR BRIDGE [IPADDR BRIDGE]"
        exit 1
fi

if [ `whoami` != "root" ]; then
        echo "You need to run this as root, try: sudo $* ..."
        exit 1
fi

DIR=$1
NAME=`basename $1`
PWD=`pwd`

if [ $# -gt 1 ]; then
        LEFT=${NAME}-left
        LEFT_ADDR=$2
        LEFT_BRIDGE=$3
fi

if [ $# -gt 3 ]; then
        RIGHT=${NAME}-right
        RIGHT_ADDR=$4
        RIGHT_BRIDGE=$5
fi


# Create LXC root file system
mkdir -p $DIR/rootfs
cd $DIR/rootfs

# Setup basic Linux FHS structure
mkdir bin
mkdir home
mkdir lib
mkdir lib64
mkdir opt
mkdir proc
mkdir root
mkdir sbin
mkdir sys
mkdir tmp
mkdir usr
mkdir var

# Populate from host (suboptimal)
cp -a /etc .
cp -a /dev .
rm -fr dev/pts
mkdir dev/pts
rm -fr dev/shm
mkdir dev/shm

# Create LXC configuration
cd ..
cat << EOF > lxc.conf
# LXC basics and rootfs
lxc.utsname     = $NAME
lxc.pts         = 1024
lxc.rootfs      = $DIR/rootfs

lxc.mount.entry = /lib lib none ro,bind 0 0
lxc.mount.entry = /bin bin none ro,bind 0 0
lxc.mount.entry = /usr usr none ro,bind 0 0
lxc.mount.entry = /sbin sbin none ro,bind 0 0
lxc.mount.entry = /lib64 lib64 none ro,bind 0 0
lxc.mount.entry = proc /proc proc nodev,noexec,nosuid 0 0
lxc.mount.entry = tmpfs /dev/shm tmpfs  defaults 0 0

EOF

# If arguments for first (left) interface are given
if [ $# -gt 1 ]; then
        cat << EOF >> lxc.conf
# LXC networking
lxc.network.type       = veth
# Interface name in the host
lxc.network.veth.pair  = $LEFT
# Interface name in the container
lxc.network.name       = left
lxc.network.flags      = up
lxc.network.link       = $LEFT_BRIDGE
# lxc.network.hwaddr     = Automatically generated if left out
lxc.network.ipv4       = $LEFT_ADDR

EOF
fi

# If arguments for a second (right) interface are given
if [ $# -gt 1 ]; then
        cat << EOF >> lxc.conf
lxc.network.type       = veth
# Interface name in the host
lxc.network.veth.pair  = $RIGHT
# Interface name in the container
lxc.network.name       = right
lxc.network.flags      = up
lxc.network.link       = $RIGHT_BRIDGE
# lxc.network.hwaddr     = Automatically generated if left out
lxc.network.ipv4       = $RIGHT_ADDR

EOF
fi

# print startup instruction
cd ..
echo "Linux Container (LXC) created, start it as:"
echo "      cd $PWD"
echo "      lxc-execute -n $DIR -f $DIR/lxc.conf -- /sbin/mrouted"
echo
