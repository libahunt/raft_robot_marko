void runMotors(int frontDirection) {
  /**
   * @param frontDirection - sector number 0-39
   */
  
  int sector = int(frontDirection/90);//divide possible directions into 4 (90 degree) sectors
  int frontDirectionDeg = frontDirection * 9;
  
  DP("frontDirection = ");
  DPL(frontDirection);
  DP("sector = ");
  DPL(sector);
  
  switch (sector) {
    case 0: 
      PWMs[0] = fullPWM * cos(0.0174532925 * frontDirectionDeg);
      PWMs[1] = fullPWM * cos(0.0174532925 * (90 - frontDirectionDeg));
      PWMs[2] = 0;
      PWMs[3] = 0;
      break;
    
    case 1:
      PWMs[0] = 0;
      PWMs[1] = fullPWM * cos(0.0174532925 * (frontDirectionDeg - 90));
      PWMs[2] = fullPWM * cos(0.0174532925 * (180 - frontDirectionDeg));
      PWMs[3] = 0;
      break;

    case 2:
      PWMs[0] = 0;
      PWMs[1] = 0;
      PWMs[2] = fullPWM * cos(0.0174532925 * (frontDirectionDeg - 180));
      PWMs[3] = fullPWM * cos(0.0174532925 * (270 - frontDirectionDeg));
      break;
    
    case 3:
      PWMs[0] = fullPWM * cos(0.0174532925 * (360 - frontDirectionDeg));
      PWMs[1] = 0;
      PWMs[2] = 0;
      PWMs[3] = fullPWM * cos(0.0174532925 * (frontDirectionDeg - 270));
      break;
  }

  DPL("Motor PWMs\t\t0\t1\t2\t3");
  DP( "         \t\t");
  for (int j=0; j<4; j++) {
    DP(PWMs[j]);
    DP("\t"); 
  }
  DPL("");

  analogWrite(motor[0], (int) (PWMs[0]*motorPWMcoef[0]));
  analogWrite(motor[1], (int) (PWMs[1]*motorPWMcoef[1]));
  analogWrite(motor[2], (int) (PWMs[2]*motorPWMcoef[2]));
  analogWrite(motor[3], (int) (PWMs[3]*motorPWMcoef[3]));
}

void stopMotors() {
  analogWrite(motor[0], 0);
  analogWrite(motor[1], 0);
  analogWrite(motor[2], 0);
  analogWrite(motor[3], 0);
}

