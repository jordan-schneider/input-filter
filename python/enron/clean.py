import pandas as pd
import re
import itertools

skip_tokens = ['Message-ID:','Date:','From:','To:','Subject:','Mime-Version:','Content-Type:','Content-Transfer-Encoding:','X-From:','X-To:','X-cc:','X-bcc:','X-Folder:','X-Origin:','X-FileName:', '----------------------', '-----Original Message-----', 'Sent by:', 'cc:', 'bcc:', 'Cc:','Bcc:', 'CC:','BCC:', 'Enron@ENRON', 'Enron@Enron', 'Enron@EnronXGate', 'ECT@ECT']

noise_re = '[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+|http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\(\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+|<.*?>|\[.*?\]|&nbsp|[_=~-]{3,}|>'

def skip_line(line):
	for token in skip_tokens:
		if token in line:
			return True
	return False

emails = pd.read_csv('emails.csv')
for message in emails.message:
	lines = message.split('\n')
	lines = [line.strip() for line in lines if not skip_line(line)]
	lines = [re.sub(noise_re, '', line).strip() for line in lines]
	lines = list(itertools.chain.from_iterable([line.split('.') for line in lines]))
	lines = [line.strip() for line in lines]
	lines = [line for line in lines if line is not '']
	print(*lines, sep='\n')
