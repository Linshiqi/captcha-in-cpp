#define cimg_display 0
#include "CImg.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace cimg_library;

std::string generateRandomText(int length) {
  const std::string characters =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  std::string randomText;
  for (int i = 0; i < length; ++i) {
    randomText += characters[rand() % characters.size()];
  }
  return randomText;
}

void addNoisePointers(const int width, const int height,
                      CImg<unsigned char> &image) {
  // Add some noise
  for (int i = 0; i < 1000; ++i) {
    int x = rand() % width;
    int y = rand() % height;
    unsigned char color[] = {(unsigned char)(rand() % 256),
                             (unsigned char)(rand() % 256),
                             (unsigned char)(rand() % 256)};
    image.draw_point(x, y, color);
  }
}

void drawBezier(int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3,
                CImg<unsigned char> &image, unsigned char *color) {
  for (float t = 0; t <= 1; t += 0.01) {
    float xt = (1 - t) * (1 - t) * (1 - t) * x0 +
               3 * (1 - t) * (1 - t) * t * x1 + 3 * (1 - t) * t * t * x2 +
               t * t * t * x3;
    float yt = (1 - t) * (1 - t) * (1 - t) * y0 +
               3 * (1 - t) * (1 - t) * t * y1 + 3 * (1 - t) * t * t * y2 +
               t * t * t * y3;
    image.draw_point(xt, yt, color);
  }
}

void addRandomCurveLine(const int width, const int height,
                        CImg<unsigned char> &image) {
  for (int i = 0; i < 20; ++i) { // Draw 5 random curves
    int x0 = 0;                 // Start from the left side
    int y0 = rand() % height;
    int x1 = rand() % width;
    int y1 = rand() % height;
    int x2 = rand() % width;
    int y2 = rand() % height;
    int x3 = width; // End at the right side
    int y3 = rand() % height;
    unsigned char color[] = {(unsigned char)(rand() % 256),
                             (unsigned char)(rand() % 256),
                             (unsigned char)(rand() % 256)};
    drawBezier(x0, y0, x1, y1, x2, y2, x3, y3, image, color);
  }
}

bool generateCaptchaImage(const std::string &text,
                          const std::string &filename) {
  const int width = 200;
  const int height = 70;
  const float fixed_space =
      10; // Adjust this value to control spacing between characters
  CImg<unsigned char> image(width, height, 1, 3, 0); // Create a black image
  const unsigned char white[] = {255, 255, 255};

  // Calculate character dimensions
  int totalChars = text.size();
  int charWidth = (width - totalChars * fixed_space) / totalChars;
  int charHeight = height / 2; // Assuming single line of text

  std::cout << "charWidth = " << charWidth << std::endl;
  std::cout << "charHeight = " << charHeight << std::endl;

  // Draw text with deformation
  int x = 2 * fixed_space;
  int y = (height - charHeight) / 2; // Center vertically
  for (char c : text) {
    CImg<unsigned char> charImage(charWidth, charHeight, 1, 3, 0);
    charImage.draw_text(0, 0, std::string(1, c).c_str(), white, 0, 1,
                        charHeight);

    // Apply random transformations
    float angle = (rand() % 30) - 15; // Rotate between -15 and 15 degrees

    charImage.rotate(angle).resize(charImage.width(), charImage.height());

    // Ensure the character fits within the image boundaries
    if (x + charImage.width() > width) {
      std::cout << "x + charImage.width() = " << x + charImage.width()
                << std::endl;
      return false;
    }

    image.draw_image(x, y, charImage);
    x += charImage.width();
  }

  // Add random curved lines
  addRandomCurveLine(width, height, image);
  addNoisePointers(width, height, image);
  // Save the image as PPM
  std::string tempFilename = "captcha.ppm";
  image.save(tempFilename.c_str());

  // Convert PPM to PNG using ImageMagick
  std::string command = "convert " + tempFilename + " " + filename;
  std::system(command.c_str());

  // Remove the temporary PPM file
  std::remove(tempFilename.c_str());

  return true;
}

int main() {
  srand(time(0));
  std::string captchaText = generateRandomText(6);
  std::string filename = "captcha.png";
  auto result = generateCaptchaImage(captchaText, filename);
  if (result) {
    std::cout << "CAPTCHA generated: " << captchaText << std::endl;
    std::cout << "Image saved as: " << filename << std::endl;
  } else {
    std::cout << "CAPTCHA not generated " << filename << std::endl;
  }

  return 0;
}
