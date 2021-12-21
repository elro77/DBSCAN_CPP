#include <iostream>
#include <chrono> // for time meassure 
#include <fstream>
#include<vector>
#include<map>
#include<string>
#include<thread>

class Silhouette
{
private:
	std::map<int, std::vector<int>> m_clusterDictionaryIndexes;
	std::map<int, std::vector<std::vector<double>>> m_clusterDictionaryVectors;

	std::map<int, std::vector<double>> m_clusterGravityPointDictionary;
	std::map<int, int> m_clusterPairsDictionary;

	std::vector<std::vector<int>> m_clusters;


public:
	Silhouette();
	double calculateSilhouetteValue(std::vector<std::vector<double>> dataset, std::vector<int> clusters);

private:
	void createClustersDictionaryIndexes(std::vector<std::vector<double>> dataset, std::vector<int> clusters);
	void createClusterGravityPoint(std::vector<std::vector<double>> dataset);
	void findClusterPairs();
	void calculateAvgSilhoueteOfCluster();
	void calculateClusterAValue();
	void calculateBValues();
	static double calcDistance(std::vector<double> p, std::vector<double> q);
	static void workFindPair(const int startIndex, const int endIndex, std::map<int, int>* clusterPairs , std::map<int, std::vector<double>> clusterGravityPointDictionary);

};

