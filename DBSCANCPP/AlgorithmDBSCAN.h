#pragma once
#include<iostream>
#include<vector>
#include<map>


class AlgorithmDBSCAN
{
private:
	float m_eps;
	unsigned int m_minPits;
	int m_currentCluster;

	bool* m_clusters;
	bool* m_noisePoints;
	bool* m_undefinedPoints;
	std::vector<int> actualKeys;

	std::map<int, std::vector<int>> m_connectionsMap;

public:
	AlgorithmDBSCAN(const float eps_, const unsigned int minPits_, const unsigned int size_);

	std::vector<int> startClustering();
private:
	void rangeQuery();
	void calcDistance();
	void createGraph();
	void zipGrid();
	void initgridDictionaryVectors();
	void connectNodes();
	void initGraph();

};

