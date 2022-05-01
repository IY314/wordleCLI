#include <array>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define RESET "\x1b[m"
#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define CYAN 36
#define WHITE 37
#define BRIGHT_BLACK 90
#define BRIGHT_RED 91
#define BRIGHT_GREEN 92
#define BRIGHT_YELLOW 93
#define BRIGHT_BLUE 94
#define BRIGHT_MAGENTA 95
#define BRIGHT_CYAN 96
#define BRIGHT_WHITE 97

template <typename T>
std::string color(const T& s, int c) {
    std::ostringstream oss;
    oss << "\x1b[" << c << 'm' << s << RESET;
    return oss.str();
}

std::vector<std::string> getWords(const std::string& filename) {
    std::vector<std::string> vec;
    std::ifstream i(filename);
    if (i) {
        std::string line;
        while (std::getline(i, line)) vec.push_back(line);
    }
    return vec;
}

std::string generateWord(const std::vector<std::string>& words) {
    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<size_t> dist(0, words.size());
    return words[dist(rng)];
}

std::string getGuess(const std::vector<std::string>& words) {
    std::string guess;
    std::cout << '>';
    std::getline(std::cin, guess);
    if (guess.size() != 5 ||
        std::find(words.begin(), words.end(), guess) == words.end())
        return "";

    return guess;
}

std::array<int, 5> validateGuess(const std::string& guess,
                                 const std::string& word) {
    if (guess == word) {
        return {1, 1, 1, 1, 1};
    }
    std::array<int, 5> array{0, 0, 0, 0, 0};
    for (int i = 0; i < 5; ++i) {
        if (guess[i] == word[i]) {
            array[i] = 1;
        } else if (word.find(guess[i]) != std::string::npos) {
            array[i] = -1;
        }
    }

    return array;
}

void display(
    const std::array<std::pair<std::string, std::array<int, 5>>, 6>& guesses,
    int currentGuess) {
    std::cout << "WORDLE\n\n";
    for (int i = 0; i < 6; ++i) {
        if (currentGuess < i) {
            std::cout << color(". . . . .\n", WHITE);
            continue;
        }
        for (int j = 0; j < 5; ++j) {
            int charColor;
            switch (guesses[i].second[j]) {
                case -1:
                    charColor = BRIGHT_YELLOW;
                    break;
                case 0:
                    charColor = BRIGHT_BLACK;
                    break;
                case 1:
                    charColor = BRIGHT_GREEN;
                    break;
            }
            std::cout << color(guesses[i].first[j], charColor) << ' ';
        }
        std::cout << '\n';
    }
}

int main(int argc, const char* const* argv) {
    std::string filename = argc > 1 ? argv[1] : "./words.txt";
    std::vector<std::string> words = getWords(filename);
    if (!words.size()) {
        std::cerr << "Could not open words file '" << filename << "'\n";
        std::exit(1);
    }
    std::string word = generateWord(words);
    std::array<std::pair<std::string, std::array<int, 5>>, 6> guesses;

    int i = 0;
    while (i < 6) {
        std::string guess = getGuess(words);
        if (guess == "") {
            continue;
        }
        std::array<int, 5> validity = validateGuess(guess, word);
        guesses[i] = {guess, validity};
        display(guesses, i);
        if (std::all_of(validity.begin(), validity.end(),
                        [](int i) { return i == 1; })) {
            break;
        }
        ++i;
    }
    std::cout << word << '\n';
}
