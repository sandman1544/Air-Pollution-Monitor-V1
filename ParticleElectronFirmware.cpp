//SETTINGS
int samplingDuration=60000; //time between pollution measurements being sent to electron from arduino, NOT the time interval between data being sent to the internet

int pdatadelay=30000;
bool fanOn=false;

int pdataarrayelement=0;
int serialData=0;
char pdataarray[110];

class pollutionTimerEndClass
{
  private:
    Timer* pollutionTimerPointer_ = nullptr;
  public:
  void getPollutionData() 
  {
        readSensors();
        if (pollutionTimerPointer_ != nullptr) 
        {
            pollutionTimerPointer_->stop();
        }
    }
    void setPollutionTimerPointer(Timer* pollutionTimerPointer) 
    {
        pollutionTimerPointer_ = pollutionTimerPointer;
    }
   
   void readSensors()
   {
       Serial1.println("s");
   }
};

class fanTimerEndClass      //the purpose of this class is that it contains all methods (along with sequence) related to getting pollutiond data; it is the class connected to the timer 
{
    private:
       Timer* pollutionTimerPointer_;
    public:
    fanTimerEndClass(Timer* pollutionTimerPointer) 
    {
        pollutionTimerPointer_ = pollutionTimerPointer;
    }
    void toggleFan()        //sends data to the fan
    {                 
        if(fanOn==false)
            {
                Serial1.write('f');
                fanOn=true;
                pollutionTimerPointer_->start();
            }
            else
            {
                Serial1.write('n');
                 fanOn=false; 
            }
    }
    
public: void getPollutionData()    //controls sequence of instructions to eventually get the    pollution data
    {
       
    }
 
};

    pollutionTimerEndClass pollutionTimerEnd;
    Timer pollutionDataTimer(pdatadelay,&pollutionTimerEndClass::getPollutionData,pollutionTimerEnd);

    fanTimerEndClass fanTimerEnd(&pollutionDataTimer);
    Timer fanTimer(samplingDuration,&fanTimerEndClass::toggleFan,fanTimerEnd);


void setup() 
{
    pollutionTimerEnd.setPollutionTimerPointer(&pollutionDataTimer);
    Serial.begin(9600);     //set baudraet for serial monitor
    Serial1.begin(9600);    //set the buadrate for electron ---> arduino communication  (Serial 1 line)
    memset(pdataarray,0,110);
    fanTimer.start();   
}

void loop() 
{

}

void serialEvent1()
{
    if(Serial1.available()>0)
    {
        Serial1.readBytes(pdataarray,110);
        Serial.println(pdataarray);
    }
    sendPollutionDataToCloud();
    memset(pdataarray,0,110);
}

void sendPollutionDataToCloud()
{
        Particle.publish("Data Values",pdataarray);
}
