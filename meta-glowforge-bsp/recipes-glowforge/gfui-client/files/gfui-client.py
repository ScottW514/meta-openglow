#!/usr/bin/python3
"""
(C) Copyright 2020
Scott Wiederhold, s.e.wiederhold@gmail.com
https://community.openglow.org

SPDX-License-Identifier:    MIT
"""
import argparse
from daemonize import Daemonize
import logging
from pathlib import Path
import shutil

from gfutilities.configuration import *
from gfutilities import GFUIService
from gfhardware import Machine

parser = argparse.ArgumentParser(description='GF UI Client')
parser.add_argument('-d', '--daemonize', action='store_true', help='Run as daemon')
args = parser.parse_args()


if not Path('/data/etc/gfui-client.conf').is_file():
    shutil.copyfile('/etc/gfui-client.conf.sample', '/data/etc/gfui-client.conf')

parse('/data/etc/gfui-client.conf')

logging.basicConfig(format='(%(levelname)s) %(module)s:%(funcName)s %(message)s')
logger = logging.getLogger("openglow")
logger.setLevel(logging.DEBUG)

keep_fds = []

if get_cfg('LOGGING.FILE'):
    Path(get_cfg('LOGGING.FILE')).parent.mkdir(parents=True, exist_ok=True)
    fh = logging.FileHandler(get_cfg('LOGGING.FILE'))
    fh.setLevel(log_level(get_cfg('LOGGING.LEVEL')))
    fh.setFormatter(logging.Formatter('%(asctime)s (%(levelname)s) %(module)s:%(funcName)s %(message)s'))
    logger.addHandler(fh)
    keep_fds = [fh.stream.fileno()]


def main():
    service = GFUIService(Machine())
    service.connect()
    service.run()


if args.daemonize:
    daemon = Daemonize(app="gfui-client", pid="/var/run/gfui-client.pid",
                       action=main, keep_fds=keep_fds, logger=logger)
    daemon.start()
else:
    main()
