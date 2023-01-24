#ifndef LIGHT_H
#define LIGHT_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include<string>


class Light {
    
public:
    enum LightType
    {
        Direction = 0,
        Point = 1,
        Spot = 2
    };
    glm::vec3 ambient, diffuse, specular;
    glm::vec3 model, pos;
    LightType type;
    Light(LightType type, glm::vec3 model, glm::vec3 pos);
        /*
            if  Direction : pos = direction,
                Point : pos = position
            ** Ignore spotlight for now **
        */
    
    void setProp(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

};

#endif