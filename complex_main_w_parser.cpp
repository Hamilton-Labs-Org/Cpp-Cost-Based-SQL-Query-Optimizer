/*
Explanation
Define the Query Structure: We define a simple structure to represent the SQL query.
Parse the Query: We implement a basic SQL parser to convert the SQL string into our Query structure. This involves tokenizing the SQL string and extracting the SELECT columns, FROM tables, and WHERE conditions.
Cost-based Optimization: We implement a dynamic programming approach to find the optimal join order based on estimated join costs.
Generate the Optimized Query: We generate the SQL string from the optimized query plan.
Main Function: We put everything together and demonstrate the optimization process.
*/


#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <limits>
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

// Cost-based optimization using dynamic programming
struct Plan {
    std::vector<Table> tables;
    std::vector<std::pair<std::string, std::string>> joins;
    int cost;
};

int estimateJoinCost(const Table& t1, const Table& t2) {
    // Simplified cost estimation: product of row counts
    return t1.rows * t2.rows;
}

Plan optimizeQuery(const Query& query) {
    std::unordered_map<std::string, Table> tableMap;
    for (const auto& table : query.fromTables) {
        tableMap[table.name] = table;
    }

    std::unordered_map<std::string, Plan> dp;
    for (const auto& table : query.fromTables) {
        dp[table.name] = {{table}, {}, table.rows};
    }

    for (size_t i = 1; i < query.fromTables.size(); ++i) {
        std::unordered_map<std::string, Plan> newDp;
        for (const auto& entry : dp) {
            const auto& plan = entry.second;
            for (const auto& table : query.fromTables) {
                if (std::find_if(plan.tables.begin(), plan.tables.end(), [&](const Table& t) { return t.name == table.name; }) != plan.tables.end()) {
                    continue;
                }

                std::vector<Table> newTables = plan.tables;
                newTables.push_back(table);
                std::vector<std::pair<std::string, std::string>> newJoins = plan.joins;
                for (const auto& join : query.joinConditions) {
                    if ((join.first.find(table.name) != std::string::npos && join.second.find(plan.tables[0].name) != std::string::npos) ||
                        (join.second.find(table.name) != std::string::npos && join.first.find(plan.tables[0].name) != std::string::npos)) {
                        newJoins.push_back(join);
                    }
                }

                int newCost = plan.cost + estimateJoinCost(plan.tables[0], table);
                std::string newKey;
                for (const auto& t : newTables) {
                    newKey += t.name + ",";
                }

                if (newDp.find(newKey) == newDp.end() || newDp[newKey].cost > newCost) {
                    newDp[newKey] = {newTables, newJoins, newCost};
                }
            }
        }
        dp = newDp;
    }

    Plan bestPlan = { {}, {}, std::numeric_limits<int>::max() };
    for (const auto& entry : dp) {
        if (entry.second.cost < bestPlan.cost) {
            bestPlan = entry.second;
        }
    }

    return bestPlan;
}

// Generate the Optimized Query
std::string generateOptimizedQuery(const Plan& plan) {
    std::string optimizedQuery = "SELECT ";
    for (const auto& column : plan.tables[0].name) {
        optimizedQuery += column + ", ";
    }
    optimizedQuery.pop_back();
    optimizedQuery.pop_back();

    optimizedQuery += " FROM ";
    for (const auto& table : plan.tables) {
        optimizedQuery += table.name + ", ";
    }
    optimizedQuery.pop_back();
    optimizedQuery.pop_back();

    if (!plan.joins.empty()) {
        optimizedQuery += " WHERE ";
        for (const auto& join : plan.joins) {
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

    Plan optimizedPlan = optimizeQuery(query);

    std::string optimizedQuery = generateOptimizedQuery(optimizedPlan);
    std::cout << "Optimized Query: " << optimizedQuery << std::endl;

    return 0;
}
