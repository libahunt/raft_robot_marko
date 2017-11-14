/**
 * RGB LED stip with 10 pixels in circle is mounted on the raft.
 * Pixel closest to front direction is lit with white color.
 * Back direction is dark.
 * On the right side red color is fading from bright (close to front) to dark (at the back).
 * On the left side green color is fading from bright (close to front) to dark (at the back).
 * Library: https://github.com/adafruit/LPD8806
 */
void showDirectionLights() {
  
  float frontLedFloat = frontDirection / 4.0;
  int frontLedIndex = (int) (frontLedFloat + 0.5);
  float backLedIndex = frontLedIndex + 5;
  if (backLedIndex > 9) backLedIndex -= 10;
  
  lights.setPixelColor(frontLedIndex, lights.Color(255, 255, 255));
  lights.setPixelColor(backLedIndex, 0);
  //Green is clockwise from front - the ascending indexes make decreasing brightness.
  for (int i=0; i<4; i++) {
    int index = frontLedIndex + 1 + i;
    if (index > 9) index -= 10;
    int brightness = ((4-i) * 64) - 1;
    lights.setPixelColor(index, lights.Color(0, brightness, 0));
  }
  //Red is counterclockwise from front - the descending indexes make decreasing brightness.
  for (int i=0; i<4; i++) {
    int index = frontLedIndex - 1 - i;
    if (index < 0) index += 10;
    int brightness = ((4-i) * 64) - 1;
    lights.setPixelColor(index, lights.Color(brightness, 0, 0));
  }
  
}

