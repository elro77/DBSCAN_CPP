// DBSCANCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono> // for time meassure 
#include <fstream>
#include<vector>
#include<string>
#include<thread>
#include "AlgorithmDBSCAN.h"
#include "Silhouette.h"

/* ============ Version comments:
* 
* ==Version 1.00 , naive implementation of DBSCAN:
* 2000 points = 70 seconds with (eps = 3, minPts = 2)
* 2000 points = 70 seconds with (eps = 4, minPts = 2)
* 2000 points = 73 seconds with (eps = 5, minPts = 2)
* 
* 5000 points = FOREVER seconds with (eps = 3, minPts = 2)
* 5000 points = FOREVER seconds with (eps = 4, minPts = 2)
* 5000 points = FOREVER seconds with (eps = 5, minPts = 2)
* 
* ==Version 1.01 , using threads and grid as python:
* 2000 points = 14 seconds with (eps = 3, minPts = 2)
* 2000 points = 14 seconds with (eps = 4, minPts = 2)
* 2000 points = 14 seconds with (eps = 5, minPts = 2)
* 
* 5000 points = 13 seconds with (eps = 3, minPts = 2)
* 5000 points = 87 seconds with (eps = 4, minPts = 2)
* 5000 points = 92 seconds with (eps = 5, minPts = 2)
* 
* 
* * ==Version 1.02 , after optimization of the IDE:
* reading time of 100,000 points is 7 seconds
* 2000 points = 0 seconds with (eps = 3, minPts = 2)
* 2000 points = 1 seconds with (eps = 4, minPts = 2)
* 2000 points = 1 seconds with (eps = 5, minPts = 2)
* 
* 5000 points = 0 seconds with (eps = 3, minPts = 2)
* 5000 points = 7 seconds with (eps = 4, minPts = 2)
* 5000 points = 7 seconds with (eps = 5, minPts = 2)
* 
* 
* 
*  ==Version 1.03 , upgrade running on graph using arrays and removing rangeQuery func even when its inline:
* 2000 points = 0 seconds with (eps = 3, minPts = 2)
* 2000 points = 0 seconds with (eps = 4, minPts = 2)
* 2000 points = 0 seconds with (eps = 5, minPts = 2)
* 				
* 5000 points = 0 seconds with (eps = 3, minPts = 2)
* 5000 points = 0 seconds with (eps = 4, minPts = 2)
* 5000 points = 0 seconds with (eps = 5, minPts = 2)
* 
* 10000 points = 1 seconds with (eps = 3, minPts = 2)
* 10000 points = 1 seconds with (eps = 4, minPts = 2)
* 10000 points = 1 seconds with (eps = 5, minPts = 2)
* 
* 50000 points = 38 seconds with (eps = 3, minPts = 2)
* 50000 points = 38 seconds with (eps = 4, minPts = 2)
* 50000 points = 38 seconds with (eps = 5, minPts = 2)
* 
* 100000 points = 158 seconds with (eps = 3, minPts = 2)
* 100000 points = 158 seconds with (eps = 4, minPts = 2)
* 100000 points = 158 seconds with (eps = 5, minPts = 2)
* 
* 
*  
* 
* 
* 
*/

using namespace std::chrono;
using namespace std;


vector<double> readLineData(string line)
{
	std::vector<double> rowData;
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
	int amountOfPoints = 5000;
    std::string line;
    std::vector<std::vector<double>> dataset;
	AlgorithmDBSCAN* algoDBSCAN;
	std::map<int, std::vector<int>> clustersMap;


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
	cout << "Reading time last: " << duration.count() << " seconds" << endl;



	start = high_resolution_clock::now();

	start = high_resolution_clock::now();
	algoDBSCAN = new AlgorithmDBSCAN(4, 3, dataset.size());
	auto resultClusters = algoDBSCAN->startClustering(dataset);

	stop = high_resolution_clock::now();
	duration = duration_cast<seconds>(stop - start);
	cout << "running DBSCAN time : " << duration.count() << " seconds" << endl;

	start = high_resolution_clock::now();
	Silhouette* silhouette = new Silhouette();
	double silhouetteValue = silhouette->calculateSilhouetteValue(dataset, resultClusters);
	stop = high_resolution_clock::now();
	duration = duration_cast<seconds>(stop - start);
	//cout << "(" << i << ", " << j << ") " << "silhouetteValue =  " << silhouetteValue << "\n";
	cout << "silhouette calculation lasted: " << duration.count() << "\n";
	cout << "Svalue = " << silhouetteValue << "\n";

	


	//print clusters
	for (int i = 0; i < resultClusters.size(); i++)
	{
		long cnt = 0;
		int clusterNumber = resultClusters[i];
		if (clusterNumber != -1)
		{
			cnt++;
			auto it = clustersMap.find(clusterNumber);
			if (it == clustersMap.end())
			{
				std::vector<int> indexes;
				clustersMap.insert({ clusterNumber , indexes });
			}
			clustersMap.find(clusterNumber)->second.push_back(i);

		}
	}

	if (clustersMap.size() != 0)
	{
		for (std::map<int, std::vector<int>>::iterator iter = clustersMap.begin(); iter != clustersMap.end(); ++iter)
		{
			int key = iter->first;
			std::vector<int> indexes = iter->second;
			std::string str;
			for (int i = 0; i < indexes.size(); i++)
			{
				str += to_string(indexes[i]) + ", ";
			}
			
			cout << "( "<<key << " ) : " << str << "\n\n\n";
		}
	}
	else
	{
		cout << "There is no clustering at all\n";
	}
	
    return 0;
}


/*
   auto start = high_resolution_clock::now();
   do stuf here()
   auto stop = high_resolution_clock::now();
   seconds duration = duration_cast<seconds>(stop - start);
   cout << "Reading time last: " << duration.count() << " seconds" << endl;
 */