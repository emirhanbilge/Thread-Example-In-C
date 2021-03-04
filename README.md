# Thread-Example-In-C


Goal
 In this assignment, you will solve the synchronization problem using semaphores and mutex. You should suffer from deadlock and/or starvation. You are expected to simulate and solve the “COVID-19 Test Unit” synchronization problem, which its details are given.
 Implementation Details & Requirements 
Today, the units that make Covid-19 tests in medical centers have a very important and critical role. With the increase in cases and patients, this may cause the density in these units after fillation studies. Before the test is applied, people are waiting for a long time at the entrance of the hospitals. Because there is a limited number of test units and healthcare staff. We are launching the “Covid-19 Test Unit” application to facilitate this. This will prevent both people from creating long queues and to lock the waiting rooms before being full. Also it will provide an efficient and fair sharing because units will contain people up to its capacity.
 With your simulation and solution with mutex and semaphores students hopefully will no longer wait so long for working.
 
 
 ● Each waiting room has 3 people.


● People will come to the unit in continuous and random periods.


● The hospital has 8 units and also 8 healthcare staff. So, each unit contains one staff. 


● The states for each unit: ○ Entry free state: Staff will announce to the people his remaining places to get in the waiting room, if it has one or more people in the room. For example, each room has 2 people and waits for one more person and the staff calls “The last people, let's start! Please, pay attention to your social distance and hygiene; use a mask.” ○ Idle (Empty) state: If there are no people in the room, the staff will ventilate the room. Being idle is forbidden for the staff. If any people came, they should open the room. Don't forget that no people are waiting. ○ Full and busy state: If there are 3 people in the room, the room will be in a busy state. 


● The states for each people: ○ Waiting in room: When a person comes at room, she/he gets in the nearly full capacity room. If there are no people in the room, the first person alerts the staff if he is ventilating. And the people prepare for the test and fill out the form until the room’s full. After the room is full, the test process will be applied to them and together they empty the room. ○ Waiting at hospital: If there is no empty room, she/he's gonna wait for a room at the outdoor waiting hole of the hospital.
