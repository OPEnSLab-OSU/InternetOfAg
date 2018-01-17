
float TCTemp;
float CJTemp;
float tc_vin;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tcouple_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  measure_tcouple();
}
