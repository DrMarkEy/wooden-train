import { getTranslation, getRotationAngle, matrixMultiply } from './matrices';

class Track {
  // origin: matrix
  // transformation: matrix

  constructor(type, origin, transformation) {
    this.type = type;
    this.origin = origin;
    this.transformation = transformation;
  }

  getType() {
    return this.type;
  }

  getTransformation() {
    return this.transformation;
  }

  getTransformationRotation() {
    return getRotationAngle(this.transformation);
  }

  getDestination() {
    return matrixMultiply(this.origin, this.transformation);
  }

  getOriginPosition() {
    return getTranslation(this.origin);
  }

  getOriginRotation() {
    return getRotationAngle(this.origin);
  }
};

export { Track };