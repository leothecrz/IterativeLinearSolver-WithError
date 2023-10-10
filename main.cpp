
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

void printArray(float *arr, int cols) 
{
    std::cout << "ARRAY: " << "\n";
    for (int j = 0; j < cols; j++) 
        std::cout << arr[j] << " ";
    std::cout << std::endl;
}

void printArray(int *arr, int cols) 
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
        
    return true;
}

bool isNegativeNumber(const std::string str)
{
    if(str.empty())
        return false;
    return (str[0] == '-') && (str.find_first_not_of("1234567890", 1) == std::string::npos);
}

int getUserEquationCount()
{
    int stringnum = 0;
    std::string input;
    do{
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

bool stringIsValid(std::string str)
{
    return true;
}

std::pair< std::vector<std::string>, int> manualRows()
{
    std::vector<std::string> stringArray;

    int rows = getUserEquationCount();

    for(int i=0; i<rows; i++)
    {
        std::cout << "\n >";
        std::string in;
        std::getline(std::cin, in, '\n');
        if(!stringIsValid(in))
            exit(1);
        stringArray.push_back(in);
        
    }

    return std::pair(stringArray, rows);
}

std::pair< std::vector<std::string>, int> fromFile()
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

        return std::pair(stringArray, i);
    }
    std::cout << "Failed To open file: " << input << std::endl;
    exit(1);
}

std::pair< std::vector<std::string>, int> getUserInput()
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
                exit(1); // ERROR
        }
        if(isPureNumber(activeSplitString[matrixLength]) || isNegativeNumber(activeSplitString[matrixLength]) )
            bmatrix[i] = stof( activeSplitString[matrixLength] );
        else
            exit(1); // ERROR
    }
    return matrixLength;
}

void jacobiIterative(float error, float* startingVals, float** matrix, int mLength, float* b)
{

    float* kthVals = startingVals;   
    float* nextVals = new float[mLength];
    for(int i=0; i<mLength; i++)
        nextVals[i] = 0;
    
    int kth_iteration = 0;
    while(kth_iteration < 50)
    {

        for(int i=0; i<mLength; i++)
        {
            float alpha = 0;
            for(int j=0; j< mLength; j++)
            {
                if(i== j)
                    continue;
                alpha += matrix[i][j] * kthVals[j]; // k th iteration

            }
            nextVals[i] = ( b[i] - alpha ) / matrix[i][i];
        }
        kthVals = nextVals;
        kth_iteration++;
    }

    printArray(nextVals, mLength);

    delete nextVals;
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

            activeSum += matrix[i][j];
        }   
        if(activeSum > matrix[i][i])
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
        if(input[0] == 'y')
            return true;
        return false;
    }
}

int main(int charc, char** charv)
{
    //int equationCount = getUserEquationCount();
    std::pair< std::vector<std::string>, int>  returnVals =  getUserInput();
    int equationCount = returnVals.second;

    float **coeffiecientMatrix = new float* [equationCount];
    float *solveValues = new float[equationCount];
    int matrixLength = setupCoeffiecientAndBMatrix(coeffiecientMatrix, solveValues, returnVals.first);

    if(checkDiagonal(coeffiecientMatrix, matrixLength))
    {
        //float* outputs = gaussAndSolve(coeffiecientMatrix, solveValues, equationCount);
        float* test = new float[matrixLength];
            for(int i=0; i<matrixLength; i++)
                test[i] = 0;;
        jacobiIterative(10, test, coeffiecientMatrix, matrixLength, solveValues);
        //delete outputs;
    }

    for(int i=0; i<equationCount; i++)
        delete coeffiecientMatrix[i];
    delete coeffiecientMatrix;
    delete solveValues;
    
}

