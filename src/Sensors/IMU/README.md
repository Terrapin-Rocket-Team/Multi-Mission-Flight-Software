# IMU

## Attitude Kinematics

Attitude kinematics deals with the orientation and rotation of a body in space. This document covers key concepts including reference frames, Direction Cosine Matrices (DCM), and quaternions.

### Reference Frames

Reference frames are essential for describing the position and orientation of objects in space. This section introduces different types of reference frames and their significance in attitude kinematics.

- **Inertial Frame**: A non-accelerating reference frame often used as a global frame of reference.
- **Body Frame**: A frame that moves with the body and is attached to it. It rotates relative to the inertial frame.

### DCM (Direction Cosine Matrix)

The Direction Cosine Matrix (DCM) is a matrix that describes the orientation of one reference frame with respect to another. It is used to convert vectors between different reference frames.

**Definition**: The DCM is a 3x3 orthogonal matrix where each element represents the cosine of the angle between the axes of the reference frames.

#### Example

A DCM matrix $\mathbf{C}$ can be represented as:

![Equation](images/equation.svg)

Where $c_{ij}$ are the direction cosines between the reference frame axes.

### Quaternions

Quaternions are a mathematical representation of rotation that extend complex numbers. They are particularly useful in 3D computations for representing and computing rotations.

- **Definition**: A quaternion $\mathbf{q}$ is defined as:
$\mathbf{q} = q_0 + q_1i + q_2j + q_3k$
Where $q_0$ is the scalar part and $q_1, q_2, q_3$ are the vector parts.

- **Advantages**:
  - **Compact Representation**: Requires four parameters instead of nine (DCM).
  - **Avoids Gimbal Lock**: No singularities in rotation representation.
  - **Efficient Computation**: Quaternion operations are computationally efficient.

#### Conversion

Quaternions can be converted to and from DCMs and Euler angles. This is useful for integrating different attitude representations in applications.

#### Example

Given a quaternion $\mathbf{q}$, the corresponding DCM $\mathbf{C}$ can be calculated as follows:

![Equation](<images/equation (1).svg>)

### Euler Angles



## Relative Attitude Determination

Sources: 
- https://stanford.edu/class/ee267/lectures/lecture10.pdf 
- https://ahrs.readthedocs.io/en/latest/filters/angular.html

Given gyroscope angular velocity **w** and accelerameter accelerations **a** measurements how do I propogate my orientation forward in time?

Since this is a relative orientation, we will start with assumption of a starting orientation of 

![Equation](<images/equation (2).svg>)

![Equation](<images/equation (3).svg>)

![Equation](<images/equation (4).svg>)

![Equation](<images/equation (5).svg>)


TODO check the order of multiplication on the following line 

![Equation](<images/equation (6).svg>)

This quaternion **$q^{t + \Delta t}$** represents the rotation from body to world frame.

### Accelerameter Complementary Filter

The above approch works... but degrades over time (search google for dead recking). One easy way to improve the attitude estimation over time is to include information from the accelerameter. When stationary, we know that the accelerameter will read +9.81 in the inertial z direction. We can use this information to correct for drift while the gryo is stationary.

TODO how to know if the gyro is stationary and stop doing this if it isnt???

Given 

![Equation](<images/equation (7).svg>) 

to turn it into the interial frame acceleration.

![Equation](<images/equation (8).svg>)

![Equation](<images/equation (9).svg>)
TODO change this so that the inverse is switched

![Equation](<images/equation (10).svg>)

Set

![Equation](<images/equation (11).svg>)

Set

![Equation](<images/equation (12).svg>)

Then your new *better* attitude is given by the complimentary filter

![Equation](<images/equation (13).svg>)

with $\alpha = [0, 1]$ TODO determine the best value of alpha for a sensor

Your new attitude now becomes 

![Equation](<images/equation (14).svg>)


## Absolute Attitude Determination

### Quaternion Based Complementary Filter (w/ Adaptive Gain)

