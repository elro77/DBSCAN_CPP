#pragma once
#include<iostream>
#include<vector>
#include<map>


class AlgorithmDBSCAN
{
private:
	float m_eps;
	unsigned int m_minPts;
	int m_currentCluster;

	std::vector<int> m_clusters;
	std::vector<bool> m_noisePoints;
	std::vector<bool> m_undefinedPoints;
	std::vector<int> actualKeys;
	

	std::map<int, std::vector<int>> m_connectionsMap;
	
public:
	AlgorithmDBSCAN(const float eps_, const unsigned int minPits_, const unsigned int size_);

	std::vector<int> startClustering(std::vector<std::vector<double>> dataset);
private:
	std::vector<int> rangeQuery(std::vector<std::vector<double>> dataset, int pIndex);
	double calcDistance(std::vector<double> p, std::vector<double> q);
	void createGraph();
	void zipGrid();
	void initgridDictionaryVectors();
	void connectNodes();
	void initGraph();
};

