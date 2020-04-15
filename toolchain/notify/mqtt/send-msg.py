#!/usr/bin/env python3

from argparse import ArgumentParser
from paho.mqtt.publish import single as publish_single
from os import listdir
from os.path import dirname, abspath
from time import time

SRC_DIR = dirname(abspath(__file__))

args = ArgumentParser('MqttPub')

args.add_argument('topic', help='Topic to publish to')
args.add_argument('payload', help='Payload to send')
args.add_argument('--username', help='Username for auth')
args.add_argument('--password', help='Password for auth')

args = args.parse_args()

USERNAME = args.username
PASSWORD = args.password
TOPIC    = args.topic
PAYLOAD  = args.payload

CERTS_DIR = '/etc/ssl/certs/'
ALL_CERTS = [CERTS_DIR + c for c in listdir(CERTS_DIR) if c.endswith('.pem')]
TLS_CONFIG = {
    'ca_certs': SRC_DIR + '/DST_Root_CA_X3.pem'
}
print(TLS_CONFIG)

publish_single(
        TOPIC, PAYLOAD,
        retain=True,
        hostname='mq.birchy.dev', port=8883,
        auth={'username': USERNAME, 'password': PASSWORD},
        tls=TLS_CONFIG
        )
