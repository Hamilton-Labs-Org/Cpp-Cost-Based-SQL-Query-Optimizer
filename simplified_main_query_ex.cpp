/*

Simplified Query Execution Planner
In this simplified example, we will focus on a basic heuristic to reorder the tables based on the number of rows.

Directory Structure
query_optimizer/
    ├── main.cpp
*/



#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

// Define the Query Structure
struct Table {
    std::string name;
    int rows; // Number of rows in the table
};

struct Query {
    std::vector<std::string> selectColumns;
    std::vector<Table> fromTables;
    std::vector<std::pair<std::string, std::string>> joinConditions; // (table1.column, table2.column)
};

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Parse the Query
Query parseQuery(const std::string& queryStr) {
    Query query;
    std::istringstream stream(queryStr);
    std::string token;

    // Parse SELECT columns
    stream >> token; // Skip "SELECT"
    std::getline(stream, token, 'F'); // Read until "FROM"
    std::istringstream selectStream(token);
    while (std::getline(selectStream, token, ',')) {
        query.selectColumns.push_back(trim(token));
    }

    // Parse FROM tables
    stream >> token; // Skip "FROM"
    std::getline(stream, token, 'W'); // Read until "WHERE"
    std::istringstream fromStream(token);
    while (std::getline(fromStream, token, ',')) {
        query.fromTables.push_back({trim(token), 1000}); // Default row count for simplicity
    }

    // Parse WHERE conditions
    stream >> token; // Skip "WHERE"
    std::getline(stream, token);
    std::istringstream whereStream(token);
    while (std::getline(whereStream, token, 'A')) { // Split by "AND"
        size_t eqPos = token.find('=');
        if (eqPos != std::string::npos) {
            std::string left = trim(token.substr(0, eqPos));
            std::string right = trim(token.substr(eqPos + 1));
            query.joinConditions.push_back({left, right});
        }
    }

    return query;
}

// Simple heuristic optimization
void optimizeQuery(Query& query) {
    // Sort tables by the number of rows (ascending)
    std::sort(query.fromTables.begin(), query.fromTables.end(), [](const Table& a, const Table& b) {
        return a.rows < b.rows;
    });

    // Reorder join conditions based on the new table order
    std::vector<std::pair<std::string, std::string>> optimizedJoins;
    for (const auto& table : query.fromTables) {
        for (const auto& join : query.joinConditions) {
            if (join.first.find(table.name) != std::string::npos || join.second.find(table.name) != std::string::npos) {
                optimizedJoins.push_back(join);
            }
        }
    }
    query.joinConditions = optimizedJoins;
}

// Generate the Optimized Query
std::string generateOptimizedQuery(const Query& query) {
    std::string optimizedQuery = "SELECT ";
    for (const auto& column : query.selectColumns) {
        optimizedQuery += column + ", ";
    }
    optimizedQuery.pop_back();
    optimizedQuery.pop_back();

    optimizedQuery += " FROM ";
    for (const auto& table : query.fromTables) {
        optimizedQuery += table.name + ", ";
    }
    optimizedQuery.pop_back();
    optimizedQuery.pop_back();

    if (!query.joinConditions.empty()) {
        optimizedQuery += " WHERE ";
        for (const auto& join : query.joinConditions) {
            optimizedQuery += join.first + " = " + join.second + " AND ";
        }
        optimizedQuery.erase(optimizedQuery.size() - 5); // Remove the last " AND "
    }

    return optimizedQuery;
}

// Main Function
int main() {
    std::string queryStr = "SELECT column1, column2 FROM table1, table2, table3 WHERE table1.column1 = table2.column1 AND table2.column2 = table3.column2";
    Query query = parseQuery(queryStr);

    std::cout << "Original Query: " << queryStr << std::endl;

    optimizeQuery(query);

    std::string optimizedQuery = generateOptimizedQuery(query);
    std::cout << "Optimized Query: " << optimizedQuery << std::endl;

    return 0;
}
