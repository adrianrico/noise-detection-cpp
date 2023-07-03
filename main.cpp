#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

/*
MANUAL INPUT PROCESSING

Don't forget to remove all the couts or replace them as logs...
Real function needs manual inputs as parameters...
*/
int signalTreatmentNoDummy()
{
    int finalResult {0}; 

    double  setpoint {0};
    cout<<"TEST SETPOINT (-1 if not applied): ";
    cin>>setpoint;
    
    double upperTempLimit    {0};
    double lowerTempLimit    {0};
    double tempLimitOffset   {20}; 

    setpoint != -1 ? (upperTempLimit = setpoint + tempLimitOffset, lowerTempLimit = setpoint - tempLimitOffset) : setpoint = -1;     

    //This stability factor will be removed and tuned in the code...
    //Manual just for testing...
    double stabilityTolerance {0};
    cout<<"TEST STABILITY FACTOR: ";
    cin>>stabilityTolerance;

    int pyroIndex {0};
    cout<<"TEST PYRO INDEX: ";
    cin>>pyroIndex;
    
    cout<<endl<<"BASIC SETUP:"<<endl;
    cout<<"Leading Pyro:\t"      <<pyroIndex<<endl;
    setpoint == -1 ? cout<<"Temp_SP:\t"          <<"No Temp Setpoint used to measure"<<endl : cout<<"Temp_SP:\t"          <<setpoint<<endl;; 
    cout<<"Upper Limit:\t"      <<upperTempLimit<<endl;
    cout<<"Lower Limit:\t"      <<lowerTempLimit<<endl;
    cout<<"Stability Fac:\t"    <<stabilityTolerance<<endl<<endl;

    vector <float> signal {};
    vector <int> sectiondata {};

    int signalSize                      {0};
    int consecutiveNoiseCounter         {0};
    int tickCounter                     {0};
    int foundSections                   {0};
    int noisyPointCounter               {0};

    float measuredTick      {0};

    bool stableFlag         {true};
    bool sectionIsOpen      {false};

     do
    {  
        cout<<endl<<"TEST MEASUREMENT: (enter -1 to stop): ";
        cin>>measuredTick;

        if (measuredTick != -1)
        {
            cout<<"INDEX"<<"\tVALUE"<<"\tDIFF"<<endl;
            signal.push_back(measuredTick);      
            signalSize = signal.size();

            if (tickCounter == 0)
            {
                cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal.at(tickCounter)<<"\t--"<<"\t  Initial measurement..."<<endl;
                tickCounter++;
            }else
            {
                //First loss of stability...
                if (abs(signal[tickCounter]-signal[tickCounter-1]) > stabilityTolerance && stableFlag == true)
                {
                    stableFlag = false;
                    foundSections++;
                    consecutiveNoiseCounter++;
                    noisyPointCounter++;

                    //To open section:
                    //sectionData[(foundSections - 1)*2] = tickCounter;
                    if (sectionIsOpen == false)
                    {
                        sectiondata.push_back(tickCounter);
                        sectionIsOpen = true;
                    }
                    
                    //Check if setpoint available...
                    if (setpoint != -1)
                    {
                        if (abs(signal[tickCounter]) > abs((setpoint-lowerTempLimit)))
                        {
                            cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Major deviation detected; Outside lower temp tolerance; Measuring signal stability..."<<endl;
                            tickCounter++;
                        }else
                        {
                            cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Major deviation detected; Within temp tolerance; Measuring signal stability..."<<endl;
                            tickCounter++;
                        }

                    }else
                    {
                        cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Major deviation detected -- No temp SP; Measuring signal stability..."<<endl;
                        tickCounter++;
                    }    
                
                }else
                {
                    //Continue the unstability...
                    if (abs(signal[tickCounter]-signal[tickCounter-1]) > stabilityTolerance && stableFlag != true)
                    {
                        if (setpoint != -1)
                        {
                            if (abs(signal[tickCounter]) > abs((setpoint-lowerTempLimit)))
                            {
                                consecutiveNoiseCounter >= 2 ?
                                cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Still not stable and outside temp tolerances... Warning! Possible water / surface issues..."<<endl
                                :
                                cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Still not stable and outside temp tolerances..."<<endl;

                                tickCounter++;
                            }else
                            {
                                consecutiveNoiseCounter >= 2 ?
                                cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Still not stable... Warning! Possible water / surface issues..."<<endl
                                :
                                cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Still not stable..."<<endl;

                                tickCounter++;
                            }

                        }else
                        {
                                consecutiveNoiseCounter >= 2 ?
                                cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Still not stable... Warning! Possible water / surface issues..."<<endl
                                :
                                cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  Still not stable..."<<endl;  
                                tickCounter++;
                        }

                        consecutiveNoiseCounter++;
                        noisyPointCounter++;

                    }else
                    {
                        stableFlag = true;
                        cout<<tickCounter<<"\t"<<std::setprecision(6)<<signal[tickCounter]<<"\t"<<std::setprecision(6)<<abs(signal[tickCounter]-signal[tickCounter-1])<<"\t  ok..."<<endl;


                        //To close section...
                        //sectionData[((foundSections - 1)*2)+1] = tickCounter;
                        if (sectionIsOpen == true)
                        {
                            sectiondata.push_back(tickCounter);
                            sectionIsOpen = false;
                        }
                        
                        consecutiveNoiseCounter = 0;
                        tickCounter++;
                    }
                    
                }
                
            }
            
            cout<<endl;
        }
    }while (measuredTick != -1);

    cout<<endl<<left<<setfill('*')<<setw(100)<<"*"<<endl;
    cout<<endl<<"-- SUMMARY --"<<endl;
    cout<<"SIGNAL SIZE: "<<signalSize<<endl;
    cout<<"MEASURED TICKS: "<<tickCounter<<endl;
    cout<<"NOISY TICKS: "<<noisyPointCounter<<endl;
    cout<<"NOISE PERCENTAGE: [ "<<(100*noisyPointCounter)/signalSize<<" ] %"<<endl; 
    
    double noisePercentage = (100*noisyPointCounter)/signalSize;

    noisePercentage >= 30 ? finalResult = pyroIndex : finalResult = 0; 

    noisePercentage >= 30 ?
    cout<<endl<< "WARNING! REACHED MORE THAN 30% NOISE --> [ "<<noisePercentage<<" ] EQUIPMENT CHECK REQUIRED...!"<<endl
    :
    cout<<endl<< "THIS DEVICE DETECTED LESS THAN 30% NOISE --> ["<<noisePercentage<<"] NO SIGNIFICANT INTERFERENCE..."<<endl;
    cout<<left<<setfill('*')<<setw(100)<<"*"<<endl;

    return finalResult;
}









