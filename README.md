Destructive Interference based Power Adaptation (DIPA)
======================================================

Destructive Interference based Power Adaptation (DIPA) is a performance enhancement algorithm for Constructive Interference (CI) on IEEE 802.15.4 nodes. This algorithm is part of our paper: "Murphy loves CI: unfolding and improving constructive interference in WSNs", in The 35th Annual IEEE International Conference on Computer Communications (INFOCOM 2016).

The constructive interference (CI) phenomenon occurs when two or more nodes transmit the same data concurrently, which makes the signals superpose. Hence, receivers can decode the packet successfully with high probability due to, supposedly, the increased signal power at the receivers [1]. 

While CI itself is supposed to be reliable, it is apparently not always the case as shown in [2]. Therefore, we develop DIPA,  an energy-efficient and distributed algorithm, that adapts transmission power to improve the performance of CI. Since CI based protocols cannot have an explicit acknowledgment packet, we make use of destructive interference on a designated byte to provide feedback. We leverage this feedback to adapt transmission powers. Refer to [2] for further details.

This code is based on FS-LWB implementation on Contiki-3.x by Chayan Sarkar. His code is availabe at https://github.com/csarkar/tailored-lwb/

You may cite DIPA in one of the following ways:

V. S. Rao, M. Koppal, R. V. Prasad, T. V. Prabhakar, C. Sarkar, and I. Niemegeers, "Murphy loves CI: unfolding and improving constructive interference in WSNs," in The 35th Annual IEEE International Conference on Computer Communications (INFOCOM 2016), San Francisco, USA, April 2016.

Bibtex:

     @inproceedings{rao2016murphy,     
     title={Murphy loves CI: Unfolding and improving constructive interference in WSNs},     
     author={Rao, Vijay S and Koppal, M and Prasad, R Venkatesha and Prabhakar, TV and Sarkar, C and Niemegeers, I},     
     booktitle={The 35th Annual IEEE International Conference on Computer Communications, IEEE INFOCOM 2016},     
     pages={1--9},      
     year={2016},      
     organization={IEEE}     
     }

References:

[1] F. Ferrari, M. Zimmerling, L. Thiele, and O. Saukh, "Efficient network flooding and time synchronization with Glossy," in IPSN 2011.

[2] V. S. Rao, M. Koppal, R. V. Prasad, T. V. Prabhakar, C. Sarkar, and I. Niemegeers, "Murphy loves CI: unfolding and improving constructive interference in WSNs," in The 35th Annual IEEE International Conference on Computer Communications (INFOCOM 2016), San Francisco, USA, April 2016.
