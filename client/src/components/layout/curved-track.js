import { Track } from './track';
function curveRight() {
  let sq2 = Math.sqrt(2);
  let x = 2 * sq2 / (1 + sq2);

  return [[1/sq2, -1/sq2, x*(1 + 1/sq2)], [1/sq2, 1/sq2, x/sq2], [0, 0, 1]];
}

class CurveRight extends Track {
  constructor(origin, length) {
    super('curve', origin, curveRight(length));
  }
};


export { CurveRight };