
// Para evitar reinicios por batería
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#include <WiFi.h>
#include <micro_ros_arduino.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <geometry_msgs/msg/pose.h>
#include <std_msgs/msg/float32.h>
#include <geometry_msgs/msg/vector3.h>

#define M_PI 3.14159265358979323846

//============PINES===============
// --- Pines Motor 1 ---
#define pinPWM1 32
#define pinPWM2 33
#define pinenc1A 36
#define pinenc1B 39

// --- Pines Motor 2 ---
#define pinPWM3 27
#define pinPWM4 14
#define pinenc2A 34
#define pinenc2B 35

// --- Canales LEDC ---
#define chPWM1 0
#define chPWM2 1
#define chPWM3 2
#define chPWM4 3

// Definición de pines para 3 sensores
#define TRIG1 5
#define ECHO1 18

#define TRIG2 17
#define ECHO2 16

#define TRIG3 4
#define ECHO3 19
//===============================

rcl_subscription_t cmd_vel_sub;
rcl_subscription_t cmd_vel2_sub;
geometry_msgs__msg__Twist cmd_vel_msg;
geometry_msgs__msg__Twist cmd_vel2_msg;
rclc_executor_t executor;
rcl_node_t node;
rcl_allocator_t allocator;
rclc_support_t support;

// Publishers
rcl_publisher_t vel_pub_left;
rcl_publisher_t vel_pub_right;
rcl_publisher_t pose_pub;
rcl_publisher_t sensors_pub;

// Mensajes
std_msgs__msg__Float32 vel_msg_left;
std_msgs__msg__Float32 vel_msg_right;
geometry_msgs__msg__Pose pose_msg;
geometry_msgs__msg__Vector3 sensors_msg;



// --- Variables Motor 1 ---
volatile int cont1 = 0;
int contprev1 = 0;
double vel1 = 0, vel_rad1 = 0, pos_rad1 = 0, pos_rad_prev1 = 0;
double error1 = 0, error_ant1 = 0, suma_e1 = 0, dif_e1 = 0, PID1 = 0;
int PWM1_val = 0;

// --- Variables Motor 2 ---
volatile int cont2 = 0;
int contprev2 = 0;
double vel2 = 0, vel_rad2 = 0, pos_rad2 = 0, pos_rad_prev2 = 0;
double error2 = 0, error_ant2 = 0, suma_e2 = 0, dif_e2 = 0, PID2 = 0;
int PWM2_val = 0;

// -------------------------
double consigna1;
double consigna2;
//-------------------------
double sr = 0;
double sl = 0;
//=========PID================
// Motor 1
double kp1 = 163.8503;
double ki1 = 821.7901;
double kd1 = -1.1337;

// Motor 2
double kp2 = 49.6819;
double ki2 = 790.6569;
double kd2 = 0;
//===========================

// Modelo diferencial simple
  const double L = 0.23;    // distancia entre ruedas (m) AJUSTA según tu robot
  const double R =0.0325;  // radio de rueda (m)

//===========================

unsigned long t_actual = 0;
hw_timer_t *timer = NULL;

char ssid[] = "Mi 11 Lite 5G";
char password[] = "Losmueblessonblancos";
char agent_ip[]="10.152.174.188";
uint16_t agent_port = 8888; // Puerto del agente



// --- Encoder Motor 1 ---
void IRAM_ATTR int_enc1A(){
  if (digitalRead(pinenc1A)!=digitalRead(pinenc1B)) cont1++;
  else cont1--;
}
void IRAM_ATTR int_enc1B(){
  if (digitalRead(pinenc1A)==digitalRead(pinenc1B)) cont1++;
  else cont1--;
}

// --- Encoder Motor 2 ---
void IRAM_ATTR int_enc2A(){
  if (digitalRead(pinenc2A)!=digitalRead(pinenc2B)) cont2++;
  else cont2--;
}
void IRAM_ATTR int_enc2B(){
  if (digitalRead(pinenc2A)==digitalRead(pinenc2B)) cont2++;
  else cont2--;
}

double medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH);
  long distancia = duracion * 0.034 / 2; // velocidad del sonido ~340 m/s
  return distancia;
}

