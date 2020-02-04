#include "ProcessDataOperationsToResults.h"

// Funktion zum Einlesen der Datei data.xml
void ProcessDataOperationsToResults::read_data(std::ifstream& inputData, DataCity& dataCity)
{
	using boost::property_tree::ptree;
	ptree pt;
	read_xml(inputData, pt);

	BOOST_FOREACH(ptree::value_type const& v, pt.get_child("data")) {
		if (v.first == "city") {
			City c;
			c.name = v.second.get<std::string>("<xmlattr>.name");
			c.population = v.second.get<int>("<xmlattr>.population");
			c.area = v.second.get<double>("area");
			dataCity.push_back(c);
		}
	}
}

// Funktion zum Einlesen der Datei operations.xml
void ProcessDataOperationsToResults::read_operations(std::ifstream& inputOperations, OperationsCollect& operationsCollect)
{
	using boost::property_tree::ptree;
	ptree pt;
	read_xml(inputOperations, pt);

	BOOST_FOREACH(ptree::value_type const& v, pt.get_child("operations")) {
		if (v.first == "operation") {
			Operation o;
			o.name = v.second.get<std::string>("<xmlattr>.name");
			o.type = v.second.get<std::string>("<xmlattr>.type");
			o.func = v.second.get<std::string>("<xmlattr>.func");
			o.attrib = v.second.get<std::string>("<xmlattr>.attrib");
			o.filter = v.second.get<std::string>("<xmlattr>.filter");
			operationsCollect.push_back(o);
		}
	}
}

// Funktion zum Runden der Results auf zwei Dezimalen in der Methode ProcessDataOperationsToResults::write_output
std::string ProcessDataOperationsToResults::prd(const double x, const int decDigits) {
	std::stringstream ss;
	ss << std::fixed;
	ss.precision(decDigits);
	ss << x;
	return ss.str();
}

//Schreibe die berechneten Ergebnisse in results in die Datei output.xml
void ProcessDataOperationsToResults::write_output(std::ofstream& outputData, Results& results)
{
	using boost::property_tree::ptree;

	static auto formatToWrite = boost::property_tree::xml_writer_make_settings<std::string>(' ', 4);
	ptree pt;
	BOOST_FOREACH(Result & resultLoop1, results)
	{
		ptree pt_temp;
		pt_temp.put("result.<xmlattr>.name", resultLoop1.name);
		pt_temp.put("result", prd(resultLoop1.dValue, 2));

		pt.add_child("results.result", pt_temp.get_child("result"));
	}
	//write_xml_element(std::cout, pt.front().first, pt.front().second, 0, boost::property_tree::xml_writer_make_settings<std::string>(' ', 4));
	write_xml_element(outputData, pt.front().first, pt.front().second, 0, boost::property_tree::xml_writer_make_settings<std::string>(' ', 4));
}

//Überprüfe ob Zeichenketten in filters im aktuellen Namen der Stadt enthalten sind oder nicht
bool ProcessDataOperationsToResults::checkFilterStringElements(std::string& cityTocheck, std::vector<std::string>& filterStringElements)
{
	bool bCheckFilterStringElementsPassed = false;
	size_t found_at = 0;
	for (std::size_t filterStringElementsIndex = 0; filterStringElementsIndex < filterStringElements.size(); ++filterStringElementsIndex)
	{
		found_at = cityTocheck.find(filterStringElements[filterStringElementsIndex], found_at + 1);
		if (found_at != std::string::npos)
		{
			bCheckFilterStringElementsPassed = true;
		}
		else
		{
			return false;
		}
	}
	if (filterStringElements.size() == 0)
	{
		bCheckFilterStringElementsPassed = true;
	}
	return bCheckFilterStringElementsPassed;
}

// Berechne den Durchschnitt der Bevoelkerung oder der Stadtflaeche bzgl der im Filter spezifizierten Staedte
void ProcessDataOperationsToResults::average(DataCity& dataCity, std::vector<std::size_t>& filterSelectedCitiesIndex, std::string operationName, std::string attrib, Results& results)
{
	if (attrib == "population")
	{
		size_t sumPopulation = 0;
		for (std::size_t filterSelectedCitiesIndexIndex = 0; filterSelectedCitiesIndexIndex < filterSelectedCitiesIndex.size(); ++filterSelectedCitiesIndexIndex)
		{
			sumPopulation = sumPopulation + dataCity[filterSelectedCitiesIndex[filterSelectedCitiesIndexIndex]].population;
		}
		Result resultPopulation;
		resultPopulation.name = operationName;
		resultPopulation.dValue = (sumPopulation / filterSelectedCitiesIndex.size());
		results.push_back(resultPopulation);
	}

	if (attrib == "area")
	{
		double sumArea = 0;
		for (std::size_t filterSelectedCitiesIndexIndex = 0; filterSelectedCitiesIndexIndex < filterSelectedCitiesIndex.size(); ++filterSelectedCitiesIndexIndex)
		{
			sumArea = sumArea + dataCity[filterSelectedCitiesIndex[filterSelectedCitiesIndexIndex]].area;
		}
		Result resultArea;
		resultArea.name = operationName;
		resultArea.dValue = (sumArea / filterSelectedCitiesIndex.size());
		results.push_back(resultArea);
	}
}

