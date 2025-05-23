#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <unordered_map>

using namespace std;

// ====================== COLOR DEFINITIONS ======================
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

// ====================== HELPER FUNCTIONS ======================
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void displayHeader(const string &title)
{
    clearScreen();
    cout << BOLD << CYAN << "========================================\n";
    cout << "  CITY ROADS NETWORK SYSTEM\n";
    cout << "========================================\n"
         << RESET;
    cout << BOLD << ">> " << title << "\n";
    cout << CYAN << "----------------------------------------\n"
         << RESET;
}

void pressEnterToContinue()
{
    cout << YELLOW << "\nPress Enter to continue..." << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ====================== City CLASS ======================
class city
{
public:
    string id;
    string name;

    city() : id(""), name("") {}
    city(string i, string n)
        : id(i), name(n) {}
};

// ====================== GRAPH CLASS ======================
class CityGraph
{
public:
    map<string, vector<pair<string, int>>> adjacencyList;
    map<string, city> cities;

    string toLower(const string &s)
    {
        string lower = s;
        transform(lower.begin(), lower.end(), lower.begin(),
                  [](unsigned char c)
                  { return tolower(c); });
        return lower;
    }

public:
    bool cityExists(const string &id)
    {
        return cities.find(id) != cities.end();
    }

    bool roadExists(const string &id1, const string &id2)
    {
        if (!cityExists(id1) || !cityExists(id2))
            return false;

        for (const auto &conn : adjacencyList[id1])
        {
            if (conn.first == id2)
                return true;
        }
        return false;
    }

    bool addcity(const string &id, const string &name)
    {
        if (cityExists(id))
        {
            cout << RED << "[Error]: city with ID " << id << " already exists.\n"
                 << RESET;
            return false;
        }

        vector<city *> cs = searchcities(name);
        if (!cs.empty())
        {
            cout << RED << "[Error]: city with name " << name << " already exists! \n"
                 << RESET;
            return false;
        }

        cities.emplace(id, city(id, name));
        cout << GREEN << "[Success]: city added successfully.\n"
             << RESET;
        return true;
    }

    bool addRoad(const string &id1, const string &id2, double budget)
    {
        if (!cityExists(id1))
        {
            cout << RED << "[Error]: city " << id1 << " doesn't exist.\n"
                 << RESET;
            return false;
        }
        if (!cityExists(id2))
        {
            cout << RED << "[Error]: city " << id2 << " doesn't exist.\n"
                 << RESET;
            return false;
        }
        if (roadExists(id1, id2))
        {
            cout << RED << "[Error]: Road already exists between these cities.\n"
                 << RESET;
            return false;
        }
        if (id1 == id2)
        {
            cout << RED << "[Error]: Cannot connect a city to itself.\n"
                 << RESET;
            return false;
        }
        if (budget <= 0)
        {
            cout << RED << "[Error]: budget must be positive.\n"
                 << RESET;
            return false;
        }

        adjacencyList[id1].emplace_back(id2, budget);
        adjacencyList[id2].emplace_back(id1, budget);
        cout << GREEN << "[Success]: Road connection added successfully.\n"
             << RESET;
        return true;
    }

    bool updatecity(const string &id, const string &newName)
    {
        if (!cityExists(id))
        {
            cout << RED << "[Error]: city " << id << " doesn't exist.\n"
                 << RESET;
            return false;
        }
        cities[id].name = newName;
        cout << GREEN << "[Success]: city updated successfully.\n"
             << RESET;
        return true;
    }

    bool deletecity(const string &id)
    {
        if (!cityExists(id))
        {
            cout << RED << "[Error]: city " << id << " doesn't exist.\n"
                 << RESET;
            return false;
        }

        for (auto &conn : adjacencyList[id])
        {
            string otherId = conn.first;
            auto &otherConns = adjacencyList[otherId];
            otherConns.erase(remove_if(otherConns.begin(), otherConns.end(),
                                       [&id](const pair<string, int> &p)
                                       { return p.first == id; }),
                             otherConns.end());
        }
        adjacencyList.erase(id);
        cities.erase(id);
        cout << GREEN << "[Success]: city deleted successfully.\n"
             << RESET;
        return true;
    }

    bool deleteRoad(const string &id1, const string &id2)
    {
        if (!roadExists(id1, id2))
        {
            cout << RED << "[Error]: No road exists between these cities.\n"
                 << RESET;
            return false;
        }

        auto &conns1 = adjacencyList[id1];
        conns1.erase(remove_if(conns1.begin(), conns1.end(),
                               [&id2](const pair<string, int> &p)
                               { return p.first == id2; }),
                     conns1.end());

        auto &conns2 = adjacencyList[id2];
        conns2.erase(remove_if(conns2.begin(), conns2.end(),
                               [&id1](const pair<string, int> &p)
                               { return p.first == id1; }),
                     conns2.end());

        cout << GREEN << "[Success]: Road connection deleted successfully.\n"
             << RESET;
        return true;
    }

    void generateMatrices(const map<string, vector<pair<string, int>>> &adjacencyList, const string &operation)
    {
        // Step 1: Extract all city IDs
        vector<string> cities;
        for (const auto &pair : adjacencyList)
        {
            cities.push_back(pair.first);
        }

        // Edge case: No cities
        if (this->cities.empty())
        {
            cout << RED << "No cities found" << endl;
            return;
        }

        // Step 2: Sort the cities to ensure consistent order
        sort(cities.begin(), cities.end());

        int size = cities.size();
        unordered_map<string, int> cityIndexMap;
        for (int i = 0; i < size; ++i)
        {
            cityIndexMap[cities[i]] = i;
        }

        // Step 3: Initialize matrices with zeros
        vector<vector<int>> connectionMatrix(size, vector<int>(size, 0));
        vector<vector<int>> budgetMatrix(size, vector<int>(size, 0));

        // Step 4: Populate the matrices
        for (int i = 0; i < size; ++i)
        {
            string currentCity = cities[i];
            const vector<pair<string, int>> &connections = adjacencyList.at(currentCity);

            for (const auto &connection : connections)
            {
                string connectedCity = connection.first;
                if (cityIndexMap.find(connectedCity) != cityIndexMap.end())
                {
                    int j = cityIndexMap[connectedCity];
                    if (currentCity == connectedCity)
                    {
                        continue;
                    }
                    connectionMatrix[i][j] = 1;
                    budgetMatrix[i][j] = connection.second;
                }
            }
        }

        // conditional checking for the matrices print
        if (operation == "ROAD_MATRIX")
        {
            cout << MAGENTA << "\n cities (" << cities.size() << "):\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;

            for (const auto &city : this->cities)
            {
                cout << setw(8) << city.second.id << setw(20) << city.second.name
                     << endl;
            }
            // Step 5: Print the matrices for verification
            cout << "Connection Matrix:" << endl;
            for (const auto &row : connectionMatrix)
            {
                for (const auto &val : row)
                {
                    cout << val << ' ';
                }
                cout << endl;
            }
        }
        else if (operation == "BUDGET_MATRIX")
        {
            cout << MAGENTA << "\n cities (" << cities.size() << "):\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;

            for (const auto &city : this->cities)
            {
                cout << setw(8) << city.second.id << setw(20) << city.second.name
                     << endl;
            }
            cout << "\nBudget Matrix:" << endl;
            for (const auto &row : budgetMatrix)
            {
                for (const auto &val : row)
                {
                    cout << val << ' ';
                }
                cout << endl;
            }
        }
        else if (operation == "CITIES")
        {
            cout << MAGENTA << "\n cities (" << cities.size() << "):\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;

            for (const auto &city : this->cities)
            {
                cout << setw(8) << city.second.id << setw(20) << city.second.name
                     << endl;
            }
        }
        else
        {
            // operation is ALL
            // print the cities, the roads and the matrices
            displayHeader("All data");
            cout << MAGENTA << "\n cities (" << cities.size() << "):\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;
            cout << CYAN << "----------------------------------------\n"
                 << RESET;

            for (const auto &city : cities)
            {
                cout << setw(8) << city.at(0) << setw(20) << city.at(1)
                     << endl;
            }
            cout << "Connection Matrix:" << endl;
            for (const auto &row : connectionMatrix)
            {
                for (const auto &val : row)
                {
                    cout << val << ' ';
                }
                cout << endl;
            }
            cout << "\nBudget Matrix:" << endl;
            for (const auto &row : budgetMatrix)
            {
                for (const auto &val : row)
                {
                    cout << val << ' ';
                }
                cout << endl;
            }
        }
    }
    city *getcity(const string &id)
    {
        if (!cityExists(id))
            return nullptr;
        return &cities[id];
    }

    int *budget(const string &id1, const string &id2)
    {
        if (!roadExists(id1, id2))
            return nullptr;

        for (auto &conn : adjacencyList[id1])
        {
            if (conn.first == id2)
            {
                return &conn.second;
            }
        }
        return nullptr;
    }

    vector<city *> searchcities(const string &query)
    {
        vector<city *> results;
        string lowerQuery = toLower(query);

        for (auto &city : cities)
        {
            string lowerName = toLower(city.second.name);
            string lowerIndex = toLower(city.second.id);

            if (lowerName.find(lowerQuery) != string::npos ||
                lowerIndex.find(lowerQuery) != string::npos)
            {
                results.push_back(&city.second);
            }
        }
        return results;
    }

    void generateGraphvizDOT(const string &filename)
    {
        ofstream dotFile(filename);

        if (!dotFile.is_open())
        {
            cerr << RED << "[Error]: Could not open file " << filename << RESET << endl;
            return;
        }

        dotFile << "graph cityNetwork {\n";
        dotFile << "  node [shape=box, style=filled, fillcolor=lightblue];\n";
        dotFile << "  edge [color=blue];\n";

        // Write nodes
        for (const auto &city : cities)
        {
            string escapedName = city.second.name;
            replace(escapedName.begin(), escapedName.end(), '"', '\'');
            dotFile << "  \"" << city.second.id << "\" [label=\""
                    << city.second.id << "\\n"
                    << escapedName << "\"];\n";
        }

        // Write edges
        for (const auto &entry : adjacencyList)
        {
            for (const auto &connection : entry.second)
            {
                if (entry.first < connection.first)
                {
                    dotFile << "  \"" << entry.first << "\" -- \""
                            << connection.first << "\" [label=\""
                            << connection.second << " billion (s)\"];\n";
                }
            }
        }

        dotFile << "}\n";
        dotFile.close();
        cout << GREEN << "[Success]: Graphviz DOT file saved to " << filename
             << "\nUse 'dot -Tpng " << filename << " -o city_network.png' to render.\n"
             << RESET;
    }

    bool loadFromCSV(const string &filename)
    {
        try
        {

            ifstream csvFile(filename);
            if (!csvFile.is_open())
            {
                cerr << RED << "[Error]: Could not open file " << filename << RESET << endl;
                return false;
            }

            cities.clear();
            adjacencyList.clear();

            string line;
            bool readingcities = false, readingRoads = false;

            while (getline(csvFile, line))
            {
                if (line.empty())
                    continue;

                if (line == "city Data")
                {
                    readingcities = true;
                    readingRoads = false;
                    getline(csvFile, line);
                    continue;
                }
                else if (line == "Road Connections")
                {
                    readingcities = false;
                    readingRoads = true;
                    getline(csvFile, line);
                    continue;
                }

                stringstream ss(line);
                string token;
                vector<string> tokens;

                while (getline(ss, token, ','))
                {
                    tokens.push_back(token);
                }

                if (readingcities && tokens.size() == 4)
                {
                    try
                    {
                        addcity(tokens[0], tokens[1]);
                    }
                    catch (...)
                    {
                        cerr << RED << "[Error]: Invalid city data in line: " << line << RESET << endl;
                    }
                }
                else if (readingRoads && tokens.size() == 5)
                {
                    try
                    {
                        int budget = stoi(tokens[4]);
                        addRoad(tokens[0], tokens[2], budget);
                    }
                    catch (...)
                    {
                        cerr << RED << "[Error]: Invalid road data in line: " << line << RESET << endl;
                    }
                }
            }

            csvFile.close();
            cout << GREEN << "[Success]: Data loaded from " << filename << RESET << endl;
            return true;
        }
        catch (...)
        {
            cerr << RED << "[Error]: Error occurred while loading data from CSV " << RESET << endl;
            return false;
        }
    }

    void printGraph()
    {
        displayHeader("Network Summary");

        cout << MAGENTA << "\n cities (" << cities.size() << "):\n"
             << RESET;
        cout << CYAN << "----------------------------------------\n"
             << RESET;
        cout << CYAN << "----------------------------------------\n"
             << RESET;

        for (const auto &city : cities)
        {
            cout << setw(8) << city.second.id << setw(20) << city.second.name
                 << endl;
        }

        cout << BLUE << "\n  ROAD CONNECTIONS (" << adjacencyList.size() << "):\n"
             << RESET;
        cout << CYAN << "----------------------------------------\n"
             << RESET;
        cout << BOLD << left << setw(20) << "From city" << setw(20) << "To city"
             << "budget (billion)\n"
             << RESET;
        cout << CYAN << "----------------------------------------\n"
             << RESET;

        for (const auto &entry : adjacencyList)
        {
            for (const auto &connection : entry.second)
            {
                if (entry.first < connection.first)
                {
                    cout << setw(20) << cities[entry.first].name + " (" + entry.first + ")"
                         << setw(20) << cities[connection.first].name + " (" + connection.first + ")"
                         << connection.second << endl;
                }
            }
        }

        cout << YELLOW << "\n[Warning]:  UNCONNECTED cities:\n"
             << RESET;
        cout << CYAN << "----------------------------------------\n"
             << RESET;
        bool anyUnconnected = false;
        for (const auto &city : cities)
        {
            if (adjacencyList.find(city.first) == adjacencyList.end())
            {
                cout << "• " << city.second.name << " (" << city.first << ")\n";
                anyUnconnected = true;
            }
        }
        if (!anyUnconnected)
        {
            cout << GREEN << "All cities are connected!\n"
                 << RESET;
        }
    }

    void saveToCSV(const string &filename)
    {
        ofstream csvFile(filename);

        if (!csvFile.is_open())
        {
            cerr << RED << "[Error]: Could not open file " << filename << RESET << endl;
            return;
        }

        csvFile << "city Data\n";
        csvFile << "ID,Name\n";
        for (const auto &city : cities)
        {
            csvFile << city.second.id << ","
                    << city.second.name << "\n";
        }

        csvFile << "\nRoad Connections\n";
        csvFile << "From ID,From Name,To ID,To Name,budget (billion)\n";
        for (const auto &entry : adjacencyList)
        {
            for (const auto &connection : entry.second)
            {
                if (entry.first < connection.first)
                {
                    csvFile << entry.first << ","
                            << cities[entry.first].name << ","
                            << connection.first << ","
                            << cities[connection.first].name << ","
                            << connection.second << "\n";
                }
            }
        }

        csvFile.close();
        cout << GREEN << "[Success]: Data saved to " << filename << RESET << endl;
    }

    void saveToText(const string &filename, const string &filename2)
    {
        ofstream textFile(filename);
        ofstream textFile2(filename2);

        if (!textFile.is_open())
        {
            cerr << RED << "❌ Error: Could not open file " << filename << RESET << endl;
            return;
        }

        if (!textFile2.is_open())
        {
            cerr << RED << "❌ Error: Could not open file " << filename2 << RESET << endl;
            return;
        }

        // Cities table
        textFile << "=== CITY LIST ===\n\n";
        textFile << "Index | City Name\n";
        textFile << "-----------------\n";
        for (const auto &city : cities)
        {
            textFile << city.second.id << "     | " << city.second.name << endl;
        }

        // Roads table
        textFile2 << "=== ROAD CONNECTIONS ===\n\n";
        textFile2 << "Nbr | Road                             | Budget\n";
        textFile2 << "-----------------------------------------------\n";

        int roadCounter = 1;
        for (const auto &entry : adjacencyList)
        {
            for (const auto &connection : entry.second)
            {
                if (entry.first < connection.first)
                {
                    string from = cities[entry.first].name + " (" + entry.first + ")";
                    string to = cities[connection.first].name + " (" + connection.first + ")";
                    string road = from + " <--> " + to;
                    textFile2 << setw(3) << roadCounter++ << " | "
                              << setw(32) << left << road
                              << "| " << connection.second << " billion (s)\n";
                }
            }
        }

        // Unconnected cities
        textFile2 << "\n=== UNCONNECTED CITIES ===\n\n";
        bool anyUnconnected = false;
        for (const auto &city : cities)
        {
            if (adjacencyList.find(city.first) == adjacencyList.end())
            {
                textFile2 << city.second.id << " (" << city.second.name << ")\n";
                anyUnconnected = true;
            }
        }
        if (!anyUnconnected)
        {
            textFile2 << "All cities are connected!\n";
        }

        textFile.close();
        textFile2.close();

        cout << GREEN << "✅ Data saved to files: " << filename << " and " << filename2 << RESET << endl;
    }
};

// ====================== INPUT FUNCTIONS ======================
int getIntegerInput(const string &prompt, int min = 0)
{
    int value;
    while (true)
    {
        cout << BOLD << prompt << RESET;
        cin >> value;
        if (cin.fail() || value < min)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Invalid input. Please enter a number";
            if (min > 0)
                cout << " >= " << min;
            cout << ".\n"
                 << RESET;
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

string getStringInput(const string &prompt, bool toLowercase = false)
{
    string input;
    cout << BOLD << prompt << RESET;
    getline(cin, input);
    if (toLowercase)
    {
        transform(input.begin(), input.end(), input.begin(),
                  [](unsigned char c)
                  { return tolower(c); });
    }
    return input;
}

void displaycity(const city *city)
{
    if (city == nullptr)
    {
        cout << RED << "[city] not found.\n"
             << RESET;
        return;
    }
    cout << MAGENTA << "\n CITY DETAILS:\n"
         << RESET;
    cout << CYAN << "----------------------------------------\n"
         << RESET;
    cout << left << setw(15) << "ID:" << city->id << "\n";
    cout << setw(15) << "Name:" << city->name << "\n";
    cout << CYAN << "----------------------------------------\n"
         << RESET;
}

// ====================== MAIN MENU ======================
void showMainMenu()
{
    displayHeader("Main Menu");
    cout << BOLD << "1. " << RESET << "Add a city\n";
    cout << BOLD << "2. " << RESET << "Add a road connection\n";
    cout << BOLD << "3. " << RESET << "Update city information\n";
    cout << BOLD << "4. " << RESET << "Delete a city\n";
    cout << BOLD << "5. " << RESET << "Delete a road connection\n";
    cout << BOLD << "6. " << RESET << "View city details\n";
    cout << BOLD << "7. " << RESET << "View road details\n";
    cout << BOLD << "8. " << RESET << "View network summary\n";
    cout << BOLD << "9. " << RESET << "Save data to files\n";
    cout << BOLD << "10. " << RESET << "Search cities\n";
    cout << BOLD << "11. " << RESET << "Generate visualization\n";
    cout << BOLD << "12. " << RESET << "Display road matrix\n";
    cout << BOLD << "13. " << RESET << "Display budget matrix\n";
    cout << BOLD << "14. " << RESET << "Display all data\n";
    cout << BOLD << "15 " << RESET << "Display all cities\n";
    cout << BOLD << "16 " << RESET << "Exit\n";
    cout << CYAN << "----------------------------------------\n"
         << RESET;
}

int main()
{
    CityGraph cityNetwork;

    // Attempt to load data at startup
    cityNetwork.loadFromCSV("cities.csv");

    while (true)
    {
        showMainMenu();
        int choice = getIntegerInput("Enter your choice (1-12): ");

        switch (choice)
        {
        case 1:
        {
            // first get the number of cities
            // displayHeader("Enter number of cities to be added");
            int n = getIntegerInput("Enter number of cities to be added: ");
            for (int i = 1; i <= n; i++)
            {

                displayHeader("Add city");
                string id = getStringInput("Enter city ID: ");
                string name = getStringInput("Enter city name: ");
                cityNetwork.addcity(id, name);
                pressEnterToContinue();
            }
            break;
        }
        case 2:
        {
            displayHeader("Add Road Connection");
            string id1 = getStringInput("Enter first city ID: ");
            string id2 = getStringInput("Enter second city ID: ");
            int budget = getIntegerInput("Enter budget (billion): ", 1);
            cityNetwork.addRoad(id1, id2, budget);
            pressEnterToContinue();
            break;
        }
        case 3:
        {
            displayHeader("Update city");
            string id = getStringInput("Enter city ID to update: ");
            string newName = getStringInput("Enter new name (leave blank to keep current): ");
            city *city = cityNetwork.getcity(id);
            if (city)
            {
                if (newName.empty())
                    newName = city->name;
                cityNetwork.updatecity(id, newName);
            }
            pressEnterToContinue();
            break;
        }
        case 4:
        {
            displayHeader("Delete city");
            string id = getStringInput("Enter city ID to delete: ");
            cityNetwork.deletecity(id);
            pressEnterToContinue();
            break;
        }
        case 5:
        {
            displayHeader("Delete Road Connection");
            string id1 = getStringInput("Enter first city ID: ");
            string id2 = getStringInput("Enter second city ID: ");
            cityNetwork.deleteRoad(id1, id2);
            pressEnterToContinue();
            break;
        }
        case 6:
        {
            displayHeader("View city");
            string id = getStringInput("Enter city ID: ");
            displaycity(cityNetwork.getcity(id));
            pressEnterToContinue();
            break;
        }
        case 7:
        {
            displayHeader("View Road Details");
            string id1 = getStringInput("Enter first city ID: ");
            string id2 = getStringInput("Enter second city ID: ");
            int *budget = cityNetwork.budget(id1, id2);
            if (budget)
            {
                cout << BLUE << "\n  ROAD DETAILS:\n"
                     << RESET;
                cout << CYAN << "----------------------------------------\n"
                     << RESET;
                cout << "From: " << cityNetwork.getcity(id1)->name << " (" << id1 << ")\n";
                cout << "To:   " << cityNetwork.getcity(id2)->name << " (" << id2 << ")\n";
                cout << "budget: " << *budget << " billion (s)\n";
                cout << CYAN << "----------------------------------------\n"
                     << RESET;
            }
            else
            {
                cout << RED << "[No] road exists between these cities.\n"
                     << RESET;
            }
            pressEnterToContinue();
            break;
        }
        case 8:
            cityNetwork.printGraph();
            pressEnterToContinue();
            break;
        case 9:
            displayHeader("Save Data");
            cityNetwork.saveToCSV("cities.csv");
            cityNetwork.saveToText("cities.txt", "roads.txt");
            pressEnterToContinue();
            break;
        case 10:
        {
            displayHeader("Search cities");
            string query = getStringInput("Enter search query: ");
            vector<city *> results = cityNetwork.searchcities(query);
            if (results.empty())
            {
                cout << RED << "[No] cities found matching '" << query << "'.\n"
                     << RESET;
            }
            else
            {
                cout << GREEN << "\nFound " << results.size() << " matching cities:\n"
                     << RESET;
                for (const auto &city : results)
                {
                    displaycity(city);
                }
            }
            pressEnterToContinue();
            break;
        }
        case 11:
            displayHeader("Generate Visualization");
            cityNetwork.generateGraphvizDOT("city_network.dot");
            pressEnterToContinue();
            break;
        case 12:
            displayHeader("Display road matrix");
            cityNetwork.generateMatrices(cityNetwork.adjacencyList, "ROAD_MATRIX");
            pressEnterToContinue();
            break;
        case 13:
            displayHeader("Display budget matrix");
            cityNetwork.generateMatrices(cityNetwork.adjacencyList, "BUDGET_MATRIX");
            pressEnterToContinue();
            break;
        case 14:
            displayHeader("Display all data");
            cityNetwork.generateMatrices(cityNetwork.adjacencyList, "ALL");
            pressEnterToContinue();
            break;
        case 15:
            displayHeader("Display all cities");
            cityNetwork.generateMatrices(cityNetwork.adjacencyList, "CITIES");
            pressEnterToContinue();
            break;
        case 16:
            displayHeader("Exit");
            cout << GREEN << "Saving data before exiting...\n"
                 << RESET;
            cityNetwork.saveToCSV("cities.csv");
            cout << GREEN << "Thank you for using city Network Management System.\n";
            cout << "Goodbye!\n"
                 << RESET;
            return 0;
        default:
            cout << RED << "[Invalid] choice. Please try again.\n"
                 << RESET;
            pressEnterToContinue();
        }
    }
    return 0;
}