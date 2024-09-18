# IMU

## Attitude Kinematics

### Reference Frames

### DCM

### Quaternions

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

