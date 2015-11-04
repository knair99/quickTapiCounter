
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "Common.h"

using std::cout;        using std::ofstream;
using std::endl;        using std::ifstream;
using std::string;      using std::getline;
using std::vector;      using std::ios;
using std::cin;         using std::ios_base;
using std::unordered_map;


void get_time_stamp(string line, vector<string>& time)
{
        string::size_type pos = 0;
        string::size_type cur = 0;

        string delimiter = ":";

        cur = line.find(delimiter);

        while(cur != string::npos)
        {
            string sTmp = line.substr(pos, cur-pos);

            time.push_back(sTmp);

            pos = cur + 1;

            cur = line.find(delimiter, pos);
        }
        
}


int get_elapsed_seconds(vector<string>& s, vector<string>&e)
{
    int elapsed_seconds;

    int start_hour = atoi( (s[0]).c_str());
    int end_hour = atoi( (e[0]).c_str());

    int start_minute = atoi( (s[1]).c_str());
    int end_minute = atoi( (e[1]).c_str());



    int diff_hours   =  end_hour - start_hour -1;
    int diff_minutes =  (60 -start_minute) + end_minute;
    
    elapsed_seconds = diff_hours * 3600 + diff_minutes * 60;


    return elapsed_seconds;
}

string get_last_line(ifstream& in_file)
{

    string last_line;
    char ch;

    //Travel to the last but one character
    in_file.seekg(-3, ios_base::end);

    while(true)
    {
        in_file.get(ch);
        
        if(ch == '\n' || ch == '\r')
        {
            break;
        }
        else
        {
            in_file.seekg(-2, ios_base::cur);
        }
        
    }

    getline(in_file, last_line);

    return last_line;

}


int calculate_total_calls(ifstream& in_file, string& line)
{
        //string line;
        int count_calls = 0;
        bool false_alarm = false;
        string::size_type pos = 0;
        string::size_type cur = 0;
        string temp_string;

        //Deputize substrings into vector
        unordered_map<string, bool> substrings;
        unordered_map<string, bool>::const_iterator it;

        //Load substrings to id false positives
        substrings[STRING_TSPI_REPLY] = false;
        substrings[STRING_TSPI_ASYNC] = false;
        substrings[STRING_TSPI_EVENT] = false;
        substrings[STRING_TSPI_ROOT]  = true;

        //Read line by line
        while(getline (in_file, line))
        {
                //Check if root string
                pos = line.find(STRING_TSPI_ROOT);
                
                if( pos != string::npos)
                {
                   //Now get just that word until either a colon
                   cur = line.find(":", pos);

                   if(cur != string::npos)
                   {
                     temp_string = line.substr(pos, cur-pos);
                   }
                   else 
                   {
                       cur =line.find(STRING_TSPI_REPLY, pos);

                       if(cur != string::npos)
                       {
                           false_alarm = true;
                       }
                   
                   }

                   it = substrings.find(temp_string);

                   if(it != substrings.end())
                   {
                        if(it->second == false)
                        {
                            false_alarm = true;
                        }
                   }

                   //Increment TAPI call if not false alarm
                    if(!false_alarm)
                    {
                            count_calls += 1;
                    }
                    else
                    {
                            false_alarm = false;
                    }
                }
        }

        return count_calls;
}


int main(int argc, char *argv[])
{

        //Define locals
        string line;
        ifstream in_file;
        vector<string> start_time;
        vector<string> end_time;

        int count_calls = 0;
        int calls_per_second = 0;


        //Fail wrong usage
        if(argc <= 1)
        {
                cout << "Usage: HowBusyIsTapi <TapiSrv-FILENAME>.Log" << endl;
                exit(ERROR_NO_ARGUMENTS);
        }
        else 
        {
                //Open file
                in_file.open(argv[1]);

                //Calculate total
                if(!in_file.is_open())
                {
                        cout << "Error: Unable to open log file" << endl;
                        exit(ERROR_CANNOT_OPEN_FILE);
                }
                else
                {
                        //Skip the first line
                        if(getline(in_file, line))
                        {
                                //Read the second line
                                if(getline(in_file, line))
                                {
                                        //Get start time
                                        get_time_stamp(line, start_time);

                                        cout << "Start time = " << start_time[0] << ":" << start_time[1] << endl;
                                        
                                }
                        }


                        cout << "Opened file: Calculating Traffic : Please wait ..." << endl;

                        count_calls = calculate_total_calls(in_file, line);
                        cout << "Total #TAPI calls: " << count_calls <<endl;


                        //Get the last line
                        in_file.clear();
                        string last_line = get_last_line(in_file);
                        get_time_stamp(last_line, end_time);
                        cout << "End time = " << end_time[0] << ":" << end_time[1] << endl;


                        //Get time difference
                        int elapsed_seconds = get_elapsed_seconds(start_time, end_time);
                        cout << "Time difference is about: " << elapsed_seconds << " seconds" << endl;

                        //Tapi calls/second
                        calls_per_second  = count_calls/elapsed_seconds;
                        cout << "Tapi calls / seconds " << calls_per_second << endl;

                }
        }

        //Close file
        if(in_file.is_open())
        {
                in_file.close();
        }

        return (SUCCESS);

}
