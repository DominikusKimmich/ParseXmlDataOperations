#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

class ProcessDataOperationsToResults
{
public:
	bool processDataOperations(void);
private:
	// Datenstruktur zum Bereithalten der DatensĂ¤tze aus der Datei data.xml
	struct City
	{
		std::string name;
		size_t population;
		double area;
	};
	typedef std::vector<City> DataCity;

	// Datenstruktur zum Bereithalten der Operationen aus der Datei operations.xml
	struct Operation
	{
		std::string name;
		std::string type;
		std::string func;
		std::string attrib;
		std::string filter;
	};

	typedef std::vector<Operation> OperationsCollect;

	struct Result
	{
		std::string name;
		double dValue;
	};
	typedef std::vector<Result> Results;

	void read_data(std::ifstream& inputData, DataCity& dataCity);
	void read_operations(std::ifstream& inputOperations, OperationsCollect& operationsCollect);
	std::string prd(const double x, const int decDigits);
	void write_output(std::ofstream& outputData, Results& results);
	bool checkFilterStringElements(std::string& cityTocheck, std::vector<std::string>& filterStringElements);
	void average(DataCity& dataCity, std::vector<std::size_t>& filterSelectedCitiesIndex, std::string operationName, std::string attrib, Results& results);
	void sum(DataCity& dataCity, std::vector<std::size_t>& filterSelectedCitiesIndex, std::string operationName, std::string attrib, Results& results);
	void min_or_max(DataCity& dataCity, std::vector<std::size_t>& filterSelectedCitiesIndex, std::string operationName, std::string attrib, Results& results, bool bMinMax);
	void apply_operations_to_data(DataCity& dataCity, OperationsCollect& operationsCollect, Results& results);
};