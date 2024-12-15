// Define the Query Structure
// First, we need to define the structure to represent a SQL query.

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct Table {
    std::string name;
    int rows; // Number of rows in the table
};

struct Query {
    std::vector<std::string> selectColumns;
    std::vector<Table> fromTables;
    std::vector<std::pair<std::string, std::string>> joinConditions; // (table1.column, table2.column)
};
