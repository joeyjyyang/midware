#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

class Matrix
{
public:
    // Alias for convenience.
    using Matrix_t = std::vector<std::vector<double>>;

    // Default constructor with no parameters.
    Matrix() : num_rows_(0), num_cols_(0), matrix_(std::make_unique<Matrix_t>(num_rows_, std::vector<double>(num_cols_, 0)))
    {}

    // Default constructor with desired dimensions and same initial value parameters.
    Matrix(const size_t num_rows, const size_t num_cols, const double initial_value) : num_rows_(num_rows), num_cols_(num_cols), matrix_(std::make_unique<Matrix_t>(num_rows_, std::vector<double>(num_cols_, initial_value))) {}

    // Default constructor with desired dimensions and initial value parameters.
    Matrix(const Matrix_t& matrix) : num_rows_(matrix.size()), num_cols_(matrix[0].size()), matrix_(std::make_unique<Matrix_t>(matrix)) {}

    // Copy constructor
    Matrix(const Matrix& other) : num_rows_(other.num_rows_), num_cols_(other.num_cols_), matrix_(std::make_unique<Matrix_t>(*other.matrix_))
    {}

    // Copy assignment operator.
    Matrix& operator=(const Matrix& other)
    {
        if (this != &other)
        {
            num_rows_ = other.num_rows_;
            num_cols_ = other.num_cols_;
            matrix_ = std::make_unique<Matrix_t>(*other.matrix_);
        }

        return *this; 
    }

    // Move constructor.
    Matrix(Matrix&& other) = default;

    // Move assignment operator.
    Matrix& operator=(Matrix&& other) = default;

    // Destructor.
    ~Matrix() = default;

    // Matrix multiplication with matrix parameter.
    Matrix operator*(Matrix& other)
    {
        if (num_cols_ != other.num_rows_)
        {
            throw std::invalid_argument("Number of columns in left matrix do not equal number of rows in right matrix.");
        }

        Matrix product(num_rows_, other.num_cols_, 0);

        for (size_t i = 0; i < num_rows_; i++) 
        {
            for (size_t j = 0; j < other.num_cols_; j++) 
            {
                for (size_t k = 0; k < num_cols_; ++k) 
                {
                    (*product.matrix_)[i][j] += ((*matrix_)[i][k] * (*other.matrix_)[k][j]);
                }
            }
        }

        return product;
    }

    // Matrix multiplication with factor parameter.
    Matrix operator*(double factor)
    {
        Matrix product(*this);

        for (size_t i = 0; i < num_rows_; i++)
        {
            for (size_t j = 0; j < num_cols_; j++)
            {
                (*product.matrix_)[i][j] *= factor;
            }
        }

        return product;
    }

    double dotProduct(Matrix& other)
    {

    }

    Matrix crossProduct(Matrix& other)
    {

    }

    void print() const
    {
        std::cout << "Printing matrix: \n";

        for (size_t i = 0; i < num_rows_; i++)
        {
            for (size_t j = 0; j < num_cols_; j++)
            {
                std::cout << (*matrix_)[i][j] << " ";
            }

            std::cout << "\n";
        }
    }

private:
    size_t num_rows_;
    size_t num_cols_;
    std::unique_ptr<Matrix_t> matrix_;
};

int main(int argc, char* argv[])
{
    Matrix matrix_a(3, 2, 2.0);
    matrix_a.print();

    Matrix matrix_b(matrix_a);
    matrix_b.print();

    Matrix matrix_c;
    matrix_c.print();
    matrix_c = matrix_a;
    matrix_c.print();

    std::vector<std::vector<double>> matrix{{1.0, 3.0, 1.0, 2.0}, {2.0, 2.0, 1.5, 1.0}};
    Matrix matrix_d(matrix);
    Matrix matrix_e = matrix_a * matrix_d;
    matrix_e.print();

    Matrix matrix_f = matrix_e * 5;
    matrix_f.print();

    return 0;
}
