#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// ---------------------------------------------------------------------------
// Matrix class: a square N x N matrix of long long values.
// Data members are private; all access goes through public member functions.
// Storage is a dynamically allocated array (no std::vector), so the class
// follows the Rule of Three (copy constructor, copy assignment, destructor).
// ---------------------------------------------------------------------------
class Matrix
{
private:
    int size;        // N
    long long *data; // N * N values stored row by row

    int index(int row, int col) const { return row * size + col; }

public:
    // Constructors / destructor
    explicit Matrix(int n = 0) : size(n), data(nullptr)
    {
        if (size > 0)
            data = new long long[size * size]();
    }

    Matrix(const Matrix &other) : size(other.size), data(nullptr)
    {
        if (size > 0)
        {
            data = new long long[size * size];
            copy(other.data, other.data + size * size, data);
        }
    }

    Matrix &operator=(const Matrix &other)
    {
        if (this != &other)
        {
            Matrix temp(other); // copy-and-swap
            swap(size, temp.size);
            swap(data, temp.data);
        }
        return *this;
    }

    ~Matrix() { delete[] data; }

    // Accessors
    int getSize() const { return size; }

    bool isValidIndex(int i) const { return i >= 0 && i < size; }

    long long get(int row, int col) const { return data[index(row, col)]; }

    void set(int row, int col, long long value) { data[index(row, col)] = value; }

    // Row / column swaps (caller is responsible for valid indices)
    void swapRows(int r1, int r2)
    {
        for (int j = 0; j < size; j++)
            swap(data[index(r1, j)], data[index(r2, j)]);
    }

    void swapColumns(int c1, int c2)
    {
        for (int i = 0; i < size; i++)
            swap(data[index(i, c1)], data[index(i, c2)]);
    }

    // Diagonal sums
    long long mainDiagonalSum() const
    {
        long long sum = 0;
        for (int i = 0; i < size; i++)
            sum += get(i, i);
        return sum;
    }

    long long secondaryDiagonalSum() const
    {
        long long sum = 0;
        for (int i = 0; i < size; i++)
            sum += get(i, size - 1 - i);
        return sum;
    }

    // Operator overloading: C = A + B
    Matrix operator+(const Matrix &rhs) const
    {
        if (size != rhs.size)
            throw invalid_argument("Matrix sizes do not match.");
        Matrix result(size);
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                result.set(i, j, get(i, j) + rhs.get(i, j));
        return result;
    }

    // Operator overloading: C = A * B (standard matrix multiplication)
    Matrix operator*(const Matrix &rhs) const
    {
        if (size != rhs.size)
            throw invalid_argument("Matrix sizes do not match.");
        Matrix result(size);
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
            {
                long long sum = 0;
                for (int k = 0; k < size; k++)
                    sum += get(i, k) * rhs.get(k, j);
                result.set(i, j, sum);
            }
        return result;
    }

    // Read size * size values from a stream into this (already sized) matrix.
    friend istream &operator>>(istream &in, Matrix &m)
    {
        for (int i = 0; i < m.size * m.size; i++)
            in >> m.data[i];
        return in;
    }

    // Print with right-aligned columns.
    friend ostream &operator<<(ostream &out, const Matrix &m)
    {
        size_t width = 0;
        for (int i = 0; i < m.size * m.size; i++)
            width = max(width, to_string(m.data[i]).size());

        for (int i = 0; i < m.size; i++)
        {
            for (int j = 0; j < m.size; j++)
            {
                if (j > 0)
                    out << " ";
                out << setw(static_cast<int>(width)) << m.get(i, j);
            }
            out << "\n";
        }
        return out;
    }
};

