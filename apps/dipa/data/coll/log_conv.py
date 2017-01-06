#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Author: Xin Wang
# @Date:   2015-06-22 14:28:59


import time
import sys

'''
Log Converter
converts wilab testbed log to Cooja log format
'''

class TestbedLogConverter(object):

    def __init__(self, filename):
        try:
            content = open(filename).read().replace(r'\0', '').split('\n')
            print('>> Reading file {}'.format(filename))
        except Exception as e:
            print('>> Open file error:', e)
        # remove header
        self.output_filename = '{}.log'.format(filename)
        self.content = content[1:]
        self.output = []
        self.base_timestamp = None
        self.parse()
        self.write_to_file()

    def convert_time_format(self, time_str):
        '''converts time '2015-06-19 18:12:29' to '00:05.754' (relateive time)
        '''
        if self.base_timestamp is None:
            self.base_timestamp = self.get_timestamp(time_str)
        timestamp = self.get_timestamp(time_str)
        diff = timestamp - self.base_timestamp
        minute = int(diff / 60)
        second = int(diff % 60)
        return '{:02d}:{:02d}.000'.format(minute, second)

    def get_timestamp(self, time_str):
        '''gets timestamp from string like '2015-06-19 18:12:29'
        @return timestamp
        '''
        res = time.strptime(time_str, '%Y-%m-%d %H:%M:%S')
        return time.mktime(res)

    def make_message(self, line):
        '''generates log message like 'TIME ID:NODEID MSG'
        '''
	
        msg, ev_time, node_id, mtime, msg_seqno = line.split('\t')
        ev_time = self.convert_time_format(ev_time)
        msg = msg.replace(r'\n', '')
        return '{}\tID:{}\t{}'.format(ev_time, node_id, msg)

    def parse(self):
        '''parses each line
        '''
        for line in self.content:
            if len(line) == 0:
                continue
            log_item = self.make_message(line)
            self.output.append(log_item)

    def write_to_file(self):
        '''writes output into file
        '''
        print '>> Writing to {}'.format(self.output_filename)
        with open(self.output_filename, 'w') as fp:
            fp.write('\n'.join(self.output))



if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: python log_converter.py <LOG_FILENAME>')
        exit(1)
    adapter = TestbedLogConverter(sys.argv[1])