Sources: 
- https://ahrs.readthedocs.io/en/latest/filters/aqua.html#quaternion-based-complementary-filter
- https://ahrs.readthedocs.io/en/latest/filters/aqua.html#accelerometer-based-correction
- https://ahrs.readthedocs.io/en/latest/filters/aqua.html#magnetometer-based-correction

Given gyroscope (angular velocity), accelermater (linear acceleration), magnatometer (magnetic field) measurements (and delta t), determine and propogate an absolute orientation quaternion.

Generally this method will work by combining the propogated gyroscope with the propogated accelerameter (roll, pitch) with the propogated magnatometer (yaw). 

Note this derivation is done on the quaternion that maps inertial to body coordinates. This is generally backwards but follows the sources above (they call inertial global and body local). 

#### Determine gyro orientation

1. Determine rate of change of quaternion

${B \atop }\mathbf{\omega}_{q,t_{k}} = \begin{bmatrix}0 \\\omega_x \\\omega_y \\\omega_z\end{bmatrix}$

${B \atop I}q_{w,t_k} = {I \atop B}q_w^{-1}$

The above step is necessary as the quaternion we track in the code is the mapping from the body frame to the interial frame.

${B \atop I}\dot{q}_{w,t_k} = -\frac{1}{2} {B \atop }\mathbf{\omega}_{q,t_{k}} {B \atop I}q_{w,t_k}$

2. Propogate gyro orientation

${B \atop I}q_{w,t_k} = {B \atop I}q_{w,t_{k-1}} + {B \atop I}\dot{q}_{w,t_k} * \Delta t$

#### Determine Accel orientation

3. Find the predicted gravity vector in the inertial frame

$g = {I \atop }g_p = {B \atop I}q_{w,t_k}^{-1} * {B \atop }a * {B \atop I}q_{w,t_k}$

4. Determine delta acc orientation

If g_z == -1, then TODO (shouldn't happen though).

$\Delta q_{acc} = \begin{bmatrix}\sqrt{\frac{g_z+1}{2}} \\-\frac{g_y}{\sqrt{(2*(g_z+1))}}  \\ \frac{g_x}{\sqrt{2(g_z+1)}} \\0 \end{bmatrix}$

5. To reduce effect of high frequency noise preform interpolation between delta q_acc and identity quaternion

$\Delta q_{acc} = \Delta q_{acc}.interpolation([1, 0, 0, 0], \alpha=.5, \epsilon=.9)$

6. Combine with gryo orientation to correct roll and pitch

${B \atop I}q_{wa} = {B \atop I}q_{w} \Delta q_{acc}$

#### Determine Mag orientation

7. Get magnetic field vector in world frame

${I \atop }m = {B \atop I}q_{wa}^{-1} * {B \atop }m * {B \atop I}q_{wa}$

8. Determine delta mag orientation

$\Gamma = m_x^2 + m_y^2$

$\Delta q_{mag} = \begin{bmatrix}\frac{\sqrt{\Gamma + m_x\sqrt{\Gamma}}}{\sqrt{2\Gamma}} \\0  \\0  \\ \frac{m_y}{\sqrt{2(\Gamma + m_x\sqrt{\Gamma})}} \end{bmatrix}$

9. To reduce effect of high frequency noise preform interpolation between delta q_mag and identity quaternion (alpha can be different from accelerameter)

$\Delta q_{mag} = \Delta q_{mag}.interpolation([1, 0, 0, 0], \alpha=.6, \epsilon=.9)$

10. Combine with gryo/acc orientation to correct yaw

${B \atop I}q = {B \atop I}q_{wa} \Delta q_{mag}$

${I \atop B}q = {B \atop I}q^{-1}$

This last q with the I over the B is what we want. This is the mapping from the body to interial frame as a quaternion and can be used as such:

${I \atop }r = {I \atop B}q * {B \atop }r * {I \atop B}q^{-1}$