void cmd_vel_callback(const void * msgin) {
  const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;

  double v = msg->linear.x;   // velocidad lineal (m/s)
  double w = msg->angular.z;  // velocidad angular (rad/s)

  double v_r = v + w * (L/2.0);
  double v_l = v - w * (L/2.0);
  Serial.println(v_r);
  Serial.println(v_l);

  consigna1 = v_l / R; // rad/s
  consigna2 = v_r / R; // rad/s
}


void cmd_vel2_callback(const void * msgin) {
  const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;

  double v_l = msg->linear.x;   // velocidad lineal (m/s)
  double v_r = msg->linear.y;  // velocidad angular (rad/s)

  consigna1 = v_l / R; // rad/s
  consigna2 = v_r / R; // rad/s
}

/*void calculo_pose(){
  sr = 2*PI* R*(cont1/(44.0*35.0));
  sl = 2*PI* R*(cont2/(44.0*35.0));

  pose_msg.position.x = ((sr+sl)/(sr-sl))*(L/2)*sin((sr-sl)/L);
  pose_msg.position.y = ((sr+sl)/(sr-sl))*(L/2)*(1-cos((sr-sl)/L));
  pose_msg.orientation.z = (sr-sl)/L;
}*/

long cont1_prev_pose = 0;
long cont2_prev_pose = 0;

void calculo_pose(){
  long dcont1 = cont1 - cont1_prev_pose;
  long dcont2 = cont2 - cont2_prev_pose;

  cont1_prev_pose = cont1;
  cont2_prev_pose = cont2;

  double sr = 2 * PI * R * (dcont1 / (44.0 * 35.0));
  double sl = 2 * PI * R * (dcont2 / (44.0 * 35.0));

  double ds = (sr + sl) / 2.0;
  double dtheta = (sr - sl) / L;

  pose_msg.position.x += ds * cos(pose_msg.orientation.z + dtheta / 2.0);
  pose_msg.position.y += ds * sin(pose_msg.orientation.z  + dtheta / 2.0);
  pose_msg.orientation.z += dtheta;
}

// --- ISR Timer ---
void onTimerISR(){
  t_actual = millis();

  // Motor 1
  vel1=(cont1-contprev1)/0.01;
  pos_rad1=2*PI*cont1/(44*35);
  vel_rad1=(pos_rad1-pos_rad_prev1)/0.01;
  contprev1=cont1;
  pos_rad_prev1=pos_rad1;
  

  // Motor 2
  vel2=(cont2-contprev2)/0.01;
  pos_rad2=2*PI*cont2/(44*35);
  vel_rad2=(pos_rad2-pos_rad_prev2)/0.01;
  contprev2=cont2;
  pos_rad_prev2=pos_rad2;
    
    P3_PID(vel_rad1, consigna1, kp1, ki1, kd1, error1, error_ant1, suma_e1, dif_e1, PID1, PWM1_val);
    P3_actua(PWM1_val, chPWM1, chPWM2);
    
    P3_PID(vel_rad2, consigna2, kp2, ki2, kd2, error2, error_ant2, suma_e2, dif_e2, PID2, PWM2_val);
    P3_actua(PWM2_val, chPWM3, chPWM4);
 

  
  
}

// --- PID Genérico con parámetros por motor ---
void P3_PID(double parametro, double consigna,
            double kp, double ki, double kd,
            double &error, double &error_ant, double &suma_e, double &dif_e,
            double &PID, int &PWM){
  if(consigna == 0){
    PWM =0;
  }else{
    error = consigna - parametro;
    suma_e += error*0.01;
    dif_e = (error - error_ant)/0.01;
    PID = kp*error + ki*suma_e + kd*dif_e;
    error_ant = error;
  
    if(fabs(consigna - parametro) > 0.1) {
      PWM = (int)PID;
    }
    else {
      
      PID = 0;
      
    }
  }
  

  if(PWM > 1023) {
    PWM = 1023;
  }
  else if(PWM < -1023) {
    PWM = -1023;
  }
  else if(PWM < 200 && PWM > 0) {
    PWM = 200;
  }
  else if(PWM > -200 && PWM < 0) {
    PWM = -200;
  }
}

