import numpy as np

# Arrival rate (lambda) for the exponential distribution
arrival_rate = 0.075

# Initialize random number generator
rng = np.random.default_rng()

# Current time
t = 0

# Generate and store arrival times for 1000 tasks
arrival_times = []
for i in range(1000):
    # Generate random value from exponential distribution
    r = rng.exponential(1 / arrival_rate)

    # Calculate the arrival time of the next task
    next_arrival_time = np.ceil(t + r)

    # Update current time
    t = next_arrival_time

    # Store arrival time
    arrival_times.append(next_arrival_time)

    print(i+1000,",3,",next_arrival_time)