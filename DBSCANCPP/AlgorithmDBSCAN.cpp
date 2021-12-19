#include "AlgorithmDBSCAN.h"


AlgorithmDBSCAN::AlgorithmDBSCAN(const float eps_, const unsigned int minPits_, const unsigned int size_)
{
	m_eps = eps_;
	m_minPits = minPits_;
	m_currentCluster = -1;
	m_clusters = new bool[size_];
	m_noisePoints = new bool[size_];
	m_undefinedPoints = new bool[size_];
	for (int i = 0; i < size_; i++)
	{
		m_clusters[i] = false;
		m_noisePoints[i] = false;
		m_undefinedPoints[i] = false;
	}
}

std::vector<int> AlgorithmDBSCAN::startClustering()
{
	createGraph();
	return {};
}

void AlgorithmDBSCAN::rangeQuery()
{

}
void AlgorithmDBSCAN::calcDistance()
{

}
void AlgorithmDBSCAN::createGraph()
{
	initgridDictionaryVectors();
	initGraph();

}
void AlgorithmDBSCAN::zipGrid()
{

}
void AlgorithmDBSCAN::initgridDictionaryVectors()
{

}
void AlgorithmDBSCAN::connectNodes()
{

}
void AlgorithmDBSCAN::initGraph()
{

}
