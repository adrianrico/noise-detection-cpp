#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

void automatedCalls();
vector <double> readFile(string fileName);
void noiseDetection(double dTemp, double pyroPlant_getMinValue);
void manualSimulation();

// ALL PROTOTYPES MUST BE ABOVE...!

int main()
{
    cout<<left<<setfill('*')<<setw(100)<<"*"<<endl<<endl;
	cout<<right<<setfill(' ')<<setw(0)<<"[** WATER DETECTION ALGORITHM STARTED **]"<<endl;
	cout<<endl<<left<<setfill('*')<<setw(100)<<"*"<<endl;

    cout<<endl<<"MANUAL SIMULATION? [(0) no / (1) yes] --> ";
    int userSelection {};
    cin>>userSelection;
    cout<<endl;

    switch (userSelection)
    {
        case 0:
            automatedCalls();
        break;
            
        case 1:
            manualSimulation();
        break;
    }

    return 0;
}










/**
 * @brief [ Manual input and calls ]
 * 
 */
void manualSimulation()
{
    //Simulate function call reading from L1...
    double manualMeasurement;
    double pyroMinValue {220};
    do
    {
        
        cout<<left<<setfill('-')<<setw(41)<<"-"<<endl; //Just in testing, not to production...!
        cout<<"To end simulation enter [-1]"<<endl;
        cout<<"[ENTER SIMULATED VALUE] --> ";
        cin>>manualMeasurement;
        cout<<left<<setfill('-')<<setw(41)<<"-"<<endl; //Just in testing, not to production...
        noiseDetection(manualMeasurement, pyroMinValue);
    } while (manualMeasurement != -1); 
}










/**
 * @brief 
 * [ Automated processing for all vector measurements ]
 * 
 * @note Generic file to test any signal, just change the content of the file...
 */
void automatedCalls()
{
    //string genericFileName =  "01_Generic - Empty.csv";
    string genericFileName = "00_Generic.csv";

    cout<<"[i] READING FILE..."<<endl<<endl;
    vector <double> measurements = readFile(genericFileName);
    if (measurements.size() > 0)
    {
        for (size_t i = 0; i < measurements.size(); i++)
        {
            noiseDetection(measurements[i],220);
        }
    }else
    {
        cout<<"[i] ERROR! - File is empty, no data found ["<<measurements.size()<<"]"<<endl;
    }
}










/**
 * @brief [ Fill a vector with measurements in the EXCEL file]
 * 
 * @param fileName [ string ]
 * 
 * @return [ vector <double> ] genericSignalVector
 * 
 * @note - All the files MUST exist in the source code project folder...!
 */
vector <double> readFile(string fileName)
{
    // Reserve vector space for the measurements...
    vector <double> genericSignalVector {};

    ifstream openedFile(fileName);
    string fileLine;
                
    getline(openedFile,fileLine);
    while (getline(openedFile,fileLine))
    {
        stringstream    stream(fileLine);
        string          lineValue;
        getline(stream, lineValue);

        //Fill vector from only the generic signal file...
        genericSignalVector.push_back(stod(lineValue)); // --> STOD = Cast from string to double...
    }
    
    return genericSignalVector; 
}










/**
 * @brief 
 * [ Main noise detection logic ]
 * 
 * @param dTemp [double] 
 * @param pyroPlant_getMinValue [double] PYRO min physical measurement device range --> pyroPlant.getMinValue() (HT 220°c - 1600°c) 
 */
void noiseDetection(double dTemp, double pyroPlant_getMinValue)
{   
    //Own internal algorithm variables
    static double prevDtemp {0};

    double stabilityFactor {8};

    static size_t consecutiveNoiseCounter {0};

    static bool stableFlag {true};
    
    /*
    - Validate if the measurement is below the device physical measurement range...
    - In case that the range is not supplied and clamped to 0 added the second validation...
    */
    if (dTemp < pyroPlant_getMinValue || dTemp == 0)
    {
        cout<<"Current measurement:\t"<<dTemp;
        cout<<" NO STRIP DETECTED!\tMeasurement below device low range or clamped to 0..."<<endl;
        
        cout<<endl;
    
        prevDtemp                   = 0;
        consecutiveNoiseCounter     = 0;
        stableFlag                  = true;
         
    }else
    {   
       
        /*
        - Find the first valid measurement above device measurement range...
        - Store value in a variable to start camparisons (static variable)...               
        */
        if (dTemp > pyroPlant_getMinValue && dTemp > 0 && prevDtemp == 0)
        {
            cout<<"CURRENT: "<<dTemp<<"  \tPREVIOUS: "<<prevDtemp;
            cout<<" \t\tSTRIP FOUND! First valid measurement stored..."<<endl;
             
            prevDtemp = dTemp;
             
        }else
        {
            //start checking from the actual measurement vs previous...
            if (prevDtemp !=0)
            {
                //First unstability find...
                if (abs(dTemp - prevDtemp) > stabilityFactor && stableFlag == true)
                {
                    cout<<"CURRENT: "<<dTemp<<"  \tPREVIOUS: "<<prevDtemp;
                    cout<<"  \tTolerance jump: "<<abs(dTemp - prevDtemp);
                    cout<<" \t\tUnstability found...!"<<endl;
                    
                    stableFlag = false;
                    consecutiveNoiseCounter ++;
                    prevDtemp = dTemp;
                
                }else
                {
                    //Check if there is a consecutive noise...
                    if (abs(dTemp - prevDtemp) > stabilityFactor && stableFlag == false)
                    {
                        cout<<"CURRENT: "<<dTemp<<"  \tPREVIOUS: "<<prevDtemp;
                        cout<<"  \tTolerance jump: "<<abs(dTemp - prevDtemp);
                        consecutiveNoiseCounter >= 2 ?
                        cout<<" \t\tStill not stable... Warning! Possible water / surface issues..."<<endl
                        :
                        cout<<" \t\tStill not stable...!"<<endl;

                        consecutiveNoiseCounter++;
                        prevDtemp = dTemp;
                    }else
                    {    
                        //Stability recovered...
                        cout<<"CURRENT: "<<dTemp<<"  \tPREVIOUS: "<<prevDtemp;
                        cout<<"  \tTolerance jump: "<<abs(dTemp - prevDtemp);
                        cout<<" \t\tok..."<<endl;

                        prevDtemp = dTemp;
                        stableFlag = true;
                        consecutiveNoiseCounter = 0;
                      
                    }
                }
            }
        }
    } 
}