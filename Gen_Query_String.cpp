// Generate the Optimized Query
// Finally, we generate the optimized query string.

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
