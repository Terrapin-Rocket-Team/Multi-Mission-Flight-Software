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

