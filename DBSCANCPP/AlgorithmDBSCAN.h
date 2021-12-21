#pragma once
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<thread>
#include <chrono> // for time meassure 

using namespace std::chrono;
using namespace std;

class AlgorithmDBSCAN
{
private:
	double m_eps;
	unsigned int m_minPts;
	int m_currentCluster;

	std::vector<std::vector<int>> m_connectionsArray;
	std::vector<bool> m_isInConnetions;

	std::vector<int> m_clusters;
	std::vector<bool> m_noisePoints;
	std::vector<bool> m_undefinedPoints;
	std::set<int> m_actualKeys;
	

	std::map<int, std::vector<int>> m_connectionsMap;
	std::map<int, std::vector<int>> m_gridDictionaryIndexes;
	std::map<int, std::vector<std::vector<double>>> m_gridDictionaryVectors;
	
public:
	AlgorithmDBSCAN(const double eps_, const unsigned int minPits_, const unsigned int size_);

	std::vector<int> startClustering(std::vector<std::vector<double>> dataset);
private:
	inline std::vector<int> rangeQuery(std::vector<std::vector<double>> dataset, int pIndex);
	inline static double calcDistance(std::vector<double> p, std::vector<double> q);
	void createGraph(std::vector<std::vector<double>> dataset);
	void zipGrid();
	void initgridDictionaryVectors(std::vector<std::vector<double>> dataset);
	static void connectNodes();
	void initGraph();
	inline int calcAvg(std::vector<double> vec);
	inline bool isKeyActual(int key);
	template<class T>
	inline void uniteVectors(std::vector<T> v1, std::vector<T> v2);
	static void searchForConnections(const int startIndex, const int endIndex, const double eps, std::vector<std::vector<double>> vectorList, std::vector<int> indexes, std::map<int, std::vector<int>> *connectionMap);

};

/*
   auto start = high_resolution_clock::now();
   do stuf here()
   auto stop = high_resolution_clock::now();
   seconds duration = duration_cast<seconds>(stop - start);
   cout << "Reading time last: " << duration.count() << " seconds" << endl;
 */