from dataclasses import dataclass
import pybullet as p
import time
import numpy as np
import utils
import argparse
from scipy import signal



@dataclass
class Point:
    x: float
    y: float

@dataclass
class World:
    """A class to hold the world objects"""
    plate: int
    sphere: int

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--setpoint", type=float, nargs=2, default=(0.0,0.0))
    parser.add_argument("--kp", type=float, default=8) # set these values to tune the PD controller
    parser.add_argument("--kd", type=float, default=2) # set these values to tune the PD controller
    parser.add_argument("--noise", action="store_true", help="Add noise to the measurements")
    parser.add_argument("--filtered", action="store_true", help="filter the measurements")
    cmd_args = parser.parse_args()
    print(cmd_args)
    cmd_args.setpoint = Point(*cmd_args.setpoint)
    return cmd_args


def run_controller(kp, kd, setpoint, noise, filtered, world: World):
    prev_error_x = 0.0
    prev_error_y = 0.0
    
    N_ord = 2
    f_cutoff = 10 
    wn = f_cutoff * 2.0/100 # Nyquist
    
    x_in = [0.0]*(N_ord+1)
    x_out = [0.0]*(N_ord+1)
    y_in = [0.0]*(N_ord+1)
    y_out = [0.0]*(N_ord+1)
    
    b, a = signal.butter(N_ord, wn, 'low', analog=False)
    
    
    def set_plate_angles(theta_x, theta_y):
        p.setJointMotorControl2(world.plate, 1, p.POSITION_CONTROL, targetPosition=np.clip(theta_x, -0.1, 0.1), force=5, maxVelocity=2)
        p.setJointMotorControl2(world.plate, 0, p.POSITION_CONTROL, targetPosition=np.clip(-theta_y, -0.1, 0.1), force=5, maxVelocity=2)

    # you can set the variables that should stay accross control loop here

    def pd_controller(x, y, kp, kd, setpoint):
        """Implement a PD controller, you can access the setpoint via setpoint.x and setpoint.y
        the plate is small around 0.1 to 0.2 meters. You will have to calculate the error and change in error and 
        use those to calculate the angle to apply to the plate. It must then oscillate around the setpoint. Calculate the error between the hx, yi position of the ball and the setpoint given as an argument.  Perform proportional control using the errors as inputs to produce the angles of the plate. Getting the ball to oscillate around the setpoint."""
        
        # Refer to the nonlocal prevs 
        nonlocal prev_error_x
        nonlocal prev_error_y 

        error_x = setpoint.x - x
        error_y = setpoint.y - y

        delta_error_x = (error_x - prev_error_x) / 0.01
        delta_error_y = (error_y - prev_error_y) / 0.01

        angle_x = (kp) * error_x + (kd) * delta_error_x 
        angle_y = (kp) * error_y + (kd) * delta_error_y

        prev_error_x = error_x
        prev_error_y = error_y

        return angle_x, angle_y


    def filter_val(val, in1, out1):
        """Implement a filter here, you can use scipy.signal.butter to compute the filter coefficients and then scipy.signal.lfilter to apply the filter.but we recommend you implement it yourself instead of using lfilter because you'll have to do that on the real system later.
        Take a look at the butterworth example written by Renato for inspiration."""
        
        # Perform a sample shift from the reverse
        for i in range(N_ord, 0, -1):
            in1[i] = in1[i-1]
            out1[i] = out1[i-1]
    
        in1[0] = val
	
        val_filtered = 0.0
	
        for i in range(0,N_ord+1): 
            val_filtered += b[i]*in1[i] 
            if i > 0:
            	val_filtered -= a[i]*out1[i]
        
        out1[0] = val_filtered
        
        return val_filtered
        


    def every_10ms(i: int, t: float):
        '''This function is called every ms and performs the following:
        1. Get the measurement of the position of the ball
        2. Calculate the forces to be applied to the plate
        3. Apply the forces to the plate
        '''
        (x,y,z), orientation = p.getBasePositionAndOrientation(world.sphere)
        if noise:
            x += utils.noise(t) # the noise added has a frequency between 30 and 50 Hz
            y += utils.noise(t, seed = 43) # so that the noise on y is different than the one on x
            	
            
        
        if filtered:
            x = filter_val(x,x_in,x_out)
            y = filter_val(y,y_in,y_out)

        (angle_x, angle_y) = pd_controller(x, y, kp, kd, setpoint)
        set_plate_angles(angle_x, angle_y)

        if i%10 == 0: # print every 100 ms
            print(f"t: {t:.2f}, x: {x:.3f},\ty: {y:.3f},\tax: {angle_x:.3f},\tay: {angle_y:.3f}")

    utils.loop_every(0.01, every_10ms) # we run our controller at 100 Hz using a linux alarm signal

def run_simulation( initial_ball_position = Point(np.random.uniform(-0.2, 0.2),
                                                  np.random.uniform(-0.2, 0.2))):
    p.connect(p.GUI)
    p.setAdditionalSearchPath("assets")
    plate = p.loadURDF("plate.urdf")
    sphere = p.createMultiBody(0.2
        , p.createCollisionShape(p.GEOM_SPHERE, radius=0.04)
        , basePosition = [initial_ball_position.x,initial_ball_position.y,0.5]
    )

    #zoom to the plate
    p.resetDebugVisualizerCamera(cameraDistance=1.0, cameraYaw=0, cameraPitch=-45, cameraTargetPosition=[0,0,0])

    p.setJointMotorControl2(plate, 0, p.POSITION_CONTROL, targetPosition=0, force=5, maxVelocity=2)
    p.setJointMotorControl2(plate, 1, p.POSITION_CONTROL, targetPosition=0, force=5, maxVelocity=2)

    p.setGravity(0, 0, -9.8)

    #update the simulation at 100 Hz
    p.setTimeStep(0.01)
    p.setRealTimeSimulation(1)
    return World(plate=plate, sphere=sphere)


if __name__ == "__main__":
    cmd_args = parse_args()
    world = run_simulation()
    run_controller(**vars(cmd_args), world=world)
    time.sleep(10000)
