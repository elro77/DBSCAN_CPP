#pragma once
#include<iostream>
#include<vector>
#include<map>
#include <chrono> // for time meassure 

using namespace std::chrono;
using namespace std;

class AlgorithmDBSCAN
{
private:
	float m_eps;
	unsigned int m_minPts;
	int m_currentCluster;

	std::vector<int> m_clusters;
	std::vector<bool> m_noisePoints;
	std::vector<bool> m_undefinedPoints;
	std::vector<int> m_actualKeys;
	

	std::map<int, std::vector<int>> m_connectionsMap;
	std::map<int, std::vector<int>> m_gridDictionaryIndexes;
	std::map<int, std::vector<std::vector<double>>> m_gridDictionaryVectors;
	
public:
	AlgorithmDBSCAN(const float eps_, const unsigned int minPits_, const unsigned int size_);

	std::vector<int> startClustering(std::vector<std::vector<double>> dataset);
private:
	std::vector<int> rangeQuery(std::vector<std::vector<double>> dataset, int pIndex);
	double calcDistance(std::vector<double> p, std::vector<double> q);
	void createGraph(std::vector<std::vector<double>> dataset);
	void zipGrid();
	void initgridDictionaryVectors(std::vector<std::vector<double>> dataset);
	void connectNodes();
	void initGraph();
	inline int calcAvg(std::vector<double> vec);
};

/*
   auto start = high_resolution_clock::now();
   do stuf here()
   auto stop = high_resolution_clock::now();
   seconds duration = duration_cast<seconds>(stop - start);
   cout << "Reading time last: " << duration.count() << " seconds" << endl;
 */