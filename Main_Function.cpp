// Main Function
// Putting it all together:

int main() {
    std::string queryStr = "SELECT column1, column2 FROM table1, table2, table3 WHERE table1.column1 = table2.column1 AND table2.column2 = table3.column2";
    Query query = parseQuery(queryStr);

    std::cout << "Original Query: " << queryStr << std::endl;

    optimizeQuery(query);

    std::string optimizedQuery = generateOptimizedQuery(query);
    std::cout << "Optimized Query: " << optimizedQuery << std::endl;

    return 0;
}
