import requests
from requests.auth import HTTPBasicAuth
import os
import sys

tests_to_run = []
ac = len(sys.argv)
if ac > 1:
	for av in sys.argv[1:]:
		# print ("to run: test", av)
		tests_to_run.append(int(av))

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def props(cls):   
  return [i for i in cls.__dict__.keys() if i[:1] != '_']
# https://stackoverflow.com/questions/16923898/how-to-get-the-raw-content-of-a-response-in-requests-with-python
# https://stackoverflow.com/questions/9058305/getting-attributes-of-a-class

def printResponse(r, i):

	print ("-------- REQUEST")
	# print (props(r.request))
	print (r.request.method, "on", r.request.url)
	print ()
	for h in r.request.headers:
		print (h + ": " + r.request.headers[h])
	print ()
	print ("Body:", r.request.body)

	print ()
	print ("-------- RESPONSE")
	print ("Status code:", r.status_code)
	print ()
	for h in r.headers:
		print (h + ": " + r.headers[h])
	print ()
	print ("Body:", r.text)

	# print ("-------- ENCODING")
	# print (r.encoding)
	print ("------------------\n")

# PUT -------------------------------------------------------------------------

i = 0;

i += 1
if len(tests_to_run) == 0 or i in tests_to_run:
	r = requests.get('http://localhost:8080/')
	printResponse(r, i)

# ---
i += 1
headers = {'Accept-Language': 'fr'}
if len(tests_to_run) == 0 or i in tests_to_run:
	r = requests.get('http://localhost:8080/test', headers=headers)
	printResponse(r, i)

# ---
i += 1
headers = {'Accept-Language': 'en'}
if len(tests_to_run) == 0 or i in tests_to_run:
	r = requests.get('http://localhost:8080/test', headers=headers)
	printResponse(r, i)

# ---
i += 1
headers = {'Accept-Language': 'fr', 'Accept-Charset': 'utf8'}
if len(tests_to_run) == 0 or i in tests_to_run:
	r = requests.get('http://localhost:8080/test', headers=headers)
	printResponse(r, i)

# ---
i += 1
headers = {'Accept-Charset': 'iso-8859-1'}
if len(tests_to_run) == 0 or i in tests_to_run:
	r = requests.get('http://localhost:8080/test', headers=headers)
	printResponse(r, i)