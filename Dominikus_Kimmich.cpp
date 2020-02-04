#include <iostream>
#include "ProcessDataOperationsToResults.h"

int main()
{
	ProcessDataOperationsToResults processDataOperationsToResults;
	if (processDataOperationsToResults.processDataOperations())
	{
		std::cout << "Process file data.xml according to operations specified in file operations.xml.\n";
		std::cout << "Write output to file output.xml \n";
	}
}