// ---------------------------------------------------------------------------
// 1. Read N and two N x N matrices from a file.
// ---------------------------------------------------------------------------
bool loadMatrices(const string &filename, Matrix &A, Matrix &B)
{
    ifstream in(filename);
    if (!in)
    {
        cout << "Error: could not open file '" << filename << "'.\n";
        return false;
    }

    int n;
    if (!(in >> n) || n <= 0)
    {
        cout << "Error: the first line must contain a positive integer N.\n";
        return false;
    }

    A = Matrix(n);
    B = Matrix(n);

    if (!(in >> A))
    {
        cout << "Error: not enough values for Matrix A.\n";
        return false;
    }
    if (!(in >> B))
    {
        cout << "Error: not enough values for Matrix B.\n";
        return false;
    }
    return true;
}

// 4. Compute and display the diagonal sums.
void displayDiagonalSums(const Matrix &M, const string &name)
{
    cout << "Diagonal sums for Matrix " << name << ":\n";
    cout << "Main diagonal sum: " << M.mainDiagonalSum() << "\n";
    cout << "Secondary diagonal sum: " << M.secondaryDiagonalSum() << "\n";
}

// 5. Swap two rows. Matrix is passed BY VALUE so the original is unchanged.
//    Defaults: rows 0 and 1.
void swapRows(Matrix M, int r1 = 0, int r2 = 1)
{
    if (!M.isValidIndex(r1) || !M.isValidIndex(r2))
    {
        cout << "Problem 5 - Invalid row indices.\n";
        return;
    }
    M.swapRows(r1, r2);
    cout << "Problem 5 - Rows " << r1 << " and " << r2 << " swapped:\n";
    cout << M;
}

// 6. Swap two columns (by value). Defaults: columns 0 and 1.
void swapColumns(Matrix M, int c1 = 0, int c2 = 1)
{
    if (!M.isValidIndex(c1) || !M.isValidIndex(c2))
    {
        cout << "Problem 6 - Invalid column indices.\n";
        return;
    }
    M.swapColumns(c1, c2);
    cout << "Problem 6 - Columns " << c1 << " and " << c2 << " swapped:\n";
    cout << M;
}

// 7. Update one element (by value). Defaults: row 0, column 0, value 100.
void updateElement(Matrix M, int row = 0, int col = 0, long long value = 100)
{
    if (!M.isValidIndex(row) || !M.isValidIndex(col))
    {
        cout << "Problem 7 - Invalid row or column index.\n";
        return;
    }
    M.set(row, col, value);
    cout << "Problem 7 - Element (" << row << ", " << col << ") set to " << value << ":\n";
    cout << M;
}

int main()
{
    string filename;
    cout << "Enter input filename: ";
    cin >> filename;

    Matrix A, B;
    if (!loadMatrices(filename, A, B))
        return 1;

    // 1. Display both matrices
    cout << "Matrix A:\n"
         << A;
    cout << "Matrix B:\n"
         << B;

    // 2. Addition via operator+
    Matrix sum = A + B;
    cout << "A + B:\n"
         << sum;

    // 3. Multiplication via operator*
    Matrix product = A * B;
    cout << "A * B:\n"
         << product;

    // 4. Diagonal sums
    displayDiagonalSums(A, "A");

    // 5-7. Each call works on a copy; A itself is never modified.

    // Defaults (rows/columns 0 and 1; element (0, 0) = 100)
    swapRows(A);
    swapColumns(A);
    updateElement(A);

    // User-specified values (indices start at 0)
    int r1, r2, c1, c2, row, col;
    long long value;

    cout << "Problem 5 - Enter two row indices to swap: ";
    if (cin >> r1 >> r2)
        swapRows(A, r1, r2);
    else
    {
        cout << "Invalid input for row indices.\n";
        return 1;
    }

    cout << "Problem 6 - Enter two column indices to swap: ";
    if (cin >> c1 >> c2)
        swapColumns(A, c1, c2);
    else
    {
        cout << "Invalid input for column indices.\n";
        return 1;
    }

    cout << "Problem 7 - Enter row index, column index, and new value: ";
    if (cin >> row >> col >> value)
        updateElement(A, row, col, value);
    else
    {
        cout << "Invalid input for the update.\n";
        return 1;
    }

    return 0;
}
