/*
 * This library expands the OpenGL glm mathematical library with a number of 
 * utility functions. 
 * There are also operators to write vectors and matrices to an ostream.
 */

#ifndef __RASTERMAN_GLM_UTILS_H__
#define __RASTERMAN_GLM_UTILS_H__

/**
 * \file glmutils.h
 */

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <string>
#include <cctype>
#include <iomanip>

// Use gtx_transform
#define GLM_ENABLE_EXPERIMENTAL
// Use radians by default
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>

namespace glm {
    /**
     * Creates a 4x4 matrix which performs a translation
     * \param dx - the x-component of the translation vector
     * \param dy - the y-component of the translation vector
     * \param dz - the z-component of the translation vector
     * \return a 4x4 matrix performing a translation
     */
    glm::mat4x4 translate(float dx, float dy, float dz);

    /**
     * Creates a 4x4 matrix which performs a scaling
     * \param s_x - scale factor in the x-direction
     * \param s_y - scale factor in the y-dirextion
     * \param s_z - scale factor in the z-dirextion
     * \return a 4x4 matrix performing a scaling
     */
    glm::mat4x4 scale(float s_x, float s_y, float s_z);

    /**
     * Create a 4x4 matrix which performs a rotation around the x-axis
     * \param angle - the rotation angle
     * \return a 4x4 matrix performing a translation
     */
    glm::mat4x4 rotateX(float angle);

    /**
     * Create a 4x4 matrix which performs a rotation around the y-axis
     * \param angle - the rotation angle
     * \return a 4x4 matrix performing a translation
     */
    glm::mat4x4 rotateY(float angle);

    /**
     * Create a 4x4 matrix which performs a rotation around the z-axis
     * \param angle - the rotation angle
     * \return a 4x4 matrix performing a translation
     */
    glm::mat4x4 rotateZ(float angle);

    /**
     * Creates a 4x4 matrix which performs an XY-shear
     * \param sh_x - the shear factor in the x-direction
     * \param sh_y - the shear factor in the y-direction
     * \return a 4x4 matrix performing an XY shear
     */
    glm::mat4x4 shearXY(float sh_x, float sh_y);

    /**
     * Creates a 4x4 matrix which performs an XY-shear
     * \param shearfactors - a vector containing the two shear factors
     * \return a 4x4 matrix performing an XY shear
     */
    glm::mat4x4 shearXY(glm::vec2 const& shearfactors);

    /**
     * Creates a 4x4 matrix which performs an XZ-shear
     * \param sh_x - the shear factor in the x-direction
     * \param sh_z - the shear factor in the z-direction
     * \return a 4x4 matrix performing an XZ shear
     */
    glm::mat4x4 shearXZ(float sh_x, float sh_z);

    /**
     * Creates a 4x4 matrix which performs an XZ-shear
     * \param shearfactors - a vector containing the two shear factors
     * \return a 4x4 matrix performing an XZ shear
     */
    glm::mat4x4 shearXZ(glm::vec2 const& shearfactors);

    /**
     * Creates a 4x4 matrix which performs an YZ-shear
     * \param sh_y - the shear factor in the y-direction
     * \param sh_z - the shear factor in the z-direction
     * \return a 4x4 matrix performing an YZ shear
     */
    glm::mat4x4 shearYZ(float sh_y, float sh_z);

    /**
     * Creates a 4x4 matrix which performs an YZ-shear
     * \param shearfactors - a vector containing the two shear factors
     * \return a 4x4 matrix performing an YZ shear
     */
    glm::mat4x4 shearYZ(glm::vec2 const& shearfactors);

    /**
     * writes a glm::vec2 to a std::ostream
     * \param s - a std::ostream where the output should be written.
     * \param v - a glm::vec2 which should be written.
     * \return the parameter s.
     */
    std::ostream& operator<<(std::ostream& s, glm::vec2 const& v);
    
    /**
     * writes a glm::vec3 to a std::ostream
     * \param s - a std::ostream where the output should be written.
     * \param v - a glm::vec3 which should be written.
     * \return the parameter s.
     */
    std::ostream& operator<<(std::ostream& s, glm::vec3 const& v);

    /**
     * writes a glm::vec4 to a std::ostream
     * \param s - a std::ostream& where the output should be written.
     * \param v - a glm::vec4& which should be written.
     * \return the parameter s.
     */
    std::ostream& operator<<(std::ostream& s, glm::vec4 const& v);

    /**
     * writes a glm::mat2x2 to a std::ostream
     * \param s - a std::ostream where the output should be written.
     * \param m - a glm::mat2x2 which should be written.
     * \return the parameter s.
     */
    std::ostream& operator<<(std::ostream& s, glm::mat2x2 const& m);

    /**
     * writes a glm::mat3x3 to a std::ostream
     * \param s - a std::ostream where the output should be written.
     * \param m - a glm::mat3x3 which should be written.
     * \return the parameter s.
     */
    std::ostream& operator<<(std::ostream& s, glm::mat3x3 const& m);

    /**
     * writes a glm::mat4x4 to a std::ostream
     * \param s - a std::ostream where the output should be written.
     * \param m - a glm::mat4x4 which should be written.
     * \return the parameter s.
     */
    std::ostream& operator<<(std::ostream& s, glm::mat4x4 const& m);
}

#endif
