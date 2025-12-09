Mechantronics-Project

The main goal of this project was to create a car that uses three ultrasonic sensors, positioned at the front, −45°, and +45°, to stop when an object is in its way and perform a pass on either side or stand still if all sensors are blocked. Within the code, you will notice a state system and a sub-state system. The state system is responsible for the following states: idle, cruise, stop_decide, and pass_exec. These states are determined by raw sensor data that has been converted into distances. For example, if the front threshold distance is not met and neither of the side sensors detects anything within the specified range, the car will enter the cruise state. However, when an object is detected by one or more sensors, the code will switch to its sub-state where it performs a pass execution based on which sensors are blocked. This sub-state is very important because it allows the car to perform a full pass without the code overriding once the front or side sensors become clear. To do this, four DC motors were used as well as an L293D to control them. The motors were spliced so that the motor controller could control both sides, forward and backward. An Arduino Uno was also used to collect data from the ultrasonic sensors, control output and input flow, and handle state-machine interactions. A couple of limitations of this project were the single L293D motor driver, which led to some voltage drops under load as well as some jitter during straight bursts; no real-time odometry because the robot used timers instead of actual feedback; and no encoder feedback, which hindered pivot accuracy as well as its ability to move in a perfectly straight line during the cruise state.

<img width="566" height="111" alt="Screenshot 2025-12-08 at 9 33 45 PM" src="https://github.com/user-attachments/assets/52e98084-d78a-42f6-8f3d-8be2ce54912c" />

Chassis link: https://www.amazon.com/gp/product/B07DNXBFQN/ref=ox_sc_rp_title_rp_4?smid=&pf_rd_p=a4970ca5-d151-4edb-a206-3bc56f60ce74&pf_rd_r=8B8TTVWSQQA6YW4D4YST&pd_rd_wg=202wy&pd_rd_i=B07DNXBFQN&pd_rd_w=sxp2z&content-id=amzn1.sym.a4970ca5-d151-4edb-a206-3bc56f60ce74&pd_rd_r=a6ed79d0-8acb-43bc-a77d-d4cfc50527f7&th=1<img width="108" height="17" alt="image" src="https://github.com/user-attachments/assets/625324f0-45f1-4f79-9ef2-46b0d7a2576f" />

Uno kit link: https://www.amazon.com/gp/product/B01D8KOZF4/ref=ox_sc_rp_title_rp_6?smid=&psc=1&pf_rd_p=a4970ca5-d151-4edb-a206-3bc56f60ce74&pf_rd_r=KJCFHYYAF06QVCPK6E6E&pd_rd_wg=3fEMC&pd_rd_i=B01D8KOZF4&pd_rd_w=hep9f&content-id=amzn1.sym.a4970ca5-d151-4edb-a206-3bc56f60ce74&pd_rd_r=430bb664-d08c-4bea-80a0-9d48d6b4ff54<img width="108" height="17" alt="image" src="https://github.com/user-attachments/assets/b3c5b241-d331-4209-a667-ef931f914f4f" />

Sensors + Bracket link: https://www.amazon.com/gp/product/B0D1MDP9V3/ref=ox_sc_rp_title_rp_2?smid=&psc=1&pf_rd_p=a4970ca5-d151-4edb-a206-3bc56f60ce74&pf_rd_r=YNS1HJ23GSKZEBR0GCFC&pd_rd_wg=hVMJK&pd_rd_i=B0D1MDP9V3&pd_rd_w=4stIM&content-id=amzn1.sym.a4970ca5-d151-4edb-a206-3bc56f60ce74&pd_rd_r=ba402348-1a30-4578-91af-9d49e0d238ab<img width="108" height="17" alt="image" src="https://github.com/user-attachments/assets/9859d706-d114-4d57-88a0-b7a01ee8f5f1" />

<img width="1051" height="620" alt="Screenshot 2025-12-08 at 10 23 16 PM" src="https://github.com/user-attachments/assets/94a07b84-27b5-4f83-bdb1-14e77a1bc63e" />

Key: 
{Top motor: Left side splice}
{Bottom motor: Right side splice}
{Right Sensor: Right 45 degrees}
{Middle Sensor: Straight}
{Left Sensor: Left 45 degrees}

![IMG_4088](https://github.com/user-attachments/assets/8de74188-9de0-43ba-a291-789bdc6b54b8)


![IMG_4089(1)](https://github.com/user-attachments/assets/d4865f80-842d-4d89-89e8-48c185a22dc6)
