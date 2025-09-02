function rotate(deg) {
  return [[Math.cos(deg), -Math.sin(deg), 0], [Math.sin(deg), Math.cos(deg), 0], [0, 0, 1]];
}

function translate(x, y) {
  return [[1, 0, x], [0, 1, y], [0, 0, 1]];
}

function buildMatrix(x, y, a) {
  return matrixMultiply(translate(x, y), rotate(a * Math.PI / 180));
}

function matrixMultiply(A, B) {
  let result = [];
  for (let i = 0; i < A.length; i++) {
    result[i] = [];
    for (let j = 0; j < B[0].length; j++) {
      let sum = 0;
      for (let k = 0; k < A[0].length; k++) {
        sum += A[i][k] * B[k][j];
      }
      result[i][j] = sum;
    }
  }
  return result;
}

function getTranslation(matrix) {
  return {x: matrix[0][2], y: matrix[1][2]};
}

function getRotationAngle(matrix) {
  return Math.atan2(matrix[1][0], matrix[0][0]) * 180 / Math.PI;
}

export { buildMatrix, translate, rotate, matrixMultiply, getTranslation, getRotationAngle };