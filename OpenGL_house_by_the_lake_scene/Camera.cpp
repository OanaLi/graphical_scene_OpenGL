#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));

        //TODO - Update the rest of camera parameters
    }

    // nou
    void Camera::setCameraPosition(glm::vec3 cameraPosition) {
        this->cameraPosition = cameraPosition;
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
        this->cameraUpDirection = normalize(cross(cameraRightDirection, cameraFrontDirection));
    }

    // nou
    void Camera::setCameraTarget(glm::vec3 cameraTarget) {
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
        this->cameraUpDirection = normalize(cross(cameraRightDirection, cameraFrontDirection));
    }

    //nou
    glm::vec3 Camera::getCameraPosition() {
        return this->cameraPosition;
    }

    //nou
    glm::vec3 Camera::getCameraTarget() {
        return this->cameraTarget;
    }


    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) 
        {
        case MOVE_LEFT:
                cameraPosition -= speed * cameraRightDirection;
                cameraTarget -= speed * cameraRightDirection;
            break;

        case MOVE_RIGHT:
                cameraPosition += speed * cameraRightDirection;
                cameraTarget += speed * cameraRightDirection;
            break;

        case MOVE_FORWARD:
                cameraPosition += speed * cameraFrontDirection;
                cameraTarget += speed * cameraFrontDirection;
            break;

        case MOVE_BACKWARD:
                cameraPosition -= speed * cameraFrontDirection;
                cameraTarget -= speed * cameraFrontDirection;
            break;
        }
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis

    void Camera::rotate(float pitch, float yaw) {

        //yaw: camera sta pe loc, la fel si up fiindca se roteste fata de y
        glm::mat4 rotationMatrixYaw = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)); 
        cameraFrontDirection = glm::vec3(rotationMatrixYaw * glm::vec4(cameraFrontDirection, 1.0f));  
        cameraFrontDirection = normalize(cameraFrontDirection); 
        cameraRightDirection = normalize(cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));



        //pitch: se modifica vectorul up si front, dreapta nu se modifica!!
        glm::mat4 rotationMatrixPitch = glm::rotate(rotationMatrixYaw, glm::radians(pitch), cameraRightDirection);
        glm::vec3 newFrontDirection = glm::vec3(rotationMatrixPitch * glm::vec4(cameraFrontDirection, 1.0f));
        this->cameraFrontDirection = normalize(newFrontDirection); 
        
        
        this->cameraUpDirection = normalize(cross(cameraRightDirection, cameraFrontDirection));
        this->cameraTarget = cameraPosition + cameraFrontDirection; 
    }

     
}