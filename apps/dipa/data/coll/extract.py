#!/usr/bin/env python

pkts_recvd = {};
bad_crc = {};
pkt_lost = {};
bits_lost = {};

def init ():
	pkts_recvd.clear ();
	bad_crc.clear ();
	pkt_lost.clear ();
	bits_lost.clear ();

def update_data (d):
	pkts_recvd[d[0]] = d[3];
	bad_crc[d[0]]= d[4];
	pkt_lost[d[0]] = d[5];
	bits_lost[d[0]] = d[6];
	
def write_data (fname):
	ids = list (pkts_recvd.keys ());
	fname = fname + '.csv';
	try:
		g = open (fname, 'w');
	except IOError:
		print ('Could not open csv file ', fname);
		return;
		
	with g:
		for i in ids:
			s = i + pkts_recvd[i] + bad_crc[i] + pkt_lost[i] + bits_lost[i] + '\n';
			g.write (s);
			
	try:
		g.close ();
	except IOError:
		return;
			 
def extract_and_write (fname):
	init ();
	try:
		f = open (fname, 'r');
	except IOError:
		print ('Could not open file ', fname);
		return;
		
	data = f.readlines();
	for l in range(0, len(data)):
		if data[l].find ('node_id:') == -1 and data[l].find ('Contiki') == -1 and data[l].find ('Starting') == -1 and data[l].find ('Rime') == -1 and data[l].find ('MAC') == -1 and data[l].find ('channel') == -1:
			d = data[l].split();			
			update_data (d[2:]);
	
	write_data (fname);
	try:
		f.close ();
	except IOError:
		return;

def main(args):
	for a in args:
		extract_and_write (a);
    
	return 0

if __name__ == '__main__':
    import sys    
    sys.exit(main(sys.argv[1:]))
