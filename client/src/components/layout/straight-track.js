import { Track } from './track';

function straight(length) {
  return [[1, 0, length], [0, 1, 0], [0, 0, 1]];
}

class StraightTrack extends Track {
  constructor(origin, length) {
    super('straight', origin, straight(length));
  }
};

class ShortStraightTrack extends StraightTrack {
  constructor(origin) {
    super(origin, 100);
  }
};

class LongStraightTrack extends StraightTrack {
  constructor(origin) {
    super(origin, Math.sqrt(2));
  }
};

export { ShortStraightTrack, LongStraightTrack };