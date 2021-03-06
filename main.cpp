// sort | (c) 2020 Icinga GmbH | GPLv2+

#include <getopt.h>
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <set>
#include <map>
#include <boost/lexical_cast.hpp>

using std::cout;

template<typename T>
void bubbleSort(std::vector<T> *arr, std::size_t length,
                bool (*funcCallback)(std::string *, std::string *));

template<typename T>
void quickSort(std::vector<T> *arr, int low, int high,
               bool (*funcCallback)(std::string *, std::string *));

template<typename T>
void mergeSort(std::vector<T> *arr, int begin, int vecSize,
               bool (*funcCallback)(std::string *, std::string *));

template<typename T>
void printSortedArray(const std::vector<T> &arr);

bool checkForEquality(std::string *firstNum, std::string *secondNum);

bool checkNumeric(std::string *firstNum, std::string *secondNum);

template<typename T, typename Key>
bool key_exists(const T &container, const Key &key);

template<typename Key, typename Value>
bool findByValue(std::map<Key, Value> mapOfElemen, Value value);

void printHelp();

int main(int argc, char **argv) {
    std::vector<std::string> stringVector;
    std::string fileName;
    std::map<std::string, std::string> mapOption{};
    int options, optionIndex = 0;
    bool (*funcCallback)(std::string *, std::string *) = checkForEquality;
    static struct option $longOptions[] = {
            {"numeric-sort",          no_argument,       nullptr, 'n'},
            {"output",                required_argument, nullptr, 'o'},
            {"unique",                no_argument,       nullptr, 'u'},
            {"ignore-leading-blanks", no_argument,       nullptr, 'b'},
            {"ignore-case",           no_argument,       nullptr, 'f'},
            {"random-sort",           no_argument,       nullptr, 'R'},
            {"reverse",               no_argument,       nullptr, 'r'},
            {"quick-sort",            no_argument,       nullptr, 'q'},
            {"merge-sort",            no_argument,       nullptr, 'm'},
            {"help",                  no_argument,       nullptr, 'h'},
    };

    while ((options = getopt_long(argc, argv, "no:ubfRrqmh",
            $longOptions, &optionIndex)) != -1) {
        switch (options) {
            case 'n':
                mapOption.insert(std::make_pair("n", "numeric-sort"));
                funcCallback = checkNumeric;
                break;
            case 'o':
                mapOption.insert(std::make_pair("o", "output"));
                fileName = optarg;
                break;
            case 'u':
                mapOption.insert(std::make_pair("u", "unique"));
                break;
            case 'b':
                mapOption.insert(std::make_pair("b", "ignore-leading-blanks"));
                break;
            case 'f':
                mapOption.insert(std::make_pair("f", "ignore-case"));
                break;
            case 'R':
                mapOption.insert(std::make_pair("R", "random-sort"));
                break;
            case 'r':
                mapOption.insert(std::make_pair("r", "reverse"));
                break;
            case 'q':
                mapOption.insert(std::make_pair("q", "quick-sort"));
                break;
            case 'm':
                mapOption.insert(std::make_pair("m", "merge-sort"));
                break;
            case 'h':
                mapOption.insert(std::make_pair("h", "help"));
                break;
            default: /* For invalid option e.g '?' */
                exit(EXIT_FAILURE);
        }
    }

    if (key_exists(mapOption, "h")) {
        printHelp();
    }

    if (argv[optind] != nullptr) {
        std::fstream file{argv[optind]};
        for (std::string line; getline(file, line);) {
            if (key_exists(mapOption, "b") && line.empty()) {
                continue;
            }

            stringVector.emplace_back(line);
        }

        file.close();
    } else {
        for (std::string line; getline(std::cin, line);) {
            if (key_exists(mapOption, "b") && line.empty()) {
                continue;
            }

            stringVector.emplace_back(line);
        }
    }

    if (key_exists(mapOption, "u")) {
        std::set<std::string> s(stringVector.begin(), stringVector.end());
        stringVector.assign(s.begin(), s.end());
    } else if (key_exists(mapOption, "f")) {
        for (auto &line : stringVector) {
            for (auto &c : line) {
                c = toupper(c);
            }
        }
    }

    int vectorSize = stringVector.size();

    if (key_exists(mapOption, "R")) {
        std::random_device randomDevice;
        shuffle(stringVector.begin(), stringVector.end(), randomDevice);
    } else if (key_exists(mapOption, "q")) {
        quickSort(&stringVector, 0, vectorSize - 1, funcCallback);
    } else if (key_exists(mapOption, "m")) {
        mergeSort(&stringVector, 0, vectorSize - 1, funcCallback);
    } else {
        bubbleSort(&stringVector, vectorSize, funcCallback);
    }

    if (key_exists(mapOption, "r")) {
        reverse(stringVector.begin(), stringVector.end());
    }

    if (key_exists(mapOption, "o")) {
        std::ofstream file{fileName};
        if (file.is_open() && file.good()) {
            for (const auto &line : stringVector) {
                file << line << std::endl;
            }

            file.close();
        } else {
            printf("Could not open file name %s", fileName.c_str());
            exit(EXIT_FAILURE);
        }
    } else {
        printSortedArray(stringVector);
    }

    return EXIT_SUCCESS;
}

