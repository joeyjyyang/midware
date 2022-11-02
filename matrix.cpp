#include <cassert>
#include <iostream>
#include <vector>

class Matrix
{
public:
    Matrix() : num_rows_(0), num_cols_(0), matrix_(new std::vector<std::vector<double>>(num_rows_, std::vector<double>(num_cols_, 0)))
    {}

    Matrix(const size_t num_rows, const size_t num_cols, const double initial_value) : num_rows_(num_rows), num_cols_(num_cols), matrix_(new std::vector<std::vector<double>>(num_rows_, std::vector<double>(num_cols_, initial_value))) {}

    Matrix(const Matrix& other) : num_rows_(other.num_rows_), num_cols_(other.num_cols_), matrix_(new std::vector<std::vector<double>>(*other.matrix_))
    {}

    Matrix& operator=(const Matrix& other)
    {
        if (this != &other)
        {
            delete matrix_;
            matrix_ = nullptr;
            num_rows_ = other.num_rows_;
            num_cols_ = other.num_cols_;
            matrix_ = new std::vector<std::vector<double>>(*other.matrix_);
        }

        return *this;
    }

    ~Matrix()
    {
        delete matrix_;
        matrix_ = nullptr;
    }

    Matrix operator*=(Matrix& other)
    {
        assert(num_cols_ == other.num_rows_);



        Matrix product;

        return product;
    }

    double dotProduct(Matrix& other)
    {

    }

    Matrix& crossProduct(Matrix& other)
    {
        
    }

    void print() const
    {
        for (size_t r_i = 0; r_i < num_rows_; r_i++)
        {
            for (size_t c_i = 0; c_i < num_cols_; c_i++)
            {
                std::cout << (*matrix_)[r_i][c_i] << " ";
            }

            std::cout << "\n";
        }
    }

private:
    size_t num_rows_;
    size_t num_cols_;
    std::vector<std::vector<double>>* matrix_;
};

int main(int argc, char* argv[])
{
    Matrix matrix_a(3, 2, 2);
    matrix_a.print();

    Matrix matrix_b(matrix_a);
    matrix_b.print();

    Matrix matrix_c;
    matrix_c = matrix_a;

    Matrix matrix_d(2, 4, 3);
    matrix_a *= matrix_d;
    matrix_a.print();

    return 0;
}