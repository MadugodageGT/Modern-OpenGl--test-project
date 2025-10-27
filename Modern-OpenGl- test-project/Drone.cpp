#include "Drone.h"


Drone::Drone(const std::vector<PathPoint>& pathPoints, bool loop)
    : path(pathPoints), currentPathIndex(0), startTime(0.0f),
    isActive(false), loopPath(loop), modelMatrix(glm::mat4(1.0f))
{
    if (!path.empty()) {
        currentPosition = path[0].position;
        currentColor = path[0].color;
    }
}

void Drone::start(float currentTime) {
    startTime = currentTime;
    isActive = true;
    currentPathIndex = 0;
}

void Drone::update(float currentTime) {
    if (!isActive || path.size() < 2) return;

    float elapsedTime = currentTime - startTime;

    // Find which segment we're on
    while (currentPathIndex < path.size() - 1) {
        if (elapsedTime <= path[currentPathIndex + 1].timestamp) {
            break;
        }
        currentPathIndex++;
    }

    // Check if we've reached the end
    if (currentPathIndex >= path.size() - 1) {
        if (loopPath) {
            // Restart from beginning
            startTime = currentTime;
            currentPathIndex = 0;
            elapsedTime = 0.0f;
        }
        else {
            // Stay at final position
            currentPosition = path.back().position;
            currentColor = path.back().color;
            isActive = false;
            modelMatrix = glm::translate(glm::mat4(1.0f), currentPosition);
            return;
        }
    }

    // Interpolate between current and next path point
    const PathPoint& p1 = path[currentPathIndex];
    const PathPoint& p2 = path[currentPathIndex + 1];

    float segmentDuration = p2.timestamp - p1.timestamp;
    float segmentElapsed = elapsedTime - p1.timestamp;
    float t = segmentElapsed / segmentDuration; // 0 to 1
    t = glm::clamp(t, 0.0f, 1.0f);

    // Smooth interpolation (optional - use linear or smoothstep)
    // t = t * t * (3.0f - 2.0f * t); // smoothstep

    currentPosition = interpolatePosition(p1, p2, t);
    currentColor = interpolateColor(p1, p2, t);

    // Update model matrix
    modelMatrix = glm::translate(glm::mat4(1.0f), currentPosition);
}

void Drone::reset() {
    currentPathIndex = 0;
    isActive = false;
    if (!path.empty()) {
        currentPosition = path[0].position;
        currentColor = path[0].color;
    }
}

void Drone::setPath(const std::vector<PathPoint>& newPath) {
    path = newPath;
    reset();
}

glm::vec3 Drone::interpolatePosition(const PathPoint& p1, const PathPoint& p2, float t) {
    return glm::mix(p1.position, p2.position, t);
}

glm::vec3 Drone::interpolateColor(const PathPoint& p1, const PathPoint& p2, float t) {
    return glm::mix(p1.color, p2.color, t);
}