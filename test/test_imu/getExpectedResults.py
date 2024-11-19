import numpy as np
from ahrs.filters.aqua import slerp_I, adaptive_gain
import numpy as np
from ahrs.common.orientation import q_prod, q2R

ALPHA = .9
BETA = .9
E = .9

def Omega(x: np.ndarray) -> np.ndarray:
        # Stolen and slightly modified from ahrs.filters.aqua import AQUA
        """
        Omega operator.

        Given a vector :math:`\\mathbf{x}\\in\\mathbb{R}^3`, return a
        :math:`4\\times 4` matrix of the form:

        .. math::
            \\boldsymbol\\Omega(\\mathbf{x}) =
            \\begin{bmatrix}
            0 & -\\mathbf{x}^T \\\\ \\mathbf{x} & \\lfloor\\mathbf{x}\\rfloor_\\times
            \\end{bmatrix} =
            \\begin{bmatrix}
            0 & x_1 & x_2 & x_3 \\\\
            -x_1 & 0 & x_3 & -x_2 \\\\
            -x_2 & -x_3 & 0 & x_1 \\\\
            -x_3 & x_2 & -x_1 & 0
            \\end{bmatrix}

        This operator is a simplification to create a 4-by-4 matrix used for
        the product between the angular rate and a quaternion, such that:

        .. math::

            ^L_G\\dot{\\mathbf{q}}_{\\omega, t_k} = \\frac{1}{2}\\boldsymbol\\Omega(\\,^L\\mathbf{\\omega}_{q, t_k})\\;^L_G\\mathbf{q}_{t_{k-1}}

        .. note::
            The original definition in the article (eq. 39) has an errata
            missing the multiplication with :math:`\\frac{1}{2}`.

        Parameters
        ----------
        x : numpy.ndarray
            Three-dimensional vector representing the angular rate around the
            three axes of the local frame.

        Returns
        -------
        Omega : numpy.ndarray
            Omega matrix.
        """
        return 0.5 * np.array([
            [0.0,    x[0],  x[1],  x[2]],
            [-x[0],   0.0,  x[2], -x[1]],
            [-x[1], -x[2],   0.0,  x[0]],
            [-x[2],  x[1], -x[0],   0.0]])


def updateMARG(q: np.ndarray, gyr: np.ndarray, acc: np.ndarray, mag: np.ndarray, dt: float = None) -> np.ndarray:
        # Stolen and slightly modified from ahrs.filters.aqua import AQUA
        
        """
        Quaternion Estimation with a MARG architecture.

        The estimation is made in two steps: a *prediction* is done with the
        angular rate (gyroscope) to integrate and estimate the current
        orientation; then a *correction* step uses the measured accelerometer
        and magnetic field to infer the expected geodetic values. Its
        divergence is used to correct the predicted quaternion.

        If the gyroscope data is invalid, it returns the given a-priori
        quaternion. Secondly, if the accelerometer data is invalid the
        predicted quaternion (using gyroscopes) is returned. Finally, if the
        magnetometer measurements are invalid, returns a quaternion corrected
        by the accelerometer only.

        Parameters
        ----------
        q : numpy.ndarray
            A-priori quaternion.
        gyr : numpy.ndarray
            Sample of tri-axial Gyroscope in rad/s.
        acc : numpy.ndarray
            Sample of tri-axial Accelerometer in m/s^2
        mag : numpy.ndarray
            Sample of tri-axial Magnetometer in mT
        dt : float, default: None
            Time step, in seconds, between consecutive Quaternions.

        Returns
        -------
        q : numpy.ndarray
            Estimated quaternion.

        """
        if gyr is None or not np.linalg.norm(gyr)>0:
            return q
        # PREDICTION
        qDot = Omega(gyr) @ q                          # Quaternion derivative (eq. 39)
        qInt = q + qDot*dt                                  # Quaternion integration (eq. 42)
        qInt /= np.linalg.norm(qInt)
        # CORRECTION
        a_norm = np.linalg.norm(acc)
        if not a_norm > 0:
            return qInt
        a = acc/a_norm
        gx, gy, gz = q2R(qInt).T @ a                        # Predicted gravity (eq. 44)
        # Accelerometer-Based Quaternion
        q_acc = np.array([np.sqrt((gz+1.0)/2.0), -gy/np.sqrt(2.0*(gz+1.0)), gx/np.sqrt(2.0*(gz+1.0)), 0.0])     # Delta Quaternion (eq. 47)
        alpha = ALPHA
        alpha = adaptive_gain(alpha, acc)
        q_acc = slerp_I(q_acc, alpha, E)
        q_prime = q_prod(qInt, q_acc)                       # (eq. 53)
        q_prime /= np.linalg.norm(q_prime)
        # Magnetometer-Based Quaternion
        m_norm = np.linalg.norm(mag)
        if not m_norm > 0:
            return q_prime
        lx, ly, _ = q2R(q_prime).T @ (mag/m_norm)           # World frame magnetic vector (eq. 54)
        Gamma = lx**2 + ly**2                               # (eq. 28)
        q_mag = np.array([np.sqrt(Gamma+lx*np.sqrt(Gamma))/np.sqrt(2*Gamma), 0.0, 0.0, ly/np.sqrt(2*(Gamma+lx*np.sqrt(Gamma)))])    # (eq. 58)
        q_mag = slerp_I(q_mag, BETA, E)
        # Generalized Quaternion
        q = q_prod(q_prime, q_mag)                          # (eq. 59)
        return q/np.linalg.norm(q)


######### test_quaternion_based_complimentary_filter #########
# Test case 2: Small angular velocity with no external acceleration, identity quaternion
def test_case_2():
    # IMU measurements (small angular velocity in the x-direction, and acceleration due to gravity)
    q0=np.array([1.0, 0.0, 0.0, 0.0])
    dt = .1
    acc = np.array([0.0, 0.0, 9.81])  # Gravity in z-direction (no external acceleration)
    gyr = np.array([0.1, 0.0, 0.0])   # Small angular velocity in x-direction
    mag = np.zeros(3)                 # No magnetometer data
    
    # Initialize AQUA filter
    q = updateMARG(q0, gyr, acc, mag, dt)
    
    print(f"Test Case 2 Quaternion: {q}")

# Test case 3: Large angular velocity and non-gravitational acceleration
def test_case_3():
    # IMU measurements (large angular velocity and non-gravitational acceleration)
    q0=np.array([1.0, 0.0, 0.0, 0.0])
    dt = .1
    acc = np.array([2.0, 2.0, 5.0])  # Non-gravity forces
    gyr = np.array([1.0, 1.0, 1.0])  # Large angular velocity
    mag = np.array([0.5, 0.5, 0.5])  # Some magnetic field data for variety
    
    # Initialize AQUA filter
    q = updateMARG(q0, gyr, acc, mag, dt)

    print(f"Test Case 3 Quaternion: {q}")

if __name__ == "__main__":
    # Run the test cases
    test_case_2()
    test_case_3()
