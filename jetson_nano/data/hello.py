#!/usr/bin/env python3
# coding: utf-8

import sys

from jetson_voice import ASR, AudioInput, ConfigArgParser, list_audio_devices

#!/usr/bin/env python3
# coding: utf-8
import smtplib
def send_email(sender,receiver,message):
    smtp_server = 'mail.ntu.edu.tw'
    smtp_server_port = 587
    smtp_server_account = 'enter_your_mail'
    smtp_server_password = 'enter_your_password'
    server = smtplib.SMTP(smtp_server,smtp_server_port)
    server_res = server.ehlo()
    print(f'res==> {server_res}')
    smtp_ttls = server.starttls()
    print(f'start tls ==> {smtp_ttls}')
    smtp_login = server.login(smtp_server_account,smtp_server_password)
    print(f'SMTP login ==> {smtp_login}')
		
    status = server.sendmail(sender,receiver, message)
    if not status:
        print('Mail sent.')
    else:
        print(f'Mail failed with error: {status}')    
message = """\
Subject: Command detected

This message is sent from Python:"""
    
    
parser = ConfigArgParser()

parser.add_argument('--model', default='quartznet', type=str, help='path to model, service name, or json config file')
parser.add_argument('--wav', default=None, type=str, help='path to input wav/ogg/flac file')
parser.add_argument('--mic', default=None, type=str, help='device name or number of input microphone')
parser.add_argument('--list-devices', action='store_true', help='list audio input devices')

args = parser.parse_args()
print(args)
    
# list audio devices
if args.list_devices:
    list_audio_devices()
    sys.exit()
    
# load the model
asr = ASR(args.model)

# create the audio input stream
stream = AudioInput(wav=args.wav, mic=args.mic, 
                     sample_rate=asr.sample_rate, 
                     chunk_size=asr.chunk_size)

# run transcription
for samples in stream:
    results = asr(samples)
    
    if asr.classification:
        # print(f"class '{results[0]}' ({results[1]:.3f})")
        if results[1]>0.8:
            print(f"Keyword {results[0]} detected with threshold value: {results[1]:.3f}",'\n',"Executing response...")

            send_email("b08901125@ntu.edu.tw","b08901125@g.ntu.edu.tw",message)
    else:
        for transcript in results:
            print(transcript['text'])
            
            if transcript['end']:
                print('')
                
print('\naudio stream closed.')
    
