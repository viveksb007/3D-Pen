#include <Wire.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>  
#include <util/delay.h>
#include <avr/interrupt.h>

extern void timer_init();
extern void record_AccelRegisters();
extern void record_GyroRegisters();
extern void setup_MPU();
extern void print_Data();
extern void sensor_calib();//calculates max value of g


volatile float current_time, integral, derivative, gForceX, gForceY, gForceZ, omegaX, omegaY, omegaZ;
volatile float gmax = 0, theta_x = 0,theta_y = 0, theta_z = 0, acc_y, acc_x, acc_z, vel_x, vel_y, vel_z, dist_x, dist_y, dist_z;
volatile long accelX, accelY, accelZ, gyroX, gyroY, gyroZ;
volatile uint32_t count =0;


int main(void)
{
    Serial.begin(9600);
    Wire.begin();
    setup_MPU();
    sensor_calib(); // Assuming during calibration gravity || Z-direction. Will modify later for Robustness
    
   while (1) //event to be exicuted at every cpu clk cycle except during ISR execution
    {
        record_AccelRegisters();
        record_GyroRegisters();
        //print_Data();
    }
}


void timer_init()
{
    TCCR0A |= (1 << WGM01);// Set the Timer Mode to CTC
    OCR0A = 250;// Set the value that you want to count to
    TIMSK0 |= (1 << OCIE0A);    //Set the ISR COMPA vect
    sei();         //enable interrupts
    TCCR0B |= (1 << CS01) | (1 << CS00);    // set prescaler to 64 and start the timer
}

void sensor_calib()
{
  //write procedure to calculate max value of g and store it in 'gmax'.
  for(int i=0;i<100;i++){
    record_AccelRegisters();
    gmax += gForceZ;
  }
  gmax /= 100;
  Serial.print("Caliberated with gmax = ");
  Serial.println(gmax);
  timer_init();
}
void setup_MPU()
{
  Wire.beginTransmission(0b1101000); 
  Wire.write(0x6B); 
  Wire.write(0b00000000); 
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); 
  Wire.write(0x1B); 
  Wire.write(0x00000000); 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); 
  Wire.write(0x1C);  
  Wire.write(0b00000000); 
  Wire.endTransmission(); 
}

void record_AccelRegisters() 
{
  Wire.beginTransmission(0b1101000); 
  Wire.write(0x3B); 
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); 
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); 
  accelY = Wire.read()<<8|Wire.read(); 
  accelZ = Wire.read()<<8|Wire.read(); 
  gForceX = 9.8*2*accelX / 16384.0;
  gForceY = 9.8*2*accelY / 16384.0; 
  gForceZ = 9.8*2*accelZ / 16384.0;
}



void record_GyroRegisters() 
{
  Wire.beginTransmission(0b1101000); 
  Wire.write(0x43); 
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); 
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); 
  gyroY = Wire.read()<<8|Wire.read(); 
  gyroZ = Wire.read()<<8|Wire.read(); 
  omegaX = gyroX / 131.0;
  omegaY = gyroY / 131.0; 
  omegaZ = gyroZ / 131.0;
}



void print_Data()
{
  //Serial.print("Gyro (deg)");
  //Serial.print(" X=");
  //Serial.print(omegaX );
  //Serial.print(" Y=");
  //Serial.print(omegaY );
  //Serial.print(" Z=");
  //Serial.print(omegaZ );
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);
}

ISR (TIMER0_COMPA_vect)  //event to be exicuted every 0.001s here
{
    theta_x += (omegaX*0.001);//degrees
    theta_y += (omegaY*0.001);
    theta_z += (omegaZ*0.001);
    acc_x = gForceX - gmax*cos(theta_x*0.01745329252); 
    acc_y = gForceY - gmax*cos(theta_y*0.01745329252);
    acc_z = gForceZ - gmax*cos(theta_z*0.01745329252);
    vel_x += acc_x*0.001;
    vel_y += acc_y*0.001;
    vel_z += acc_z*0.001;
    dist_x += vel_x*0.001;
    dist_y += vel_y*0.001;
    dist_z += vel_z*0.001;
    count++;
    if(count%10 == 0)   //event to be exicuted every 0.01s here
    {
        Serial.print(" Acc_X = ");
        Serial.print(acc_x);
        Serial.print(" Acc_Y = ");
        Serial.print(acc_y);
        Serial.print(" Acc_Z = ");
        Serial.println(acc_z);
        
        Serial.print(" Vel_X = ");
        Serial.print(vel_x);
        Serial.print(" Vel_Y = ");
        Serial.print(vel_y);
        Serial.print(" Vel_Z = ");
        Serial.println(vel_z);
        
        Serial.print(" Dist_X = ");
        Serial.print(dist_x);
        Serial.print(" Dist_Y = ");
        Serial.print(dist_y);
        Serial.print(" Dist_Z = ");
        Serial.println(dist_z);
    
    }
    
    
}
