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
$\mathbf{C} = \begin{bmatrix}
1 - 2(q_2^2 + q_3^2) & 2(q_1q_2 - q_0q_3) & 2(q_1q_3 + q_0q_2) \\
2(q_1q_2 + q_0q_3) & 1 - 2(q_1^2 + q_3^2) & 2(q_2q_3 - q_0q_1) \\
2(q_1q_3 - q_0q_2) & 2(q_2q_3 + q_0q_1) & 1 - 2(q_1^2 + q_2^2)
\end{bmatrix}$

### Euler Angles



## Relative Attitude Determination

Sources: 
- https://stanford.edu/class/ee267/lectures/lecture10.pdf 
- https://ahrs.readthedocs.io/en/latest/filters/angular.html

Given gyroscope angular velocity **w** and accelerameter accelerations **a** measurements how do I propogate my orientation forward in time?

Since this is a relative orientation, we will start with assumption of a starting orientation of 

Set **$q^{t=0}$** = $\begin{bmatrix} 1 \\ 0 \\ 0 \\0 \end{bmatrix}$

Set $\theta = |$**w**|$\Delta t$ 

Set **v** = $\frac{\bold{w}}{|\bold{w}|}$

**$q_\Delta$** = $\begin{bmatrix} cos(\frac{\theta}{2}) \\ v_1*sin(\frac{\theta}{2}) \\ v_2*sin(\frac{\theta}{2}) \\v_3*sin(\frac{\theta}{2}) \end{bmatrix}$


TODO check the order of multiplication on the following line 

**$q^{t + \Delta t}$** = **$q_\Delta$** * **$q^{t}$**

This quaternion **$q^{t + \Delta t}$** represents the rotation from body to world frame.

### Accelerameter Complementary Filter

The above approch works... but degrades over time (search google for dead recking). One easy way to improve the attitude estimation over time is to include information from the accelerameter. When stationary, we know that the accelerameter will read +9.81 in the inertial z direction. We can use this information to correct for drift while the gryo is stationary.

TODO how to know if the gyro is stationary and stop doing this if it isnt???

Given **$a^\Beta$** = $\begin{bmatrix} a_x \\ a_y \\a_z \end{bmatrix}$ turn it into the interial frame acceleration.

**$q_a^\Beta$** = $\begin{bmatrix} 0 \\ a_x \\ a_y \\a_z \end{bmatrix}$

**$q_a^I$** = **$q^{t + \Delta t}$** * **$q_a^\Beta$** * **$q^{(t + \Delta t)^{-1}}$**

Set **v** = $\begin{bmatrix} \frac{q_{a_x}^I}{|q_{a}^I|} \\ \frac{q_{a_y}^I}{|q_{a}^I|} \\\frac{q_{a_z}^I}{|q_{a}^I|} \end{bmatrix}$

Set $\phi = cos^{-1}(\bold{v_x})$

Set **n** = $\begin{bmatrix} v_y \\ -v_x \\ 0\end{bmatrix}$

Then your new *better* attitude is given by the complimentary filter

**$q^{t + \Delta t}_c$** = $\begin{bmatrix} (1-\alpha)*\phi \\ \frac{n_x}{|\bold{n}|} \\ \frac{n_y}{|\bold{n}|} \\ \frac{n_z}{|\bold{n}|} \end{bmatrix}$ * **$q^{t + \Delta t}$**

with $\alpha = [0, 1]$ TODO determine the best value of alpha for a sensor

Your new attitude now becomes 

**$q^{t}$** = **$q^{t + \Delta t}_c$**


## Absolute Attitude Determination