// --- Actuación Genérica ---
void P3_actua(int PWM, int chA, int chB){
  if(PWM > 0){
    ledcWrite(chA, PWM);
    ledcWrite(chB, 0);
  } else if(PWM < 0){
    ledcWrite(chA, 0);
    ledcWrite(chB, abs(PWM));
  } else {
    ledcWrite(chA, 0);
    ledcWrite(chB, 0);
  }
}

void setup() {
  Serial.begin(115200);

  // Protección de Batería
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Desactivar reinicio por bajada de voltaje

  // Conectar WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  // Configurar transporte micro-ROS por WiFi
  set_microros_wifi_transports(ssid, password, agent_ip, agent_port);

  // Timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimerISR, true);
  timerAlarmWrite(timer, 10000, true); // 10ms
  timerAlarmEnable(timer);

  // Encoder Motor 1
  pinMode(pinenc1A, INPUT);
  pinMode(pinenc1B, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinenc1A), int_enc1A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinenc1B), int_enc1B, CHANGE);

  // Encoder Motor 2
  pinMode(pinenc2A, INPUT);
  pinMode(pinenc2B, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinenc2A), int_enc2A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinenc2B), int_enc2B, CHANGE);

  // PWM Motor 1
  ledcSetup(chPWM1, 1000, 10);
  ledcAttachPin(pinPWM1, chPWM1);
  ledcWrite(chPWM1, 0);
  ledcSetup(chPWM2, 1000, 10);
  ledcAttachPin(pinPWM2, chPWM2);
  ledcWrite(chPWM2, 0);


  // PWM Motor 2
  ledcSetup(chPWM3, 1000, 10);
  ledcAttachPin(pinPWM3, chPWM3);
  ledcWrite(chPWM3, 0);
  ledcSetup(chPWM4, 1000, 10);
  ledcAttachPin(pinPWM4, chPWM4);
  ledcWrite(chPWM4, 0);
  
  //SENSOR FRONTAL
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);

  //SENSOR DERECHA
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  
  //SENSOR IZQUIERDA
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "esp32_node", "", &support);
  
  rclc_subscription_init_default(
    &cmd_vel_sub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
    "cmd_vel"
  );

  rclc_subscription_init_default(
    &cmd_vel2_sub,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
    "cmd_vel2"
  );

  rclc_publisher_init_default(
  &vel_pub_left,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
  "velocity_left"
  );
  
  rclc_publisher_init_default(
    &vel_pub_right,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "velocity_right"
  );

  rclc_publisher_init_default(
  &pose_pub,
  &node,
  ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Pose),
  "robot_pose"
  );

  rclc_publisher_init_default( 
    &sensors_pub, 
    &node, 
    ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Vector3), 
    "sensors_vector" 
    );
  
  rclc_executor_init(&executor, &support.context, 2, &allocator);
  rclc_executor_add_subscription(&executor, &cmd_vel_sub, &cmd_vel_msg, &cmd_vel_callback, ON_NEW_DATA);
  rclc_executor_add_subscription(&executor, &cmd_vel2_sub, &cmd_vel2_msg, &cmd_vel2_callback, ON_NEW_DATA);


  pose_msg.position.x = 0.0;
  pose_msg.position.y = 0.0;
  pose_msg.position.z = 0.0;
  
  pose_msg.orientation.x = 0.0;
  pose_msg.orientation.y = 0.0;
  pose_msg.orientation.z = 0.0;
  pose_msg.orientation.w = 0.0;


}

void loop() {

  // EN CENTIMETROS
  double d1 = medirDistancia(TRIG1, ECHO1);
  double d2 = medirDistancia(TRIG2, ECHO2);
  double d3 = medirDistancia(TRIG3, ECHO3);
 
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));

  calculo_pose();

  sensors_msg.x = d1;
  sensors_msg.y = d2;
  sensors_msg.z = d3;

  vel_msg_left.data  = vel_rad1;  // velocidad angular rueda izquierda
  vel_msg_right.data = vel_rad2;  // velocidad angular rueda derecha

  rcl_publish(&vel_pub_left, &vel_msg_left, NULL);
  rcl_publish(&vel_pub_right, &vel_msg_right, NULL);
  rcl_publish(&pose_pub, &pose_msg, NULL);
  rcl_publish(&sensors_pub, &sensors_msg, NULL);


  delay(10);
}
