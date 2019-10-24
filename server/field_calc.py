import numpy as np
import matplotlib.pyplot as plt
import random

robot_m = 1
time_slice = 0.5
k = 5
obs_decay = 2

MAX_OBSTACLE = 100
MAX_TIME = 500

obstacles = np.zeros((MAX_OBSTACLE, 3)) #[x, y, m]
position = np.zeros([int(MAX_TIME/time_slice), 2])

def calField(obs, pos, m, p=2, mode=0):
    if mode == 0:
        mask = obs[:, 2] > 0
    else:
        mask = obs[:, 2] < 0
    # Get real obstacles (Those mass != 0)
    real_obs = obs[mask, :]
    # Cal distance
    vec_x = pos - real_obs[:, [0, 1]]
    # Cal norm
    dis_norm = np.linalg.norm(vec_x, axis=1)
    # Cal field magnitude
    accer_magn = k *real_obs[:, 2] / ( dis_norm**(p) * m )
    # Flip array
    dis_norm = dis_norm.reshape((dis_norm.shape[0], 1))
    accer_magn = accer_magn.reshape((accer_magn.shape[0], 1))
    # Cal unit vector
    vec_unit = vec_x / dis_norm
    # Cal field vector
    vec_accer = vec_unit * accer_magn
    vec_accer = np.mean(vec_accer, axis=0)
    #accer = accer / np.linalg.norm(accer, axis=0)
    return vec_accer

def main():
    num_obs = 18
    map_range = 20
    for i in range(0, num_obs):
        obstacles[i] = [random.random()*map_range, random.random()*map_range, (random.random()*8+5)]
    obstacles[num_obs] = [map_range*0.9, map_range*0.9, -40]

    position[0] = [2, 2]
    pre_speed = [0, 0]
    last =  position.shape[0]
    pre_norm_accer = 0
    for i in range(1, position.shape[0]):
        accer = calField(obstacles, position[i-1], m=robot_m, p=obs_decay)
        target_accer = calField(obstacles, position[i-1], m=robot_m, p=2, mode=1)
        accer += target_accer
        
        norm_accer = np.linalg.norm(accer)
        change_rate = norm_accer - pre_norm_accer
        pre_norm_accer = norm_accer
        if( change_rate < -0.1):
            print(change_rate)

        accer = accer / norm_accer

        speed =  pre_speed + accer * time_slice
        position[i] = position[i-1] + speed * time_slice
        pre_speed = 0.8*speed
        #print('target_accer= ', target_accer, 'acc=', accer, '  speed= ', speed)
        #print(position[i-1])
        if(np.linalg.norm(position[i]-obstacles[num_obs,[0,1]])<=0.5):
            last = i
            break
    circles = [plt.Circle((c[0], c[1]), c[2]/8, color='r', alpha=0.5) for c in obstacles[0:num_obs]]
    c = obstacles[num_obs]
    circles.append(plt.Circle((c[0], c[1]), c[2]/-10, color='g', alpha=0.5))

    #cc = plt.Circle(obstacles[0, :], obstacles[0, 2], color='r')
    ax = plt.gca()
    ax.cla()
    ax.set_xlim(0, map_range)
    ax.set_ylim(0, map_range)
    
    p = position[0:last]
    ax.plot(p[:,0], p[:,1])
    for c in circles:
        ax.add_artist(c)
    #ax.add_artist(cc)
    plt.show()

if __name__ == '__main__':
    main()
