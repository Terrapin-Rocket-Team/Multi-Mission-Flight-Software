import numpy as np
from ahrs.filters.aqua import slerp_I

######### test_interpolation_lerp and test_interpolation_slerp #########
# Define the quaternions as numpy arrays
p_lerp = np.array([0.95, 0.31224989992, 0.0, 0.0])  # current quaternion (identity)
p_slerp = np.array([0.5, 0.5, 0.5, 0.5])  # input quaternion

# Interpolation factor and epsilon threshold
alpha = 0.5
epsilon = 0.9

result_lerp = slerp_I(p_lerp, alpha, epsilon)
result_slerp = slerp_I(p_slerp, alpha, epsilon)

# Print the resulting quaternion
print("Resulting LERP Quaternion:", result_lerp)
print("Resulting SLERP Quaternion:", result_slerp)



