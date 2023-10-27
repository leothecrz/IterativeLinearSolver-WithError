
#include "main.hpp"

void printMatrix(float **matrix, int rows, int cols) 
{
    std::cout << "Matrix: " << "\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) 
            std::cout << matrix[i][j] << " ";
        std::cout << std::endl;
    }
}

template <class T>
void printArray(T *arr, int cols) 
{
    std::cout << "ARRAY: " << "\n";
    for (int j = 0; j < cols; j++) 
        std::cout << arr[j] << " ";
    std::cout << std::endl;
}

bool isPureNumber(const std::string str)
{
    for(int i = 0; i < str.length(); i++ )
    {
        char c = str.at(i);
        if(c < 48 || c >57)
            return false;
    }
    if(str.length() < 1)
        return false;
    return true;
}

bool isNegativeNumber(const std::string str)
{
    if(str.empty())
        return false;
    return (str[0] == '-') && (str.find_first_not_of("1234567890", 1) == std::string::npos);
}

bool stringIsValid(std::string str)
{

    std::regex numPat(R"(^\d+(\s\d+)*$)");
    if(str.size() < 1)
    {
        std::cout << "Empty String Not Valid \n";
        return false;
    }

    if(std::regex_match(str, numPat))
    {
        return true;
    }
    std::cout << "The String Is Not Valid \n";
    return false;
}

std::vector<std::string> splitString(const std::string& str)
{
    std::istringstream stream(str);
    std::vector<std::string> splits;

    do
    {
        std::string active;
        stream >> active;
        if(active != "")
            splits.push_back(active);
    } 
    while (stream);
    
    return splits;
}

int getUserEquationCount()
{
    int stringnum = 0;
    std::string input;
    do
    {
        do
        {
            std::cout << "How many equations will you be inputing?" << std::endl;
            std::getline(std::cin, input);
            std::cout << std::endl;
        } 
        while( !isPureNumber(input) );
        stringnum = stoi(input);
    }
    while(stringnum < 1);
    return stringnum;
}

std::vector<std::string> manualRows()
{
    std::vector<std::string> stringArray;
    int i =0;
    int rows = getUserEquationCount();
    do
    {
        std::cout << "\n >";
        std::string in;
        std::getline(std::cin, in, '\n');
        if(!stringIsValid(in))
            continue;
        i++;
        stringArray.push_back(in);
    } while ( (i != rows) );

    return stringArray;
}

std::vector<std::string> fromFile()
{
    std::vector<std::string> stringArray;
    std::string input;
    std::cout << "Enter file path:" << std::endl;
    std::getline(std::cin, input, '\n');
    std::ifstream inputFile(input);

    if(inputFile.is_open())
    {
        std::string activeString;
        int i = 0;
        while(inputFile.peek() != EOF)
        {
            std::getline(inputFile, activeString);
            if(!stringIsValid(activeString))
                exit(1);
            stringArray.push_back(activeString);
            i++;
        }
        inputFile.close();

        return stringArray;
    }
    std::cout << "Failed To open file: " << input << std::endl;
    exit(1);
}

std::vector<std::string> getUserInput()
{
    int val = -1;
    std::string input;
    do
    {
        do
        {
            std::cout << "Would you like to either:" << std::endl;
            std::cout << "0) Manualy Enter Row Coefficient" << std::endl;
            std::cout << "1) Give the path for a formatted text file." << std::endl;
            std::getline(std::cin, input);
        }
        while(!isPureNumber(input) || input.length() > 1);
        val = stoi(input);
    } while(val != 0 && val != 1);
    
    if(val == 0)
        return manualRows();
    else
        return fromFile();

}

int setupCoeffiecientAndBMatrix(float** matrix, float* bmatrix, std::vector<std::string> stringArrays)
{
    int matrixLength = splitString( stringArrays.at(0) ) .size() - 1; 

    for(int i=0; i<stringArrays.size(); i++)
    {
        matrix[i] = new float[matrixLength];

        std::string active = stringArrays[i] ;
        std::vector<std::string> activeSplitString = splitString( active );

        for(int j=0; j<matrixLength; j++)
        {
            if(isPureNumber(activeSplitString[j]) || isNegativeNumber(activeSplitString[j]))
                matrix[i][j] = stof(activeSplitString[j]);
            else
            {
                std::cout << "ERROR READING MATRIX. (" << i << ", " << j << ")\n";
                for(int z=0; z<matrixLength; z++)
                    delete matrix[z];
                
                delete matrix;
                delete bmatrix;
                exit(1); // ERROR
            }
                
        }
        if(isPureNumber(activeSplitString[matrixLength]) || isNegativeNumber(activeSplitString[matrixLength]) )
            bmatrix[i] = stof( activeSplitString[matrixLength] );
        else
        {
            std::cout << "ERROR READING B-MATRIX. (" << i << ")\n";
            for(int z=0; z<matrixLength; z++)
                delete matrix[z];
            
            delete matrix;
            delete bmatrix;
            exit(1); // ERROR
        }
            
    }
    return matrixLength;
}

float calcLTwoNorm(float* floats, int vectorLength)
{
    float sum = 0;
    for(int i=0; i<vectorLength; i++)    
        sum += floats[i] * floats[i]; 
    return std::sqrt(sum);
}

