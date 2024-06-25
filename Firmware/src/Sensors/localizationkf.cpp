#include "localizationkf.h"

#include <Eigen/Core>
#include <Eigen/Eigen>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

#include <math.h>

LocalizationKF::LocalizationKF() : X(Eigen::Vector<float, 9>::Zero()),
                                   P(Eigen::Matrix<float, 9, 9>::Identity()){};

void LocalizationKF::reset()
{
    // this current intializaiton method means the filter will take time to converge to the solution
    X = Eigen::Vector<float, 9>::Zero();
    P = Eigen::Matrix<float, 9, 9>::Identity();
};

void LocalizationKF::predict(const float &dt)
{
    const float dt2 = dt * dt; // temporary for readability
    const float dt3 = dt2 * dt;

    const Eigen::Matrix<float, 9, 9> A{
        {1, dt, dt2 / 2, 0, 0, 0, 0, 0, 0},
        {0, 1, dt, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, dt, dt2 / 2, 0, 0, 0},
        {0, 0, 0, 0, 1, dt, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, dt, dt2 / 2},
        {0, 0, 0, 0, 0, 0, 0, 1, dt},
        {0, 0, 0, 0, 0, 0, 0, 0, 1}};

    // dt^4 terms are considered negligible so are zero
    const Eigen::Matrix<float, 9, 9> Q{
        {0, dt3 / 2, dt2 / 2, 0, 0, 0, 0, 0, 0},
        {dt3 / 2, dt2, dt, 0, 0, 0, 0, 0, 0},
        {dt2 / 2, dt, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, dt3 / 2, dt2 / 2, 0, 0, 0},
        {0, 0, 0, dt3 / 2, dt2, dt, 0, 0, 0},
        {0, 0, 0, dt2 / 2, dt, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, dt3 / 2, dt2 / 2},
        {0, 0, 0, 0, 0, 0, dt3 / 2, dt2, dt},
        {0, 0, 0, 0, 0, 0, dt2 / 2, dt, 1}};

    // update State
    X = (A * X);
    // update Covariance estimate
    P = A * P * A.transpose() + (Q * processVariance);
}



void LocalizationKF::baroUpdate(const float &altitude)
{
    kalmanUpdate<1>(Eigen::Matrix<float, 1, 1>{altitude}, Eigen::Matrix<float, 1, 1>{baroVariance}, H_BARO);
}

void LocalizationKF::accelUpdate(const Eigen::Ref<const Eigen::Vector3f>& accel)
{
    kalmanUpdate<3>(accel, R_ACCEL.toDenseMatrix(), H_ACCEL);
}

void LocalizationKF::HaccelUpdate(const Eigen::Ref<const Eigen::Vector3f>& accel)
{
    kalmanUpdate<3>(accel, R_HACCEL.toDenseMatrix(), H_ACCEL);
}

