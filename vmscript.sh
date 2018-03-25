#!/bin/bash
# build an LXC container based on your current filesystem
# mounted read-only

if [ $# -ne 1 ]
then
        echo "Error: directory missing"
        echo "Usage: $0 directory-name"
        exit 1
fi

if [ `whoami` != "root" ]; then
        echo "You need to run this as root, try: sudo $* ..."
        exit 1
fi

# create rootfs
mkdir -p $1/rootfs
cd $1/rootfs

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
cp -a /etc .
cp -a /dev .
rm -fr dev/pts
mkdir dev/pts
rm -fr dev/shm
mkdir dev/shm

cd ..

# create lxc configuration
cat > lxc.conf << EOF
lxc.utsname = $1
lxc.rootfs = $1/rootfs
lxc.mount.entry=/lib lib none ro,bind 0 0
lxc.mount.entry=/bin bin none ro,bind 0 0
lxc.mount.entry=/usr usr none ro,bind 0 0
lxc.mount.entry=/sbin sbin none ro,bind 0 0
lxc.mount.entry=/lib64 lib64 none ro,bind 0 0
lxc.mount.entry=proc /proc proc nodev,noexec,nosuid 0 0
lxc.mount.entry=tmpfs /dev/shm tmpfs  defaults 0 0
lxc.pts=1024
EOF

# print startup instruction
cd ..
PWD=`pwd`
echo "Virtual machine created, start it as:"
echo "     # cd $PWD"
echo "     # lxc-execute -n $1 -f $1/lxc.conf -- /bin/bash"
echo
