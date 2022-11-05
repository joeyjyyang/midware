#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename T>
class Matrix
{
public:
    // Alias for convenience.
    using Matrix_t = std::vector<std::vector<T>>;

    // Deleted default (empty) constructor;
    Matrix() = delete;

    // Default constructor.
    Matrix(const size_t num_rows, const size_t num_cols, const T initial_value) : num_rows_(num_rows), num_cols_(num_cols), matrix_(std::make_unique<Matrix_t>(num_rows_, std::vector<T>(num_cols_, initial_value))) {}

    // Default constructor.
    Matrix(const Matrix_t& matrix) : num_rows_(matrix.size()), num_cols_(matrix[0].size()), matrix_(std::make_unique<Matrix_t>(matrix)) {}

    // Default constructor.
    Matrix(const std::initializer_list<std::initializer_list<T>> matrix) : num_rows_(matrix.size()), num_cols_(matrix.begin()->size()), matrix_(std::make_unique<Matrix_t>(num_rows_, std::vector<T>(num_cols_, 0)))
    {
        size_t i = 0;
        size_t j = 0;

        for (auto it_i = matrix.begin(); it_i < matrix.end(); it_i++)
        {
            for (auto it_j = it_i->begin(); it_j < it_i->end(); it_j++)
            {
                (*matrix_)[i][j] = *it_j;
                j++;
            }
            i++;
            j = 0;
        }
    }

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
            throw std::invalid_argument("Left matrix must be of dimensions A x N, and right matrix must be of dimensions N x B.");
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
    Matrix operator*(T factor)
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

    // Dot product.
    T dotProduct(Matrix& other)
    {
        if (num_rows_ != 1 || other.num_cols_ != 1 || num_cols_ != other.num_rows_)
        {
            throw std::invalid_argument("Left matrix must be vector of dimensions 1 x N, and right matrix must be vector of dimensions N x 1.");
        }

        T dot_product{0};

        for (int i = 0; i < num_cols_; i++)
        {
            dot_product += (*matrix_)[0][i] * (*other.matrix_)[i][0];
        }

        return dot_product;
    }

    // Cross product.
    Matrix crossProduct(Matrix& other)
    {
        if (num_rows_ != 1 || num_cols_ != 3 || other.num_rows_ != 1 || other.num_cols_ != 3)
        {
            throw std::invalid_argument("Matrices must be vectors of dimensions 1 x 3.");
        }

        Matrix cross_product(num_rows_, num_cols_, 0);

        (*cross_product.matrix_)[0][0] = (*matrix_)[0][1] * (*other.matrix_)[0][2] - (*matrix_)[0][2] * (*other.matrix_)[0][1];
        (*cross_product.matrix_)[0][1] = (*matrix_)[0][2] * (*other.matrix_)[0][0] - (*matrix_)[0][0] * (*other.matrix_)[0][2];
        (*cross_product.matrix_)[0][2] = (*matrix_)[0][0] * (*other.matrix_)[0][1] - (*matrix_)[0][1] * (*other.matrix_)[0][0];

        return cross_product;
    }

    // Print matrix.
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

    // Utility method.
    void setValue(const size_t row, const size_t col, const T value)
    {
        if (row < 0 || row >= num_rows_ || col < 0 || col >= num_cols_)
        {
            throw std::invalid_argument("Invalid row or column index.");
        }

        (*matrix_)[row][col] == value;
    }

private:
    size_t num_rows_;
    size_t num_cols_;
    std::unique_ptr<Matrix_t> matrix_;
};

int main(int argc, char* argv[])
{
    {
        Matrix<double> matrix_a(3, 2, 2.0);
        matrix_a.print();

        Matrix<double> matrix_b(3, 2, 2.0);
        matrix_b.print();

        Matrix<double> matrix_c(matrix_b);
        matrix_c.print();

        matrix_a = matrix_c;
        matrix_a.print();

        Matrix<double> matrix_d({{1.0, 3.0, 1.0, 2.0}, {2.0, 2.0, 1.5, 1.0}});
        matrix_d.print();

        Matrix<double> matrix_e = matrix_b * matrix_d;
        matrix_e.print();

        Matrix<double> matrix_f = matrix_e * 5;
        matrix_f.print();
    }

    {
        Matrix<double> matrix_a{{1.0, 1.2, 0.5, 0.2, -0.4}};
        matrix_a.print();
    }

    {
        Matrix<double> matrix_a({{1.0, 1.2, 0.5, 0.2, -0.4}});
        Matrix<double> matrix_b({{1.5}, {2.0}, {0.8}, {0.1}, {-0.5}});
        const double dot_product = matrix_a.dotProduct(matrix_b);
        std::cout << dot_product << "\n";
    }

    {
        Matrix<double> matrix_a({{1.0, 1.2, 0.5}});
        Matrix<double> matrix_b({{1.5, 2.0, 0.8}});
        Matrix<double> matrix_c = matrix_a.crossProduct(matrix_b);
        matrix_c.print();
    }

    return 0;
}
