#ifndef FOG_GRID_H
#define FOG_GRID_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include "util.h"
#include <glm/glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include "../header/shader_m.h"
using namespace std;
struct FogParams 
{
	glm::vec3 sigma_a;
	glm::vec3 sigma_s;
    float density;
};

class FogGrid 
{

public:
    glm::vec3 max;
    glm::vec3 min;
    glm::vec3 step;
    glm::vec3 num;
    vector<FogParams> params;

    ~FogGrid() {};
    FogGrid(string fileName) {
        fstream newfile;
        vector<string> lineExtracted;
        newfile.open(fileName, ios::in); //open a file to perform read operation using file object
        if (newfile.is_open()) { //checking whether the file is open
            string tp;
            bool paramState = false,foundA = false,foundS = false;
            // Not so fault tolerance if file's structure is wrong.
            // No need to trim space since size of exceed lineExtracted is not affect.
            FogParams tmpFog;
            while (getline(newfile, tp)) { //read data from file object and put it into string.
                lineExtracted = Util::splitString(tp, this->delimiter);
                if (paramState) {
                    
                    if (lineExtracted[0] == "a") {
                        tmpFog.sigma_a = glm::vec3(stof(lineExtracted[1]), stof(lineExtracted[2]), stof(lineExtracted[3]));
                        foundA = true;
                    }
                    else if (lineExtracted[0] == "s") { // s
                        tmpFog.sigma_s = glm::vec3(stof(lineExtracted[1]), stof(lineExtracted[2]), stof(lineExtracted[3]));
                        foundS = true;
                    }
                    else if (lineExtracted[0] == "d") {
                        if (lineExtracted.size() == 2)
                            tmpFog.density = stof(lineExtracted[1]);
                        else 
                            tmpFog.density = 1.0f;
                        if (!foundA) tmpFog.sigma_a = this->params[0].sigma_a;
                        if (!foundS) tmpFog.sigma_s = this->params[0].sigma_s;
                        foundA = true;
                        foundS = true;
                        this->params.push_back(tmpFog);
                        foundA = false;
                        foundS = false;
                    }
                }
                else if (lineExtracted[0] == "max") {
                    this->max = glm::vec3(stof(lineExtracted[1]), stof(lineExtracted[2]), stof(lineExtracted[3]));
                }
                else if (lineExtracted[0] == "min") {
                    this->min = glm::vec3(stof(lineExtracted[1]), stof(lineExtracted[2]), stof(lineExtracted[3]));
                }
                else if (lineExtracted[0] == "num") {
                    this->num = glm::vec3(stoi(lineExtracted[1]), stoi(lineExtracted[2]), stoi(lineExtracted[3]));
                    this->step = glm::abs(this->max - this->min) / num;
                    paramState = true;
                }
            }
            newfile.close(); //close the file object.
        }
        else {
            cout << "ERROR::FogGrid : file not found " << endl;
        }
    }

    void assignUniform(Shader shader) {
        // shader must have fogBox and required struct
        shader.use();
        shader.setVec3("fogBox.min", this->min);
        shader.setVec3("fogBox.max", this->max);
        shader.setVec3("fogBox.num", this->num);
        shader.setVec3("fogBox.step", this->step);
        for (unsigned int i = 0; i < this->params.size();i++) {
            FogParams tmp = this->params[i];
            shader.setVec3("fogBox.values[" + std::to_string(i) + "].sigma_s", tmp.sigma_s);
            shader.setVec3("fogBox.values[" + std::to_string(i) + "].sigma_a", tmp.sigma_a);
            shader.setFloat("fogBox.values[" + std::to_string(i) + "].density", tmp.density);               
        }
    }

private :
    string delimiter = " ";

};

# endif