// Berechne die Summe der Bevoelkerung oder der Stadtflaeche bzgl der im Filter spezifizierten Staedte
void ProcessDataOperationsToResults::sum(DataCity& dataCity, std::vector<std::size_t>& filterSelectedCitiesIndex, std::string operationName, std::string attrib, Results& results)
{
	if (attrib == "population")
	{
		size_t sumPopulation = 0;
		for (std::size_t filterSelectedCitiesIndexIndex = 0; filterSelectedCitiesIndexIndex < filterSelectedCitiesIndex.size(); ++filterSelectedCitiesIndexIndex)
		{
			sumPopulation = sumPopulation + dataCity[filterSelectedCitiesIndex[filterSelectedCitiesIndexIndex]].population;
		}
		Result resultPopulation;
		resultPopulation.name = operationName;
		resultPopulation.dValue = sumPopulation;
		results.push_back(resultPopulation);
	}

	if (attrib == "area")
	{
		double sumArea = 0;
		for (std::size_t filterSelectedCitiesIndexIndex = 0; filterSelectedCitiesIndexIndex < filterSelectedCitiesIndex.size(); ++filterSelectedCitiesIndexIndex)
		{
			sumArea = sumArea + dataCity[filterSelectedCitiesIndex[filterSelectedCitiesIndexIndex]].area;
		}
		Result resultArea;
		resultArea.name = operationName;
		resultArea.dValue = sumArea;
		results.push_back(resultArea);
	}
}

// Berechne das Maximum oder das Minimum der Bevoelkerung oder der Stadtflaeche bzgl der im Filter spezifizierten Staedte
void ProcessDataOperationsToResults::min_or_max(DataCity& dataCity, std::vector<std::size_t>& filterSelectedCitiesIndex, std::string operationName, std::string attrib, Results& results, bool bMinMax)
{
	if (attrib == "population")
	{
		std::vector<size_t>populationVector;
		for (std::size_t filterSelectedCitiesIndexIndex = 0; filterSelectedCitiesIndexIndex < filterSelectedCitiesIndex.size(); ++filterSelectedCitiesIndexIndex)
		{
			populationVector.push_back(dataCity[filterSelectedCitiesIndex[filterSelectedCitiesIndexIndex]].population);
		}
		Result resultPopulation;
		resultPopulation.name = operationName;
		if (bMinMax)
		{
			resultPopulation.dValue = *min_element(populationVector.begin(), populationVector.end());
		}
		else
		{
			resultPopulation.dValue = *max_element(populationVector.begin(), populationVector.end());
		}
		results.push_back(resultPopulation);
	}

	if (attrib == "area")
	{
		std::vector<double>areaVector;
		for (std::size_t filterSelectedCitiesIndexIndex = 0; filterSelectedCitiesIndexIndex < filterSelectedCitiesIndex.size(); ++filterSelectedCitiesIndexIndex)
		{
			areaVector.push_back(dataCity[filterSelectedCitiesIndex[filterSelectedCitiesIndexIndex]].area);
		}
		Result resultPopulation;
		resultPopulation.name = operationName;
		if (bMinMax)
		{
			resultPopulation.dValue = *min_element(areaVector.begin(), areaVector.end());
		}
		else
		{
			resultPopulation.dValue = *max_element(areaVector.begin(), areaVector.end());
		}
		results.push_back(resultPopulation);
	}
}

