#include <Arduino.h>

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void lab2rgb(const float lab[3], uint8_t rgb[3]) {
    float y = (lab[0] + 16) / 116;
    float x = lab[1] / 500 + y;
    float z = y - lab[2] / 200;
    float r, g, b;

    x = 0.95047f * ((x * x * x > 0.008856f) ? x * x * x : (x - 16.0f / 116) / 7.787f);
    y = 1.00000f * ((y * y * y > 0.008856f) ? y * y * y : (y - 16.0f / 116) / 7.787f);
    z = 1.08883f * ((z * z * z > 0.008856f) ? z * z * z : (z - 16.0f / 116) / 7.787f);

    r = x * 3.2406f + y * -1.5372f + z * -0.4986f;
    g = x * -0.9689f + y * 1.8758f + z * 0.0415f;
    b = x * 0.0557f + y * -0.2040f + z * 1.0570f;

    r = (r > 0.0031308f) ? (1.055f * pow(r, 1.0f / 2.4f) - 0.055f) : 12.92f * r;
    g = (g > 0.0031308f) ? (1.055f * pow(g, 1.0f / 2.4f) - 0.055f) : 12.92f * g;
    b = (b > 0.0031308f) ? (1.055f * pow(b, 1.0f / 2.4f) - 0.055f) : 12.92f * b;

    rgb[0] = static_cast<uint8_t>(clamp(r * 255.0f, 0.0f, 255.0f));
    rgb[1] = static_cast<uint8_t>(clamp(g * 255.0f, 0.0f, 255.0f));
    rgb[2] = static_cast<uint8_t>(clamp(b * 255.0f, 0.0f, 255.0f));
}

// Function to convert RGB to LAB
void rgb2lab(const uint8_t rgb[3], float lab[3]) {
    float r = rgb[0] / 255.0f;
    float g = rgb[1] / 255.0f;
    float b = rgb[2] / 255.0f;

    r = (r > 0.04045f) ? pow((r + 0.055f) / 1.055f, 2.4f) : r / 12.92f;
    g = (g > 0.04045f) ? pow((g + 0.055f) / 1.055f, 2.4f) : g / 12.92f;
    b = (b > 0.04045f) ? pow((b + 0.055f) / 1.055f, 2.4f) : b / 12.92f;

    float x = (r * 0.4124f + g * 0.3576f + b * 0.1805f) / 0.95047f;
    float y = (r * 0.2126f + g * 0.7152f + b * 0.0722f) / 1.00000f;
    float z = (r * 0.0193f + g * 0.1192f + b * 0.9505f) / 1.08883f;

    x = (x > 0.008856f) ? pow(x, 1.0f / 3.0f) : (7.787f * x) + 16.0f / 116;
    y = (y > 0.008856f) ? pow(y, 1.0f / 3.0f) : (7.787f * y) + 16.0f / 116;
    z = (z > 0.008856f) ? pow(z, 1.0f / 3.0f) : (7.787f * z) + 16.0f / 116;

    lab[0] = (116 * y) - 16;
    lab[1] = 500 * (x - y);
    lab[2] = 200 * (y - z);
}

// Function to compute perceptual distance between colors in CIELAB
float deltaE(const float labA[3], const float labB[3]) {
    float deltaL = labA[0] - labB[0];
    float deltaA = labA[1] - labB[1];
    float deltaB = labA[2] - labB[2];

    float c1 = sqrt(labA[1] * labA[1] + labA[2] * labA[2]);
    float c2 = sqrt(labB[1] * labB[1] + labB[2] * labB[2]);
    float deltaC = c1 - c2;
    float deltaH = deltaA * deltaA + deltaB * deltaB - deltaC * deltaC;
    deltaH = (deltaH < 0) ? 0 : sqrt(deltaH);

    float sc = 1.0f + 0.045f * c1;
    float sh = 1.0f + 0.015f * c1;

    float deltaLKlsl = deltaL / 1.0f;
    float deltaCkcsc = deltaC / sc;
    float deltaHkhsh = deltaH / sh;

    float i = deltaLKlsl * deltaLKlsl + deltaCkcsc * deltaCkcsc + deltaHkhsh * deltaHkhsh;
    return (i < 0) ? 0 : sqrt(i);
}
