"""Compile/package Uno 0.1.0 without opening any serial port or uploading."""
import argparse
import hashlib
import json
import subprocess
import tempfile
import zipfile
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--cli',default='arduino-cli')
p.add_argument('--config',type=Path)
a=p.parse_args()
root=Path(__file__).resolve().parents[2]
version=root/'firmware/uno/0.1.0'
out=root/'releases/uno-0.1.0'
out.mkdir(parents=True,exist_ok=True)
base=[a.cli]+(['--config-file',str(a.config)] if a.config else [])
with tempfile.TemporaryDirectory(prefix='sutum-uno-') as temp:
    command=base+['compile','--fqbn','arduino:avr:uno','--warnings','all',
                  '--build-path',temp,'--output-dir',str(out),str(version/'LaserShow')]
    result=subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    log=result.stdout.decode('utf-8',errors='replace').replace('\r\r\n','\n').replace('\r\n','\n')
    # Keep shared logs independent of the owner's absolute directories.
    for path,label in [(str(root),'<REPO>'),(temp,'<BUILD>'),(str(Path.home()),'<HOME>')]:
        log=log.replace(path,label).replace(path.replace('\\','/'),label)
    (out/'build-output.txt').write_text(log,encoding='utf-8')
    print(log)
    if result.returncode: raise SystemExit(result.returncode)

with zipfile.ZipFile(out/'Sutum-Uno-0.1.0-source.zip','w',zipfile.ZIP_DEFLATED) as z:
    for f in sorted(version.rglob('*')):
        if f.is_file():z.write(f,Path('Sutum-Uno-0.1.0')/f.relative_to(version))
def record(f):
    b=f.read_bytes()
    return {'path':f.relative_to(root).as_posix(),'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()}
files=[record(f) for f in sorted(version.rglob('*')) if f.is_file()]
files += [record(out/name) for name in ['LaserShow.ino.hex','Sutum-Uno-0.1.0-source.zip']]
(out/'SHA256.json').write_text(json.dumps({'version':'0.1.0','fqbn':'arduino:avr:uno','files':files},indent=2)+'\n',encoding='utf-8')
print('Compiled and packaged. No upload performed.')
