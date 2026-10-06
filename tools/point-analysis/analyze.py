"""Read-only analysis of literal unsigned-short PROGMEM XY arrays (Python 3)."""
import argparse
import collections
import hashlib
import json
import math
import re
from pathlib import Path


def arrays(text):
    text = re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)
    pattern = r'const\s+unsigned\s+short\s+(\w+)\s*\[\s*\]\s*PROGMEM\s*=\s*\{([^}]+)\}\s*;'
    for name, body in re.findall(pattern, text, flags=re.S):
        tokens = [s.strip() for s in body.split(',') if s.strip()]
        if any(not re.fullmatch(r'0[xX][0-9a-fA-F]+|[0-9]+', s) for s in tokens):
            raise ValueError(f'{name}: unsupported expression; no partial parsing allowed')
        values = [int(s, 16 if s.lower().startswith('0x') else 10) for s in tokens]
        if len(values) % 2 or any(v < 0 or v > 65535 for v in values):
            raise ValueError(f'{name}: invalid 16-bit XY pairs')
        yield name, [(values[i] & 0x7fff, values[i+1], bool(values[i] & 0x8000)) for i in range(0, len(values), 2)]


def describe(points, delay_ms):
    distances = sorted(math.hypot(b[0]-a[0], b[1]-a[1]) for a, b in zip(points, points[1:]))
    counts = collections.Counter((x, y) for x, y, _ in points)
    on_counts = collections.Counter((x, y) for x, y, on in points if on)
    return {
        'points': len(points), 'on': sum(p[2] for p in points),
        'off': sum(not p[2] for p in points),
        'consecutive_same_xy': sum(a[:2] == b[:2] for a, b in zip(points, points[1:])),
        'consecutive_same_xy_and_state': sum(a == b for a, b in zip(points, points[1:])),
        'same_xy_state_transitions': sum(a[:2] == b[:2] and a[2] != b[2] for a,b in zip(points,points[1:])),
        'out_of_12bit_range': sum(x > 4095 or y > 4095 for x,y,_ in points),
        'segment_lengths': {k: distances[min(len(distances)-1, int((len(distances)-1)*q))] if distances else None for k,q in [('min',0),('p50',.5),('p90',.9),('p99',.99),('max',1)]},
        'segments_under_5': sum(d < 5 for d in distances),
        'most_repeated_xy': [{'xy':list(xy),'visits':n,'on_visits':on_counts[xy], 'on_dwell_proxy_ms':on_counts[xy]*delay_ms} for xy,n in counts.most_common(5)],
    }


def subdivide(points, steps):
    if not points: return []
    output=[(*points[0][:2], False)]
    for a,b in zip(points, points[1:]):
        # Mirrors integer truncation toward zero; Python float differs from MCU float32.
        for i in range(steps+1):
            output.append((a[0]+int((b[0]-a[0])*i/steps), a[1]+int((b[1]-a[1])*i/steps), b[2]))
    output.append((*points[-1][:2],False))
    return output


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('header',type=Path)
    ap.add_argument('--steps',type=int,default=5)
    ap.add_argument('--delay-ms',type=float,default=6)
    ap.add_argument('--output',type=Path)
    args=ap.parse_args()
    if args.steps<1 or args.delay_ms<0: ap.error('steps >= 1 and delay-ms >= 0 required')
    raw=args.header.read_bytes()
    entries=[]
    for name,points in arrays(raw.decode('utf-8-sig')):
        entries.append({'name':name,'stored':describe(points,args.delay_ms),'subdivided_model':describe(subdivide(points,args.steps),args.delay_ms)})
    if not entries: ap.error('no supported arrays found')
    report={'header':args.header.name,'sha256':hashlib.sha256(raw).hexdigest(),'steps':args.steps,'delay_ms':args.delay_ms,
        'limitations':'All arrays independently, not actual choreography frequency. XY visit * assumed delay is a dwell proxy, not optical exposure. Model excludes float32 rounding, UART timing, Uno interpolation/correction, toggle delays and mechanics.',
        'totals':{field:sum(e['stored'][field] for e in entries) for field in ['points','on','off','consecutive_same_xy','consecutive_same_xy_and_state','same_xy_state_transitions','out_of_12bit_range']},'arrays':entries}
    result=json.dumps(report,indent=2)+'\n'
    if args.output: args.output.write_text(result,encoding='utf-8')
    else: print(result)


if __name__=='__main__': main()
