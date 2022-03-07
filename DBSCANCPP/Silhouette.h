#include <iostream>
#include <chrono> // for time meassure 
#include <fstream>
#include<vector>
#include<map>
#include<string>
#include<thread>
#include<mutex>

class Silhouette
{
private:
	std::map<int, std::vector<int>> m_clusterDictionaryIndexes;
	std::map<int, std::vector<std::vector<double>>> m_clusterDictionaryVectors;

	std::map<int, std::vector<double>> m_clusterGravityPointDictionary;
	std::map<int, int> m_clusterPairsDictionary;

	std::vector<std::vector<int>> m_clusters;

	std::vector<double> m_listAvgSilhouette;

	std::mutex m_silhouetteMutex;


public:
	Silhouette();
	double calculateSilhouetteValue(std::vector<std::vector<double>> dataset, std::vector<int> clusters);

private:
	void createClustersDictionaryIndexes(std::vector<std::vector<double>> dataset, std::vector<int> clusters);
	void createClusterGravityPoint(std::vector<std::vector<double>> dataset);
	void findClusterPairs();

	static void calculateAvgSilhoueteOfCluster(const int startClusterIndex,const int endClusterIndex, std::vector<double>* listAvgSilhouette, std::map<int, std::vector<double>>* clusterGravityPointDictionary, std::map<int, std::vector<int>>* clusterDictionaryIndexes, std::map<int, std::vector<std::vector<double>>>* clusterDictionaryVectors, std::map<int, int>* clusterPairsDictionary, std::mutex* silhouetteMutex);
	static std::vector<double> calculateClusterAValue(const int clusterNumber, std::map<int, std::vector<double>>* clusterGravityPointDictionary, std::map<int, std::vector<std::vector<double>>>* clusterDictionaryVectors);
	static std::vector<double> calculateBValues(const int clusterNumber, std::map<int, int>* clusterPairsDictionary, std::map<int, std::vector<double>>* clusterGravityPointDictionary, std::map<int, std::vector<std::vector<double>>>* clusterDictionaryVectors);
	static double calcDistance(std::vector<double> p, std::vector<double> q);
	static double calcADistance(std::vector<double> a, std::vector<double> ac, int clusterSize);
	static void workFindPair(const int startIndex, const int endIndex,const int maxSize, std::map<int, int>* clusterPairs , std::map<int, std::vector<double>> clusterGravityPointDictionary);

};

