vif cleanup
===========

Linux 5.4 is slightly broken wrt IP multicast.

When starting up a multicast routing daemon for the first time
you may get the following odd message:

    12:23:27.806 setsockopt MRT_ADD_VIF on vif 2: Address already in use

So you debug it:

    $ cat /proc/net/ip_mr_vif 
    Interface      BytesIn  PktsIn  BytesOut PktsOut Flags Local    Remote
     2 lxcbr0            0       0         0       0 08000 0103000A 00000000

???

You check running processes for any instance of pimd, mrouted, pimdd, smcrouted,
or any of the other multicast misfits out there ... alas no sail ...

???

It's not until you try a newer kernel that you realize it's a bug, and that's
where this little tool comes in.  It clears the VIF table for you.

 /Joachim