void jacobiIterative(float stopError, float* startingVals, float** matrix, int mLength, float* b)
{
    float error = 0;
    float* kthVals = startingVals;   
    float* nextVals = new float[mLength];
        for(int i=0; i<mLength; i++)
            nextVals[i] = 0;
    
    int kth_iteration = 0;
    while(kth_iteration < 50)
    {
        for(int i=0; i < mLength; i++)
        {
            float alpha = 0;
            for(int j=0; j < mLength; j++)
            {
                if(i == j)
                    continue;
                alpha += matrix[i][j] * kthVals[j]; // k th iteration
            }
            nextVals[i] = ( b[i] - alpha ) / matrix[i][i];
        }
        error = calcLTwoNorm(kthVals, mLength) - calcLTwoNorm(nextVals, mLength);
        error = std::abs(error);
        error /= calcLTwoNorm(kthVals, mLength);

        std::cout << "ERROR: " << error << "\n";

        for(int i=0; i<mLength; i++)
            kthVals[i] = nextVals[i];
        kth_iteration++;

        if(stopError >= error)
            break;

    }

    if(kth_iteration++ == 50)
    {
        std::cout << "50 Iterations elapsed" << "\n";
    }
    printArray<float>(kthVals, mLength);
    delete nextVals;
}

void gaussSidel(float stopError, float* startingVals, float** matrix, int mLength, float* b)
{
    float error = 0;
    float* kthVals = startingVals;   
    
    int kth_iteration = 0;
    while(kth_iteration < 50)
    {
        float iterationNorm = calcLTwoNorm(kthVals, mLength);
        for(int i=0; i < mLength; i++)
        {
            float alpha = 0;
            for(int j=0; j < mLength; j++)
            {
                if(i == j)
                    continue;
                alpha += matrix[i][j] * kthVals[j]; // k th iteration
            }
            kthVals[i] = ( b[i] - alpha ) / matrix[i][i];
        }

        error = iterationNorm - calcLTwoNorm(kthVals, mLength);
        error = std::abs(error);
        error /= iterationNorm;
        std::cout << "ERROR: " << error << "\n";

        if(stopError >= error)
            break;

        if(++kth_iteration == 50)
        {
            std::cout << "50 Iterations elapsed" << "\n";
        }

    }   
    std::cout << "GAUSS SIEDEL METHOD: " << "\n";
    printArray<float>(kthVals, mLength);
}

bool isDiogonlyDominant(float ** matrix, int length)
{
    float activeSum;

    for(int i=0; i<length; i++)
    {
        activeSum = 0;
        for (int j = 0; j < length ; j++)
        {
            if(j == i)
                continue;

            activeSum +=  std::abs( matrix[i][j] );
        }   
        if(activeSum > std::abs( matrix[i][i] ) )
            return false;
    }
    return true;
}

bool checkDiagonal(float** matrix, int length)
{
    if(isDiogonlyDominant(matrix, length))
    {
        std::cout << "The Matrix IS Diagonaly Dominant" << "\n";
        return true;
    }
    else
    {
        std::cout << "The Matrix is NOT Diagonaly Dominant" << "\n";
        std::cout << "Continue ?(y/n) \n  -";
        std::string input;
        std::getline(std::cin, input);
        if(input[0] == 'y' || input[0] == 'Y')
            return true;
        return false;
    }
}

float* getStartingValues(int num)
{
    std::cout << "Enter the starting values of Xn's:";
    std::string input;
    float* vals = new float[num];
    for(int i =0; i<num; i++)
    {
        std::cout << i << "th value: ";
        std::getline(std::cin, input);

        if(!isPureNumber(input))
        {
            vals[0] = INFINITY;
            return vals;
        }

        vals[i] = std::stof(input);
    }   
    return vals;
}

float getUserMaxError()
{
    std::string errorString;
    float out = INFINITY;
    bool success = false;
    do
    {
        std::cout << "Enter the maxmimu error allowed. \n >>";
        std::getline(std::cin, errorString);
        try
        {
            out = std::stof(errorString);
            success = true;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    } while (!success);
    return out;
    
}

int main(int charc, char** charv)
{
    std::vector<std::string>  returnVals =  getUserInput(); // exits only if file is not found
    int equationCount = returnVals.size();
    float **coeffiecientMatrix = new float* [equationCount];
    float *solveValues = new float[equationCount];
    int matrixLength = setupCoeffiecientAndBMatrix(coeffiecientMatrix, solveValues, returnVals); // on fail releases pointers and exits

    if(checkDiagonal(coeffiecientMatrix, matrixLength))
    {
        float* test = getStartingValues(matrixLength);
        if(test[0] != INFINITY)
        {

            float maxError = getUserMaxError();

            std::cout << "Use Jacobi Method,(Else will use GaussSidel)? (y/n): ";
            std::string in;
            std::getline(std::cin, in);
            
            if(in[0] == 'y' || in[0] == 'Y')
                jacobiIterative(maxError, test, coeffiecientMatrix, matrixLength, solveValues);
            else
                gaussSidel(maxError, test, coeffiecientMatrix, matrixLength, solveValues);

        }
        delete test;
    }

    for(int i=0; i<equationCount; i++)
        delete coeffiecientMatrix[i];
    delete coeffiecientMatrix;
    delete solveValues;
    
}

