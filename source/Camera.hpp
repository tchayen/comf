#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "glm-0.9.7.5\glm\glm.hpp"
#include "glm-0.9.7.5\glm\gtc\type_ptr.hpp"
#include "glm-0.9.7.5\glm\gtx\transform.hpp"
#include "glm-0.9.7.5\glm\gtx\rotate_vector.hpp"
#include "glm-0.9.7.5\glm\gtc\matrix_transform.hpp"

#define MAX_VERTICAL_ANGLE 80.0f
#define PI 3.14159

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 target;

    float moveSpeed;
    float rotationSpeed;

    float FOV;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    float distance;
    float distanceSpeed;
    float minDistance;
    float maxDistance;

    float verticalAngle;
    float horizontalAngle;

    float maxVerticalAngle;
    float minVerticalAngle;

    float mouseSensitivity;

    Camera()
    {
        position = glm::vec3(1.0f, 1.0f, 1.0f);
        normalizeAngles();

        FOV = 45.0f;
        nearPlane = 0.1f;
        farPlane = 200.0f;
        moveSpeed = 0.1f;
        aspectRatio = 16.0f / 9.0f;
        distance = 15.0f;
        minDistance = 1.0f;
        maxDistance = 100.0f;
        distanceSpeed = 5.0f;
        verticalAngle = -60.0f;
        horizontalAngle = 0.0f;
        maxVerticalAngle = 80.0f;
        minVerticalAngle = -5.0f;
        mouseSensitivity = 0.1f;
    }

    void offsetOrientation(float upAngle, float rightAngle)
    {
        horizontalAngle += rightAngle;
        verticalAngle -= upAngle;
        normalizeAngles();
    }
    
    glm::vec3 getForwardVector()
    {
        return glm::normalize(glm::vec3(-target.x, 0.0f, -target.z));
    }

    glm::vec3 getRightVector()
    {
        glm::vec3 right = glm::normalize(glm::rotateY(getForwardVector(), -90.0f));
        return glm::vec3(right);
    }

    glm::vec3 getUpVector()
    {
        return glm::vec3(1.0f, 1.0f, 1.0f);
    }
    
    // perspective matrix (FOV, aspect ratio, near plane, far plane)
    glm::mat4 getProjection()
    {
        return glm::perspective(FOV, aspectRatio, nearPlane, farPlane);
    }

    // view matrix (arcball camera via glm::lookAt)
    glm::mat4 getView()
    {
        target = glm::vec3(
            distance * -sinf(horizontalAngle * (PI / 180.0f)) * cosf((verticalAngle)* (PI / 180.0f)),
            distance * -sinf(verticalAngle * (PI / 180.0f)),
            -distance * cosf(horizontalAngle * (PI / 180.0f)) * cosf((verticalAngle)* (PI / 180.0f))
            );

        return glm::lookAt(target + position,
            position,
            glm::vec3(0.0f, 1.0f, 0.0f)
            );
    }

    glm::mat4 getOrientation()
    {
        glm::mat4 orientation;
        orientation = glm::rotate(orientation, verticalAngle, glm::vec3(1.0f, 0.0f, 0.0f));
        orientation = glm::rotate(orientation, horizontalAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        return orientation;
    }

    // change distance keeping in mind max and min value
    void changeDistance(float delta)
    {
        if (distance - delta > minDistance && distance - delta < maxDistance)
            distance -= delta;
    }

private:

    // keep angles in given range
    void normalizeAngles()
    {
        horizontalAngle = fmodf(horizontalAngle, 360.0f);
        //fmodf can return negative values, but this will make them all positive
        // dafuq is fmodf
        if (horizontalAngle < 0.0f)
            horizontalAngle += 360.0f;

        if (verticalAngle > -minVerticalAngle)
            verticalAngle = -minVerticalAngle;
        else if (verticalAngle < -maxVerticalAngle)
            verticalAngle = -maxVerticalAngle;
    }
};

#endif