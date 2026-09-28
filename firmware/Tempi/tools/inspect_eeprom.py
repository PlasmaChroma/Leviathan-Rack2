#!/usr/bin/env python3
"""Read a 1024-byte Tempi-format EEPROM image and emit states as JSON. No writes."""
import argparse,json
from pathlib import Path
from models import decode_state

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('image',type=Path)
    ap.add_argument('--state',type=int,choices=range(64),help='Only this zero-based state')
    args=ap.parse_args()
    try:
        data=args.image.read_bytes()
        if len(data)!=1024:raise ValueError(f'Expected 1024 bytes, got {len(data)}')
        states=[decode_state(data,s) for s in ([args.state] if args.state is not None else range(64))]
        print(json.dumps({'source':str(args.image),'warning':'Values are only as trustworthy as the input dump. Synthetic images are not hardware captures.','states':states},indent=2))
    except (OSError,ValueError) as e:ap.error(str(e))
if __name__=='__main__':main()
