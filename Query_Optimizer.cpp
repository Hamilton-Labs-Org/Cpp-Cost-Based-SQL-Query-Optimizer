// Optimize the Query
// A simple optimization technique is to reorder the joins based on the number of rows in the tables (smaller tables first).

void optimizeQuery(Query& query) {
    // Sort tables by the number of rows (ascending)
    std::sort(query.fromTables.begin(), query.fromTables.end(), [](const Table& a, const Table& b) {
        return a.rows < b.rows;
    });

    // Reorder join conditions based on the new table order
    // This is a simplified heuristic; real optimizers use more complex algorithms.
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
