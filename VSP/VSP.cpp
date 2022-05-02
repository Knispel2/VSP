#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <exception>
#include <algorithm>
using namespace std;


class Cord
{
public:
    double x, y;
    int num;
    double cost;
    Cord(double a, double b, int n, double d) : x(a), y(b), num(n), cost(d)
    {}
    friend double dist(Cord a, Cord b);
};

double dist(Cord a, Cord b)
{
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

class TSP_Graph
{
private:
    vector < vector < double >> base;
    vector < vector < double >> base_buf;
    vector <double> min_line;
    vector <double> min_column;
    int ribe_counter = 0;
    vector <bool> black_line;
    vector <bool> black_column;
    double all_cost = 0;
public:

    bool is_finish()
    {
        if (ribe_counter == base.size())
            return true;
        else
            return false;
    }

    double return_cost()
    {
        return all_cost;
    }

    TSP_Graph(vector <Cord>& cords)
    { //за бесконечность положим -1
        base.resize(cords.size(), vector<double>(cords.size(), -1));
        for (int i = 0; i < cords.size(); i++)
            for (int k = 0; k < cords.size(); k++)
                if (i != k)
                    base[k][i] = dist(cords[k], cords[i]);
        min_line.resize(cords.size(), -1);
        min_column.resize(cords.size(), -1);
        black_line.resize(cords.size(), false);
        black_column.resize(cords.size(), false);
        base_buf = base;
    }

    void line_reduction()
    {
        double counter;
        for (int i = 0; i < base.size(); i++)
        {
            if (black_line[i])
                continue;
            counter = -1;
            for (int k = 0; k < base.size(); k++)
                if ((i != k) and (base[i][k] >= 0) and ((counter == -1) or (base[i][k] < counter)))
                    counter = base[i][k];
            min_line[i] = counter;
            for (int k = 0; k < base.size(); k++)
                if ((i != k) and (base[i][k] > 0))
                    base[i][k] -= counter;
        }
    }

    void column_reduction()
    {
        double counter;
        for (int i = 0; i < base.size(); i++)
        {
            if (black_column[i])
                continue;
            counter = -1;
            for (int k = 0; k < base.size(); k++)
                if ((i != k) and (base[k][i] >= 0) and ((counter == -1) or (base[k][i] < counter)))
                    counter = base[k][i];
            min_column[i] = counter;
            for (int k = 0; k < base.size(); k++)
                if ((i != k) and (base[k][i] > 0))
                    base[k][i] -= counter;
        }
    }

    double get_min(int i, int k)
    {
        double result = 0;
        double local_min = -1;
        for (int p = 0; p < base.size(); p++)
            if (((local_min == -1) or (local_min > base[i][p])) and (base[i][p] >= 0) and (p != k))
                local_min = base[i][p];
        result += local_min;
        local_min = -1;
        for (int p = 0; p < base.size(); p++)
            if (((local_min == -1) or (local_min > base[p][k])) and (base[p][k] >= 0) and (p != i))
                local_min = base[i][p];
        result += local_min;
        return result;
    }

    void get_ribe_cost()
    {
        double counter = 0;
        double cost_ribe = 0;
        pair <int, int> buf;
        for (int i = 0; i < base.size(); i++)
            for (int k = 0; k < base.size(); k++)
                if (base[i][k] == 0)
                    if (get_min(i, k) > counter)
                    {
                        buf = make_pair(i, k);
                        cost_ribe = base_buf[i][k];
                    }
        ribe_counter++;
        base[buf.first][buf.second] = -1;
        base[buf.second][buf.first] = -1;
        all_cost += cost_ribe;
    }

    void matrix_reduction()
    {
        int counter;
        for (int i = 0; i < base.size(); i++)
        {
            counter = 0;
            for (int k = 0; k < base.size(); k++)
            {
                if (base[i][k] == -1) counter++;
                if (counter >= 2)
                {
                    black_line[i] = true;
                    for (int k = 0; k < base.size(); k++)
                        base[i][k] = -2;
                    break;
                }
            }
        }

        for (int i = 0; i < base.size(); i++)
        {
            counter = 0;
            for (int k = 0; k < base.size(); k++)
            {
                if (base[k][i] == -1) counter++;
                if (counter >= 2)
                {
                    black_column[i] = true;
                    for (int k = 0; k < base.size(); k++)
                        base[k][i] = -2;
                    break;
                }
            }
        }

    }

};



vector <string> list_files(string dir)
{
    vector <string> result;
    for (const auto& entry : filesystem::directory_iterator(dir))
        result.push_back(entry.path().string().substr(5));
    return result;
}




double TSP_eng(vector <Cord> & cords)
{
      TSP_Graph test(cords);
      int count = 0;
      while (!test.is_finish())
        {
           test.line_reduction();
           test.column_reduction();
           test.get_ribe_cost();
           test.matrix_reduction();
           count++;
           cout << count << " edge ended." << flush << endl;
        }
      return test.return_cost();
}




Cord split(string& data, int num, string file_debug = "")
{
    auto pos = data.find(" ");
    int transp;
    if (data.find("  ") != string::npos) transp = 2;
    else transp = 1;
    string buf = data.substr(pos + transp);
    double d = stod(data.substr(0, pos));
    pos = buf.find(" ");
    if (buf.find("  ") != string::npos) transp = 2;
    else transp = 1;
    return Cord(stod(buf.substr(0, pos)), stod(buf.substr(pos + transp)), num, d);
}

int main()
{
        vector <string> data = list_files("data");
        string buf;
        ofstream fout;
        fout.open("result.txt");
        string x;
        int number;
        for (int k = 0; k < data.size(); k++)
        {
            number = 0;
            x = data[k];
            cout << "Starting " << x << endl << flush;
            vector <Cord> cords;
            ifstream file("data/" + x);
            getline(file, buf);
            Cord enterd = split(buf, 0);
            while (getline(file, buf))
            {
                if ((buf == "") or (buf == " ")) continue;
                cords.push_back(split(buf, number));
                number++;
            }
            file.close();
            vector <double> transport(enterd.x, 0);
            double limit = enterd.y;
            int iteration = 0;
            int p = 1;
            double result = 0;
            while (p < cords.size())
            {
                auto center = cords[p];
                vector <Cord> iteration_data;
                iteration_data.push_back(cords[0]);
                sort(cords.begin() + p, cords.end(), [&](Cord a, Cord b)
                    {
                        return dist(a, center) < dist(b, center);
                    });
                while ((transport[iteration] < limit) and (p < cords.size()))
                {
                    transport[iteration] += cords[p].cost;
                    iteration_data.push_back(cords[p]);
                    p++;
                }
                iteration++;
                result += TSP_eng(iteration_data);
            }
            fout << x << ":" << result << endl;
            cout << x << " test finished " << endl << flush;
        }
        fout.close();
}


bool check_perm(const vector <unsigned int>& base)
{
    for (int i = 0; i < base.size(); i++)
        if (base[base[i] - 1] == i + 1) return false;
    return true;
}

double cost_perm(vector <unsigned int>& permutations, vector <Cord>& cords)
{
    double result = 0;
    for (int i = 0; i < permutations.size(); i++)
        result += dist(cords[i + 1], cords[permutations[i] - 1]);
    return result;
}

double brutforce_method(vector <Cord> cords, double result)
{
    vector <unsigned int> permutations(cords.size());
    for (int i = 0; i < cords.size(); i++)
        permutations[i] = i + 1;
    double buf;
    do {
        if (check_perm(permutations))
        {
            buf = cost_perm(permutations, cords);
            if (buf > result)
                result = buf;
        }
    } while (next_permutation(permutations.begin(), permutations.end()));
    return buf;
}


