function moveTo(x, y) {
  return "M"+x+" "+y+" ";
}

function line(x, y) {
  return "L"+x+" "+y+" ";
}

function partCircle(x, y, r, deg) {
  // rx ry rotation large-arc-flag sweep-flag x y

  return "M"+x+","+y+"  A "+r+" "+r+" 0 0 1 "+x+" "+y+" ";


  var start = polarToCartesian(x, y, r, startAngle);
  var end = polarToCartesian(x, y, r, endAngle);
  var largeArcFlag = Math.abs(endAngle - startAngle) <= 180 ? "0" : "1";
  var sweepFlag = endAngle > startAngle ? "1" : "0";
  var d = [
    "M", start.x, start.y,
    "A", r, r, 0, largeArcFlag, sweepFlag, end.x, end.y
  ].join(" ");
  return d;
}

function polarToCartesian(centerX, centerY, radius, angleInDegrees) {
  var angleInRadians = (angleInDegrees-90) * Math.PI / 180.0;
  return {
    x: centerX + (radius * Math.cos(angleInRadians)),
    y: centerY + (radius * Math.sin(angleInRadians))
  };
}

export { line, partCircle, moveTo };