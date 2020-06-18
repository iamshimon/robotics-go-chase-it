#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;
// to check if the robot is moving or not
bool is_ball_in_range = false;  
bool is_moving = false;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x=lin_x;
    srv.request.angular_z=ang_z;

    //Calling service to move the robot
    if(!client.call(srv))
    {
        ROS_ERROR("Failed to call service command_robot");
    }


}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    
    int white_pixel = 255, i=0, j=0, idx=0, row =0, loc=0;
  
   

   // ROS_INFO("Height: %1.2f, width : %1.2f , Step : %1.2f ",(float)img.height,(float)img.width, (float)img.step);


    // TODO: Loop through each pixel in the image and check if there's a bright white one
    for(i=0; i < img.height ; i++)
        {
            row = i * img.step;
            for(j=0; j< img.step ; j=j+3)
                {  
                    idx= row + j;
                    //ROS_INFO("VALUE OF idx : %1.2f",(float)idx);

                    if(img.data[idx] == white_pixel && img.data[idx+1]== white_pixel && img.data[idx+2] == white_pixel )
                        {   
                        
                            is_ball_in_range = true;
                            loc = idx %  img.step;
                             ROS_INFO(" BALL IS IN RANGE !!");
                            break;
                        
                        }
                    else{
                        is_ball_in_range = false;
                    }
                }if(is_ball_in_range) break;
             
                

        }     

         if(is_ball_in_range)
                {
                is_moving = true;
                  //ROS_INFO("VALUE OF loc : %1.2f",(float)loc);

                    if(loc < img.step/3)
                        {
                            
                            drive_robot(0.35,0.65);  //drive to left
                            
                        }
                    else if(loc > 2*(img.step/3))
                        {   
                            drive_robot(0.35,-0.65); //drive to right
                            
                        }
                    else{
                            drive_robot(1.0,0.0); //drive forward
                           
                        }

                 }
        else{
                if(is_moving)
                    {
                        drive_robot(0.0,0.0);
                        is_moving = false;
                    }
            }


    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
    ROS_INFO_STREAM("Service Client READY");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);
    ROS_INFO_STREAM("Started subscribing to topic /camera/rgb/image/raw");

    // Handle ROS communication events
    ros::spin();

    return 0;
}