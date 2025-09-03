import { Track } from './track';
import { matrixMultiply, rotate, translate } from './matrices';

const scaleFactor = 100;

function curveRight() {
  return matrixMultiply(rotate(45 * Math.PI / 180), translate(0, scaleFactor));
}

function curveLeft() {
  return matrixMultiply(rotate(-45 * Math.PI / 180), translate(0, scaleFactor));
}

class CurveRight extends Track {
  constructor(origin, length) {
    super('curve', origin, curveRight(length));
  }
};

class CurveLeft extends Track {
  constructor(origin, length) {
    super('curve', origin, curveLeft(length));
  }
};


export { CurveRight, CurveLeft };