/*
SIGNAL TREATMENT WITH DATA FROM FILES... 
*/
int signalTreatmentFromFile(vector <double> signal, int pyroIndex, double tempSP)
{
    int finalResult {0};
    
    double upperTempLimit    {0};
    double lowerTempLimit    {0};
    const double tempLimitOffset   {20};

    tempSP != -1 ? (upperTempLimit = tempSP + tempLimitOffset, lowerTempLimit = tempSP - tempLimitOffset) : tempSP = -1;    

    const double stabilityTolerance    {5}; 

    int signalSize=signal.size();
    int foundSections                   {0};
    int sectionData [(signalSize*3)] =  {0};
    int noisyPointCounter               {0};
    int consecutiveNoiseCounter {0};
    
    bool stableFlag         {true};
    bool sectionIsOpen      {false};

    vector <int> sectiondata {};

    

//Print basic data...
    cout<<left<<setfill('*')<<setw(100)<<"*"<<endl<<endl;
    cout<<"-- BASIC SETUP --"<<endl;
    cout<<"Pyro Index:\t\t\t"<<pyroIndex<<endl;
    tempSP == -1 ? cout<<"Temp Setpoint:\t\t\t"<<"No temp setpoint used to measure (Intermediate Pyro)"<<endl : cout<<"Temp Setpoint:\t\t\t"<<tempSP<<endl;
    cout<<"Upper temp limit applied:\t"<<upperTempLimit<<endl;
    cout<<"Lower temp limit applied:\t"<<lowerTempLimit<<endl;
    cout<<"Stability factor:\t\t"<<stabilityTolerance<<endl;
    cout<<endl<<left<<setfill('*')<<setw(100)<<"*"<<endl; 

//Detection algorithm start...
    cout<<endl<<"INDEX"<<"\tVALUE"<<"\t\tDIFF"<<endl;
    for (int i = 0; i < signalSize; i++)
    {
        if (i == 0)
        {
           cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t--"<<"\t  Initial measurement..."<<endl;	
        }else
        {
    
            if (abs(signal[i]-signal[i-1]) > stabilityTolerance && stableFlag == true)
            {
                stableFlag = false;
                foundSections++;
                sectionData[(foundSections-1)*3] = i;
                consecutiveNoiseCounter++;
                noisyPointCounter++;

                //To open section:
                //sectionData[(foundSections - 1)*2] = tickCounter;
                if (sectionIsOpen == false)
                {
                    sectiondata.push_back(i);
                    sectionIsOpen = true;
                }

                //Check if setpoint available...
                    if (tempSP != -1)
                    {
                        if (abs(signal[i]) > abs((tempSP-lowerTempLimit)))
                        {
                            cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Major deviation detected; Outside lower temp tolerance; Measuring signal stability..."<<endl;
                        }else
                        {
                            cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Major deviation detected; Within temp tolerance; Measuring signal stability..."<<endl;
                        }

                    }else
                    {
                        cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Major deviation detected -- No temp SP; Measuring signal stability..."<<endl;
                    }
                
            }else
            {
                //Continue the unstability...
                    if (abs(signal[i]-signal[i-1]) > stabilityTolerance && stableFlag != true)
                    {
                        if (tempSP != -1)
                        {
                            if (abs(signal[i]) > abs((tempSP-lowerTempLimit)))
                            {
                                consecutiveNoiseCounter >= 2 ?
                                cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Still not stable and outside temp tolerances... Warning! Possible water / surface issues..."<<endl
                                :
                                cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Still not stable and outside temp tolerances..."<<endl;
                            }else
                            {
                                consecutiveNoiseCounter >= 2 ?
                                cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Still not stable... Warning! Possible water / surface issues..."<<endl
                                :
                                cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Still not stable..."<<endl;
                            }

                        }else
                        {
                                consecutiveNoiseCounter >= 2 ?
                                cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Still not stable... Warning! Possible water / surface issues..."<<endl
                                :
                                cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  Still not stable..."<<endl;  
                               
                        }

                        consecutiveNoiseCounter++;
                        noisyPointCounter++;
                    
                }else
                {   
                    stableFlag = true;
                    cout<<i<<"\t"<<std::setprecision(6)<<signal[i]<<"\t\t"<<std::setprecision(6)<<abs(signal[i]-signal[i-1])<<"\t  ok..."<<endl;

                    consecutiveNoiseCounter = 0;

                    if (foundSections > 0 && sectionData[((foundSections-1)*3)+1] == 0)
                    {   sectionData[((foundSections-1)*3)+1] = i;
                        sectionData[((foundSections-1)*3)+2] = (sectionData[((foundSections-1)*3)+1])-(sectionData[((foundSections-1)*3)]);
                    }

                    
                }
                
            }
        }
        
    }
    

    cout<<endl<<left<<setfill('*')<<setw(100)<<"*"<<endl;
    cout<<endl<<"-- SUMMARY --"<<endl;
    cout<<"SIGNAL SIZE:\t\t"<<"[ "<<signalSize<<"\t]"<<endl;
    cout<<"NOISY TICKS:\t\t"<<"[ "<<noisyPointCounter<<"\t]"<<endl;
    cout<<"TOTAL NOISE PERCENTAGE:\t"<<"[ "<<(100*noisyPointCounter)/signalSize<<"\t] %"<<endl; 
    
    double noisePercentage = (100*noisyPointCounter)/signalSize;

    noisePercentage >= 30 ? finalResult = pyroIndex : finalResult = 0; 

    noisePercentage >= 30 ?
    cout<<endl<< "WARNING! REACHED MORE THAN 30% NOISE --> [ "<<noisePercentage<<" ] EQUIPMENT CHECK REQUIRED...!"<<endl
    :
    cout<<endl<< "THIS DEVICE DETECTED LESS THAN 30% NOISE: ( "<<noisePercentage<<" ) % NO SIGNIFICANT INTERFERENCE..."<<endl;
    cout<<endl<<left<<setfill('*')<<setw(100)<<"*"<<endl;


    return finalResult;
}