template<typename T>
void bubbleSort(std::vector<T> *arr, std::size_t length,
                bool (*funcCallback)(std::string *, std::string *)) {
    bool isSwapped = true;

    for (std::size_t i{0}; (i < length - 1) && (isSwapped); i++) {
        isSwapped = false;
        for (std::size_t j{0}; j < (length - i) - 1; j++) {
            if (funcCallback(&arr->at(j + 1), &arr->at(j))) {
                swap(arr->at(j + 1), arr->at(j));

                isSwapped = true;
            }
        }
    }
}

template<typename T>
void quickSort(std::vector<T> *arr, int low, int high,
               bool (*funcCallback)(std::string *, std::string *)) {
    if (low < high) {
        std::string pivot = arr->at(high);
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++) {
            if (funcCallback(&arr->at(j), &pivot)) {
                i++;
                swap(arr->at(i), arr->at(j));
            }
        }

        swap(arr->at(i + 1), arr->at(high));

        int newPivot = i + 1;

        quickSort(arr, low, newPivot - 1, funcCallback);
        quickSort(arr, newPivot + 1, high, funcCallback);
    }
}

template<typename T>
void mergeSort(std::vector<T> *arr, int begin, int vecSize,
               bool (*funcCallback)(std::string *, std::string *)) {
    if (begin >= vecSize) {
        return;
    }

    int middle = (begin + vecSize) / 2;
    mergeSort(arr, begin, middle, funcCallback);
    mergeSort(arr, middle + 1, vecSize, funcCallback);

    int i = begin, j = middle + 1;
    int l = vecSize - begin + 1;
    T *tempVec = new T[l];

    for (int k{}; k < l; k++) {
        if (j > vecSize || (i <= middle &&
            funcCallback(&arr->at(i), &arr->at(j)))) {
            tempVec[k] = arr->at(i);
            i++;
        } else {
            tempVec[k] = arr->at(j);
            j++;
        }
    }

    for (int k = 0, n = begin; k < l; k++, n++) {
        arr->at(n) = tempVec[k];
    }

    delete[] tempVec;
}

template<typename T>
void printSortedArray(const std::vector<T> &arr) {
    for (const auto &num : arr) {
        cout << num << std::endl;
    }
}

bool checkForEquality(std::string *firstNum, std::string *secondNum) {
    return *firstNum < *secondNum;
}

bool checkNumeric(std::string *firstNum, std::string *secondNum) {
    if (firstNum->empty() || secondNum->empty()) {
        return false;
    }
    char character2, character1;

    std::string::const_iterator it = firstNum->begin();
    std::string::const_iterator iterator = secondNum->begin();
    while (it != firstNum->end() && isdigit(*it)) ++it;
    while (iterator != secondNum->end() && isdigit(*iterator)) ++iterator;

    if (!firstNum->empty() && it == firstNum->end() &&
        !secondNum->empty() && iterator == secondNum->end()) {
        return boost::lexical_cast<int>(*firstNum) <
                boost::lexical_cast<int>(*secondNum);
    } else {
        for (auto &lin : *firstNum) {
            character1 = lin;
        }

        for (auto &line : *secondNum) {
            character2 = line;
        }

        return static_cast<int>(character1) < static_cast<int>(character2);
    }
}

template<typename T, typename Key>
bool key_exists(const T &container, const Key &key) {
    return container.find(key) != end(container);
}

template<typename Key, typename Value>
bool findByValue(std::map<Key, Value> mapOfElemen, Value value) {
    auto iterator = mapOfElemen.begin();
    while (iterator != mapOfElemen.end()) {
        if (iterator->second == value) {
            return true;
        }

        iterator++;
    }

    return false;
}

void printHelp() {
    cout << "Usage: Program PATH [OPTION] < [FILE] ...\n"
            " or: Program PATH [OPTION] file name\n"
            "If you execute the sort commands without "
            "the o parameter and a new text file,\n"
            "it simply uses the standard output.\n\n";

    cout << "Arguments required for long options "
            "are also required for short ones.\n"
            "Sort options:\n\n";
    cout << "-b, --ignore-leading-blanks    Ignore leading spaces\n"
            "-f, --ignore-case              Treat lower case as upper case\n"
            "-n, --numeric-sort             Sort by numeric value\n"
            "-R, --random-sort              Use a random hash to sort the keys. See shuf(1)\n"
            "-r, --reverse                  Reverse the result of the sorting output\n"
            "-o, --output=FILE              Write result to FILE instead of standard output\n"
            "-m, --merge-sort               Use merge sort Algorithm\n"
            "-u, --unique                   Only the first of output several equals\n"
            "-q, --quick-sort               Use Quick sort Algorithm\n"
            "-h, --help                     Display this help and the program will be exited\n";

    cout << "\n\nif you have any problem executing "
            "commands you can open an issue \n"
            "in github or you can fix it yourself "
            "by opening a pull request \n\n"
            "Thanks for using my sort program :)\n";

    exit(EXIT_SUCCESS);
}
