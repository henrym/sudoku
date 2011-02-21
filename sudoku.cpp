#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <stack>

using namespace std;

class SudokuBoard;
ostream& operator<< (ostream& os, const SudokuBoard& b);

class SudokuBoard
{
public:
    // Denotes an empty square on the sudoku board.
    static const int kEmpty = 0;

public:
    typedef std::pair<int, int> Coord;

public:
    SudokuBoard(const vector<int>& values)
        : values(values), rows_(9), cols_(9)
    {}

    int& operator() (const Coord& coord) {
        return (*this)(coord.first, coord.second);
    }
    int& operator() (const int r, const int c) {
        return values[r*cols_+c];
    }
    const int& operator() (const int r, const int c) const {
        return values[r*cols_+c];
    }

    int rows() const { return rows_; }
    int cols() const { return cols_; }

    void solve() {
        std::stack<Coord> coords;

        int row =0, col =0;
        while (row < rows() && col < cols()) {
            if ((*this)(row, col) == kEmpty) {
                Coord c = make_pair(row, col);
                coords.push(c);

                int* num = &(*this)(row, col);
                (*num)++;
                
                while (true) {

                    //cout << *this << endl;

                    bool valid = is_valid(row, col);
                    if (valid) {
                        // move to next
                        col++;
                        if (col >= cols()) {
                            col = 0;
                            row++;
                        }

                        break;
                    } else {
                        (*num)++;
                        while (*num > 9) {
                            *num = 0;

                            coords.pop();
                            if (coords.empty())
                                throw std::runtime_error("Puzzle is unsolvable");
                            Coord c = coords.top();
                            row = c.first;
                            col = c.second;
                        
                            num = &(*this)(row, col);
                            (*num)++;
                        }
                    }
                }
            } else {
                // move to next
                col++;
                if (col >= cols()) {
                    col = 0;
                    row++;
                }
            }

            //cout << endl << *this << endl;
        }
    }


    /* Returns whether the value at the given square is valid given the current
     * state of the game board.
     */
    bool is_valid(const int row, const int col) const {
        bool valid =     valid_row(row, col);
        valid = valid && valid_col(row, col);
        valid = valid && valid_block(row, col);
        return valid;
    }

private:
    /* Returns whether the row at the given square is valid given the current
     * state of the game board.
     */
    bool valid_row(const int row, const int col) const {
        int value = (*this)(row, col);

        if (value == kEmpty)
            return false;

        for (int c =0; c < cols_; ++c) {
            if (c == col)
                continue;

            if ((*this)(row, c) == value) {
                return false;
            }
        }
        return true;
    }

    /* Returns whether the column at the given square is valid given the current
     * state of the game board.
     */
    bool valid_col(const int row, const int col) const {
        int value = (*this)(row, col);

        if (value == kEmpty)
            return false;

        for (int r =0; r < rows_; ++r) {
            if (r == row)
                continue;

            if ((*this)(r, col) == value) {
                return false;
            }
        }

        return true;
    }

    /* Returns whether the block surrounding the given square is valid given
     * the current state of the game board.
     */
    bool valid_block(const int row, const int col) const {
        int value = (*this)(row, col);
        if (value == kEmpty)
            return false;

        int block_r = row/3;
        int block_c = col/3;

        for (int r =0; r < 3; ++r) {
            for (int c =0; c < 3; ++c) {
                int check_r = 3*block_r + r;
                int check_c = 3*block_c + c;

                if (check_r == row && check_c == col)
                    continue;

                if ((*this)(check_r, check_c) == value) {
                    return false;
                }
            }
        }

        return true;
    }

private:
    std::vector<int> values;
    int rows_, cols_;
};

ostream& operator<< (ostream& os, const SudokuBoard& b)
{
    for (int r =0; r < b.rows(); ++r) {
        for (int c =0; c < b.cols(); ++c) {
            os << b(r, c) << " ";
        }
        os << endl;
    }
    return os;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input-filename>" << endl;
        return EXIT_FAILURE;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Unable to open file \"" << argv[1] << "\"" << endl;
        return EXIT_FAILURE;
    }

    std::vector<int> values;

    for (int i = 9*9; i > 0; --i) {
        if (infile) {
            int square;
            infile >> square;
            values.push_back(square);
        } else {
            cerr << "Premature end of file" << endl;
            break;
        }
    }

    SudokuBoard board(values);
    cout << "Initial board: " << endl;
    cout << board << endl;
    board.solve();
    cout << "Solved board: " << endl;
    cout << board << endl;

    return 0;
}
