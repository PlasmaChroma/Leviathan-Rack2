"""Locate the supplied firmware without depending on the analyst's workspace."""
from pathlib import Path
import os
NAME = 'Data_Bender_v1_4_7.bin'
def firmware_path():
    override = os.environ.get('DATA_BENDER_FIRMWARE')
    if override:
        p = Path(override).expanduser().resolve()
        if p.is_file():
            return p
        raise FileNotFoundError(f'DATA_BENDER_FIRMWARE does not identify a file: {p}')
    for parent in Path(__file__).resolve().parents:
        for directory in ['', 'upload', 'inputs', 'input', 'firmware', 'originals', 'source']:
            p = parent / directory / NAME
            if p.is_file():
                return p
    raise FileNotFoundError('Set DATA_BENDER_FIRMWARE to the path of the uploaded ' + NAME)