/*
READING DATA FROM FILES AND FILL VECTORS
*/

#define excelFile  "06_AVG_pyroSignal.csv"
#define excelFile2 "06_RAW_pyroSignal.csv"
#define excelFile3 "07_AVG_pyroSignal.csv"
#define excelFile4 "07_RAW_pyroSignal.csv"
#define excelFile5 "08_AVG_pyroSignal.csv"
#define excelFile6 "08_RAW_pyroSignal.csv"
//#define excelFile5 "10_AVG_pyroSignal.csv"
//#define excelFile6 "00_AVG_scannerPyroSignal.csv"

void readFiles()
{

/*Read all signal files...*/
    vector <double> pyr06avgsignal {};
    vector <double> pyr06rawsignal {};
    vector <double> pyr07avgsignal {};
    vector <double> pyr07rawsignal {};
    vector <double> pyr08avgsignal {};
    vector <double> pyr08rawsignal {};

    for (size_t fileNumber = 0; fileNumber < 6; fileNumber++)
    {
        
        switch (fileNumber)
        {
            case 0:
            {  
                cout<<"Reading signal data file ["<<fileNumber<<"]";

                ifstream openedFile(excelFile);
                string          fileLine;
            
                getline(openedFile,fileLine);
                while (getline(openedFile,fileLine))
                {
                    stringstream    stream(fileLine);
                    string          lineValue;
                    getline(stream, lineValue);

                    pyr06avgsignal.push_back(stod(lineValue));
                }
                cout<<" --> Vector ready...!"<<endl;
            }
            break;

            case 1:
            {  
                cout<<"Reading signal data file ["<<fileNumber<<"]";

                ifstream openedFile(excelFile2);
                string          fileLine;
            
                getline(openedFile,fileLine);
                while (getline(openedFile,fileLine))
                {
                    stringstream    stream(fileLine);
                    string          lineValue;
                    getline(stream, lineValue);

                    pyr06rawsignal.push_back(stod(lineValue));
                }
                cout<<" --> Vector ready...!"<<endl;
            }
            break;

            case 2:
            {  
                cout<<"Reading signal data file ["<<fileNumber<<"]";

                ifstream openedFile(excelFile3);
                string          fileLine;
            
                getline(openedFile,fileLine);
                while (getline(openedFile,fileLine))
                {
                    stringstream    stream(fileLine);
                    string          lineValue;
                    getline(stream, lineValue);

                    pyr07avgsignal.push_back(stod(lineValue));
                }
                cout<<" --> Vector ready...!"<<endl;
            }
            break;

            case 3:
            {  
                cout<<"Reading signal data file ["<<fileNumber<<"]";

                ifstream openedFile(excelFile4);
                string          fileLine;
            
                getline(openedFile,fileLine);
                while (getline(openedFile,fileLine))
                {
                    stringstream    stream(fileLine);
                    string          lineValue;
                    getline(stream, lineValue);

                    pyr07rawsignal.push_back(stod(lineValue));
                }
                cout<<" --> Vector ready...!"<<endl;
            }
            break;

            case 4:
            {  
                cout<<"Reading signal data file ["<<fileNumber<<"]";

                ifstream openedFile(excelFile5);
                string          fileLine;
            
                getline(openedFile,fileLine);
                while (getline(openedFile,fileLine))
                {
                    stringstream    stream(fileLine);
                    string          lineValue;
                    getline(stream, lineValue);

                    pyr08avgsignal.push_back(stod(lineValue));
                }
                cout<<" --> Vector ready...!"<<endl;
            }
            break;

            case 5:
            {  
                cout<<"Reading signal data file ["<<fileNumber<<"]";

                ifstream openedFile(excelFile6);
                string          fileLine;
            
                getline(openedFile,fileLine);
                while (getline(openedFile,fileLine))
                {
                    stringstream    stream(fileLine);
                    string          lineValue;
                    getline(stream, lineValue);

                    pyr08rawsignal.push_back(stod(lineValue));
                }
                cout<<" --> Vector ready...!"<<endl;
            }
            break;
        
            default:
            break;
        }
    }
/*Read all signal files... end*/
    
/*Select the signal to work with...*/
    cout<<endl;
    cout<<"[1 ] Pyro 06 avg --> (Intermediate pyrometer  )..."<<endl;
    cout<<"[2 ] Pyro 06 raw --> (Intermediate pyrometer  )..."<<endl;
    cout<<"[3 ] Pyro 07 avg --> (Intermediate pyrometer  )..."<<endl;
    cout<<"[4 ] Pyro 07 raw --> (Intermediate pyrometer  )..."<<endl;
    cout<<"[5 ] Pyro 08 avg --> (CS exit pyrometer       )..."<<endl;
    cout<<"[6 ] Pyro 08 raw --> (CS exit pyrometer       )..."<<endl;
    cout<<"[10] Process all signals..."<<endl<<endl;

    cout<<endl<<"Select signal to process --> ";

    int userInput{};
    cin>>userInput; 
    cout<<endl;

    switch (userInput)
    {
        case 1:
            signalTreatmentFromFile(pyr06avgsignal,0,-1);
        break;

        case 2:
            signalTreatmentFromFile(pyr06rawsignal,0,-1);
        break;

        case 3:
            signalTreatmentFromFile(pyr07avgsignal,1,-1);
        break;

        case 4:
            signalTreatmentFromFile(pyr07rawsignal,1,-1);
        break;   

        case 5:
            signalTreatmentFromFile(pyr08avgsignal,3,720);
        break;

        case 6:
            signalTreatmentFromFile(pyr08rawsignal,3,720);
        break;  

        case 10:
        {   
            vector <int> pyroStatus;
            
            pyroStatus.push_back(signalTreatmentFromFile(pyr06avgsignal,0,-1));
            pyroStatus.push_back(signalTreatmentFromFile(pyr07avgsignal,1,-1));
            pyroStatus.push_back(signalTreatmentFromFile(pyr08avgsignal,3,720));
            //pyroStatus.push_back(signalTreatmentFromFile(pyrSCNsignal,10,570));
            
            int leadingPyro {};
            cout<<endl<<"LEADING PYRO INDEX? (-1 IF NOT APPLIED)--> ";
            cin>>leadingPyro;
            cout<<endl;
            
            for (size_t i = 0; i < pyroStatus.size(); i++)
            {
                if (pyroStatus.at(i) != 0)
                {
                    pyroStatus.at(i) == leadingPyro?
                    (cout<<left<<setfill('-')<<setw(100)<<"-"<<endl,
                    cout<<"PYRO [ "<<pyroStatus.at(i)<<" ] --> WARNING! THIS DEVICE REACHED MORE THAN 30% NOISE --> EQUIPMENT / CONDITIONS CHECK REQUIRED...!"<<endl,
                    cout<<"\t   --> THIS DEVICE IS USED FOR L2 ADAPTION! ADAPTION NEEDS TO BE DISABLED...!"<<endl,
                    cout<<left<<setfill('-')<<setw(100)<<"-"<<endl)
                    :
                    (cout<<left<<setfill('-')<<setw(100)<<"-"<<endl,
                    cout<<"PYRO [ "<<pyroStatus.at(i)<<" ] --> WARNING! THIS DEVICE REACHED MORE THAN 30% NOISE --> EQUIPMENT / CONDITIONS CHECK REQUIRED...!"<<endl,
                    cout<<left<<setfill('-')<<setw(100)<<"-"<<endl);
                    
                }else
                {
                    cout<<left<<setfill('-')<<setw(100)<<"-"<<endl;
	                cout<<"DEVICE MEASURED STABLE SIGNAL..."<<endl;
	                cout<<left<<setfill('-')<<setw(100)<<"-"<<endl;
                }
                                
            }
            
        }
            
        break; 

        
        default: break;
    }
/*Select the signal to work with... end*/
}










