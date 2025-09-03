import { Track } from './track';
import { translate } from './matrices';

const scaleFactor = 100;

function straight(length) {
  return translate(0, length);
}

class StraightTrack extends Track {
  constructor(origin, length) {
    super('straight', origin, straight(length * scaleFactor));
  }
};

class ShortStraightTrack extends StraightTrack {
  constructor(origin) {
    super(origin, 1);
  }
};

class LongStraightTrack extends StraightTrack {
  constructor(origin) {
    super(origin, Math.sqrt(2));
  }
};

export { ShortStraightTrack, LongStraightTrack };