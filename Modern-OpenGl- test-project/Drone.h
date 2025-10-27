#ifndef DRONE_CLASS_H
#define DRONE_CLASS_H

#include<string>

#include"VAO.h"
#include"EBO.h"
#include"Camera.h"

class Drone {
private:
    glm::vec3 currentPosition;
    glm::vec3 currentColor;
    glm::mat4 modelMatrix;

    struct PathPoint {
        
        float timestamp; // Time in seconds when drone should reach this point
        glm::vec3 position;
        glm::vec3 color;
                         
    };

    std::vector<PathPoint> path;
    size_t currentPathIndex;

    float startTime;
    bool isActive;
    bool loopPath;

    // Interpolation helpers
    glm::vec3 interpolatePosition(const PathPoint& p1, const PathPoint& p2, float t);
    glm::vec3 interpolateColor(const PathPoint& p1, const PathPoint& p2, float t);

public:
    Drone(const std::vector<PathPoint>& pathPoints, bool loop = false);

    void start(float currentTime);
    void update(float currentTime);
    void reset();

    glm::mat4 getModelMatrix() const { return modelMatrix; }
    glm::vec3 getColor() const { return currentColor; }
    glm::vec3 getPosition() const { return currentPosition; }
    bool isActiveState() const { return isActive; }

    void setPath(const std::vector<PathPoint>& newPath);
};


#endif