/*Simulate call fillPyroDataGeneral each 192ms...*/
void requestDataFromL1()
{   
    double dTemp                    {0.0};
    double pyroPlant_getMinValue    {220}; //Simulate pyro min device range --> pyroPlant.getMinValue()
    double stabilityFactor          {5};
    double prevDtemp                {0};

    size_t consecutiveNoiseCounter  {0};
    size_t checkToleranceCounter    {0};

    bool stableFlag                 {true};
    bool isRolling                  {false};

    do
    {
        //Simulate call a reading from L1...
        cout<<left<<setfill('-')<<setw(41)<<"-"<<endl; //REMOVE...!
        cout<<"[-1                   ] --> END"<<endl;
        cout<<"[ENTER SIMULATED VALUE] --> ";
        cin>>dTemp;
        cout<<left<<setfill('-')<<setw(41)<<"-"<<endl; //REMOVE...!

        if (dTemp < pyroPlant_getMinValue && isRolling == false)
        {
            cout<<endl<<"NO STRIP DETECTED\t Measurement below device low range..."<<endl;
            cout<<"dTemp (Actual Value):\t "<<dTemp<<endl;
            cout<<endl;
          
            prevDtemp = 0;
            consecutiveNoiseCounter = 0;
            stableFlag = true;
            
        }else
        {   

            if (dTemp < pyroPlant_getMinValue && isRolling == true)
            {
                /* comparison  */
                checkToleranceCounter++;

                checkToleranceCounter > 3 ? isRolling = false : isRolling = true;
            }
            
            /*
            - Find the first measurement above device lor range...
            - Store value in a variable to start camparisons...               
            */
            if (prevDtemp == 0)
            {

                cout<<endl<<"STRIP FOUND!\t First valid measurement stored..."<<endl;
                cout<<"dtemp:\t"<<dTemp<<"\t prevDtemp:\t"<<prevDtemp<<endl<<endl;
                
                prevDtemp = dTemp;
                isRolling = true;
                
            }else
            {
                if (prevDtemp !=0)
                {
                    //First unstability find...
                    if (abs(dTemp - prevDtemp) > stabilityFactor && stableFlag == true)
                    {
                        cout<<endl<<"Unstability found..."<<endl;
                        cout<<"dtemp:\t\t"<<dTemp<<"\t prevDtemp:\t"<<prevDtemp<<endl;
                        cout<<"Tolerance jump:\t"<<abs(dTemp - prevDtemp)<<endl<<endl;

                        stableFlag = false;
                        consecutiveNoiseCounter ++;
                        prevDtemp = dTemp;
                    
                    }else
                    {
                        //Check if there is a consecutive noise...
                        if (abs(dTemp - prevDtemp) > stabilityFactor && stableFlag == false)
                        {
                            consecutiveNoiseCounter >= 2 ?
                            cout<<endl<<"Still not stable... Warning! Possible water / surface issues..."<<endl
                            :
                            cout<<endl<<"Still not stable..."<<endl;  

                            cout<<"dtemp:\t\t"<<dTemp<<"\t prevDtemp:\t"<<prevDtemp<<endl;
                            cout<<"Tolerance jump:\t"<<abs(dTemp - prevDtemp)<<endl<<endl;

                            consecutiveNoiseCounter++;
                            prevDtemp = dTemp;
                        }else
                        {
                            //Stability recovered...
                            cout<<endl<<"Stability ok..."<<endl<<endl;
                            cout<<"dtemp:\t\t"<<dTemp<<"\t prevDtemp:\t"<<prevDtemp<<endl;
                            cout<<"Tolerance jump:\t"<<abs(dTemp - prevDtemp)<<endl<<endl;

                            prevDtemp = dTemp;
                            stableFlag = true;
                            consecutiveNoiseCounter = 0;
                          
                        }
                    }

                    
                }
            }

        } 

    }while (dTemp != -1);
}










int main()
{
    cout<<left<<setfill('*')<<setw(100)<<"*"<<endl<<endl;
	cout<<right<<setfill(' ')<<setw(0)<<"[** WATER DETECTION ALGORITHM STARTED **]"<<endl;
	cout<<endl<<left<<setfill('*')<<setw(100)<<"*"<<endl<<endl;

    /*OFFLINE AND POST PROCESSING SIGNAL TREATMENT*/
    /* cout<<endl<<"MANUAL SIMULATION? [(0) no / (1) yes] --> ";
    int userSelection {};
    cin>>userSelection;
    cout<<endl;

    switch (userSelection)
    {
        case 0:
            readFiles();
        break;
    
        case 1:
            cout << signalTreatmentNoDummy();
        break;

        default:        
            
        break;
    }  */

    //requestDataFromL1();
    readFiles();

    return 0;
}