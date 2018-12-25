#!/usr/bin/python
import argparse
import datetime
import struct
import sys

import serial

parser = argparse.ArgumentParser(description='receives the data from a climate logger via serial (https://github.com/martin2250/EnvironmentalLogger)',
                                 epilog='to import the data into influxDB, use this command: "influx -host <host> -import -path=<fout>"')
parser.add_argument('port', help='which serial port to use')
parser.add_argument('fout', help='output file name, \'-\' = stdio')
parser.add_argument('-f', '--format', help='output file format',
                    choices=['influx', 'tsv'], default='tsv')
parser.add_argument('--measurement',
                    help='measurement name and tags (only for influx output format)', default='test')
parser.add_argument('--database',
                    help='database (only for influx output format)', default='<DB>')
parser.add_argument('--tag', action='append', nargs=2,
                    help='additional tags (only for influx output format)')
parser.add_argument('--timezone', type=float, default=0,
                    help='time zone (+/-XX) (only for influx output format)')
args = parser.parse_args()


def nofunc(*args):
	pass


def start_influx(file):
	global args
	print('import with "influx -import -path=<fout> -precision=s -host=<IP>"')
	file.write(f'# DML\n# CONTEXT-DATABASE: {args.database}\n')


def write_influx(file, date, temperature, humidity, battery=None):
	global args
	file.write(args.measurement)
	for tag in args.tag:
		file.write(f',{tag[0]}={tag[1]}')
	file.write(f' temperature={temperature:0.2f},humidity={humidity:0.2f}')
	if battery is not None:
		file.write(f',bat={battery:0.1f}')
	file.write(
            f' {(date - datetime.timedelta(hours=args.timezone)).timestamp():0.0f}\n')


def start_tsv(file):
	file.write('# time\ttemperature (celsius)\thumidity (rH)\tbattery (volt)\n')


def write_tsv(file, date, temperature, humidity, battery=None):
	file.write(f'{date}\t{temperature:0.2f}\t{humidity:0.2f}')
	if battery is not None:
		file.write(f'\t{battery:0.1f}')
	file.write('\n')


file_start = nofunc
file_write = nofunc
file_end = nofunc

if args.format == 'influx':
	file_start = start_influx
	file_write = write_influx
elif args.format == 'tsv':
	file_start = start_tsv
	file_write = write_tsv


with serial.Serial(args.port, 250000) as port, sys.stdout if args.fout == '-' else open(args.fout, 'w') as fout:
	port.reset_input_buffer()
	print(f'opened {port.name}, waiting for start sequence')

	while True:
		x = port.read()
		if x[0] == 0xFF:
			break
		print(f'start sequence: {x[0]}')
		# elif x[0] != 0x00:
		#	print(f'invalid start sequence (received {x[0]})')

	pagecount, = struct.unpack('>H', port.read(2))

	print(f'received start sequence, {pagecount} pages expected')

	port.timeout = 1.0
	pagecounter = 0

	file_start(fout)

	while pagecounter < pagecount:
		page = port.read(128)

		if len(page) != 128:
			print(f'\nwrong number of bytes received: {len(page)}')
			exit(1)

		pagecounter += 1
		print(f'[{pagecounter}/{pagecount}]', end='')

		samples = page[0]
		interval_minutes = page[1] & 0x3F
		battery_raw = (page[1] >> 6) | ((page[2] & 0x0F) << 2)
		battery = battery_raw / 10

		month = page[2] >> 4
		year = page[3] + 2000
		day = page[4]
		hour = page[5]
		minute = page[6]
		second = page[7]

		date = datetime.datetime(year, month, day, hour, minute, second)
		print(' ', date, end='\r')

		for i in range(samples):
			temperature_raw = (page[8 + i * 3] << 4) | (page[9 + i * 3] >> 4)
			humidity_raw = ((page[9 + i * 3] & 0x0F) << 8) | page[10 + i * 3]

			temperature = temperature_raw * 165 / 4096 - 40
			humidity = humidity_raw * 100 / 4096

			file_write(fout, date + datetime.timedelta(minutes=i * interval_minutes),
			           temperature, humidity, battery if i == 0 else None)
	file_end(fout)
print('\ndone')
