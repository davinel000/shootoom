"""Verify archived source bytes against the audit manifest; optionally against cloud source."""
import argparse
import hashlib
import json
from pathlib import Path

repo=Path(__file__).resolve().parents[2]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source-root',type=Path)
args=parser.parse_args()
manifest=json.loads((repo/'docs/audit/preserved-files.json').read_text(encoding='utf-8'))
errors=[]
for row in manifest:
    paths=[repo/row['destination']]
    if args.source_root: paths.append(args.source_root/row['source'])
    for path in paths:
        if not path.is_file(): errors.append(f'Missing: {path}'); continue
        raw=path.read_bytes()
        if len(raw)!=row['bytes'] or hashlib.sha256(raw).hexdigest()!=row['sha256']:
            errors.append(f'Changed: {path}')
if errors: raise SystemExit('\n'.join(errors))
print(f'Verified {len(manifest)} preserved files' + (' and source copies' if args.source_root else ''))