// Wähle Staedte entspechend der spezifizierten Filter aus und fuehre die in operations.xml spezifizierten Funktionen auf der Auswahl aus
void ProcessDataOperationsToResults::apply_operations_to_data(DataCity& dataCity, OperationsCollect& operationsCollect, Results& results)
{
	std::string dot(".");
	std::string asterisk("*");
	for (std::size_t i = 0; i < operationsCollect.size(); ++i)
	{
		//Analysiere die Lage der Dots im Filter von operations.xml
		std::vector<std::size_t> dotIndices;
		dotIndices.push_back(operationsCollect[i].filter.find(dot));
		while ((dotIndices.back() < operationsCollect[i].filter.find(dot, dotIndices.back() + 1)) && (operationsCollect[i].filter.find(dot, dotIndices.back() + 1) < operationsCollect[i].filter.length()))
		{
			dotIndices.push_back(operationsCollect[i].filter.find(dot, dotIndices.back() + 1));
		}

		// Extrahiere Zeichenketten im Filter von operations.xml
		std::vector<std::string> filterStringElements;
		for (std::size_t dotIndexIndex = 0; dotIndexIndex < (dotIndices.size() - 1); ++dotIndexIndex)
		{
			if (operationsCollect[i].filter.compare(dotIndices[dotIndexIndex] + 1, 1, asterisk) == 0)
			{
				filterStringElements.push_back(operationsCollect[i].filter.substr(dotIndices[dotIndexIndex] + 2, dotIndices[dotIndexIndex + 1] - dotIndices[dotIndexIndex] - 2));
			}
			else
			{
				filterStringElements.push_back(operationsCollect[i].filter.substr(dotIndices[dotIndexIndex] + 1, dotIndices[dotIndexIndex + 1] - dotIndices[dotIndexIndex] - 1));
			}
		}

		// Bestimme ob erste Zeichenkette vor dem ersten Punkt im Filter von operations.xml vorhanden ist
		bool bFilterStringFirstElement = false;
		std::string filterStringFirstElement = "";
		if (dotIndices[0] > 0)
		{
			bFilterStringFirstElement = true;
			filterStringFirstElement = operationsCollect[i].filter.substr(0, dotIndices[0]);
		}

		// Bestimme ob letzte Zeichenkette nach dem letzten Punkt im Filter von operations.xml vorhanden ist
		bool bFilterStringLastElement = false;
		std::string filterStringLastElement = "";

		if (operationsCollect[i].filter.length() - dotIndices.back() > 2)
		{
			bFilterStringLastElement = true;
			if ((operationsCollect[i].filter.compare(dotIndices.back() + 1, 1, asterisk) == 0) && ((operationsCollect[i].filter.length() - dotIndices.back()) > 1))
			{
				filterStringLastElement = operationsCollect[i].filter.substr(dotIndices.back() + 2, operationsCollect[i].filter.length() - dotIndices.back() - 1);
			}
		}

		// Stelle eine Auswahl von Stadten bereit im Fall dass es eine erste Zeichenkette oder eine letzte Zeichenkette im Filter von operations.xml gibt
		std::vector<std::size_t> filterSelectedCitiesIndex;
		if (((bFilterStringFirstElement == true)) || (bFilterStringLastElement == true) && (filterStringLastElement.size() != 0))
		{
			int compareFilterElement;
			for (std::size_t j = 0; j < dataCity.size(); ++j)
			{
				if (bFilterStringFirstElement == true)
				{
					compareFilterElement = dataCity[j].name.compare(0, filterStringFirstElement.length(), filterStringFirstElement);
				}
				if (bFilterStringLastElement == true)
				{
					compareFilterElement = dataCity[j].name.compare(dataCity[j].name.length() - filterStringLastElement.length(), filterStringLastElement.length(), filterStringLastElement);
				}
				if ((compareFilterElement == 0) && (checkFilterStringElements(dataCity[j].name, filterStringElements) == true))
				{
					filterSelectedCitiesIndex.push_back(j);
				}
			}
		}

		// Stelle eine Auswahl von Stadten bereit im Fall dass es weder eine erste Zeichenkette noch eine letzte Zeichenkette im Filter von operations.xml gibt
		if ((bFilterStringFirstElement == false) && (bFilterStringLastElement == false))
		{
			for (std::size_t j = 0; j < dataCity.size(); ++j)
			{
				if (checkFilterStringElements(dataCity[j].name, filterStringElements) == true)
				{
					filterSelectedCitiesIndex.push_back(j);
				}
			}
		}

		// Fuehre die in operations.xml spezifizierten Funktionen fuer Mittelwert, Summe, Minimum oder Maximum auf der oben erstellten Auswahl von Staedten aus und schreibe die results in output.xml
		if (operationsCollect[i].func == "average")
		{
			average(dataCity, filterSelectedCitiesIndex, operationsCollect[i].name, operationsCollect[i].attrib, results);
		}

		if (operationsCollect[i].func == "sum")
		{
			sum(dataCity, filterSelectedCitiesIndex, operationsCollect[i].name, operationsCollect[i].attrib, results);
		}

		if (operationsCollect[i].func == "min")
		{
			min_or_max(dataCity, filterSelectedCitiesIndex, operationsCollect[i].name, operationsCollect[i].attrib, results, true);
		}

		if (operationsCollect[i].func == "max")
		{
			min_or_max(dataCity, filterSelectedCitiesIndex, operationsCollect[i].name, operationsCollect[i].attrib, results, false);
		}

	}
}

//Zusammenfuehrung der gesamten Funktionalitaet
bool ProcessDataOperationsToResults::processDataOperations(void)
{
	// Lade Datei data.xml
	std::ifstream inputData("data.xml");
	

	// Lade Datei operations.xml
	std::ifstream inputOperations("operations.xml");
	
	Results results;

	//Fuehre Berechnungen nur dann aus, wenn data.xml und operations.xml im aktuellen Ordner verfuegbar sind
	if (inputData && inputOperations)
	{
		DataCity dataCity;
		read_data(inputData, dataCity);

		OperationsCollect operationsCollect;
		read_operations(inputOperations, operationsCollect);

		//Verarbeite Data und Operations zu Results
		apply_operations_to_data(dataCity, operationsCollect, results);

		// Schreibe Datei output.xml
		std::ofstream outputData("output.xml");
		write_output(outputData, results);

		return true;
	}
	//Informiere den Benutzer, wenn data.xml und operations.xml nicht im aktuellen Ordner verfuegbar sind
	else
	{
		std::cout << "file data.xml and file operations.xml must be available in current directory to execute this programm properly. \n";
		return false;
	}
	
}