import numpy as np

MAX_OBSTACLE = 100
obstacles = np.zeros((MAX_OBSTACLE, 3)) #[x, y, m]

def calField(obs, x, y, m, p=2):
    mask = obs[:, 2] != 0
    # Get real obstacles (Those mass != 0)
    real_obs = obs[mask, :]
    # Cal distance
    dis = real_obs[:, [0, 1]] - [x, y]
    # Cal norm
    dis_norm = np.linalg.norm(dis, axis=1)
    # Cal field magnitude
    field = real_obs[:, 2] / dis_norm**p
    # Cal unit vector
    unit_vec = real_obs[:, [0, 1]] / dis_norm
    # Cal field vector
    field = field * unit_vec
    
    return field

def main():
    obstacles[0] = [1, 1, 1]
    obstacles[2] = [2, 2, 2]
    field = calField(obstacles, 5, 5, 10)
    print(field)

if __name__ == '__main__':
    main()
