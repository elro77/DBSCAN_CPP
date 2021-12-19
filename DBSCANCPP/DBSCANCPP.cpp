// DBSCANCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono> // for time meassure 
#include <fstream>
#include<vector>
#include<string>
using namespace std::chrono;
using namespace std;


vector<double> readLineData(string line)
{
	vector<double> rowData;
	std::stringstream stringStream(line);
	double value;
	while (stringStream >> value) {
		rowData.push_back(value);
		if (stringStream.peek() == ',')           
			stringStream.ignore();    
		
	}
	return rowData;
}


int main()
{
    int row = 1, col = 1;
	int count = 0;
	int amountOfPoints = 10000;
    std::string line;
    std::vector<std::vector<double>> dataset;


    cout << "Welcome!\n";
    cout << "Reading Data:\n";

	auto start = high_resolution_clock::now();
	//open file and read its context
    ifstream file("data.txt");
	if (file.is_open())
	{
		int cnt = 0;
		while (file.peek() != EOF && cnt < amountOfPoints)
		{
			cnt++;
			getline(file, line);
			dataset.push_back(readLineData(line));
		}
		file.close();
	}
	else
	{
		cout << "Couldn't open the file\n";
		return 0;
	}
	auto stop = high_resolution_clock::now();
	seconds duration = duration_cast<seconds>(stop - start);
	cout << "Reading time last: " << duration.count() << "seconds" << endl;
    
        
    return 0;
}


/*
   auto start = high_resolution_clock::now();
   do stuf here()
   auto stop = high_resolution_clock::now();
   seconds duration = duration_cast<seconds>(stop - start);
   cout << "Reading time last: " << duration.count() << "seconds" << endl;
 */