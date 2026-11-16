/** Counter-clockwise DIP numbering, component-side view. Rotation clockwise. */
export function physicalPins(width: number, height: number, angle: number) {
  const horizontal = angle === 90 || angle === 270;
  const n = horizontal ? width : height;
  const w = (width - 1) * 50, h = (height - 1) * 50;
  const pins: {pin: number; x: number; y: number}[] = [];
  for (let i = 0; i < n; i++) {
    if (angle === 0) {
      pins.push({pin:i+1,x:0,y:i*50},{pin:2*n-i,x:w,y:i*50});
    } else if (angle === 90) {
      pins.push({pin:n-i,x:i*50,y:0},{pin:n+1+i,x:i*50,y:h});
    } else if (angle === 180) {
      pins.push({pin:n+1+i,x:0,y:i*50},{pin:n-i,x:w,y:i*50});
    } else if (angle === 270) {
      pins.push({pin:2*n-i,x:i*50,y:0},{pin:i+1,x:i*50,y:h});
    } else throw new Error('Unsupported package rotation');
  }
  return pins;
}
