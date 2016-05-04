v1

- load vehicle model & plant model
- calculate the bounding box of vehicle


7:09 PM 4/25/2013 Thursday

- calculate trajectories for just one turning.
- integrated with simple animation
- bounding box not implemented
- bugs exist when the turning angle is too sharp, like a 150 degrees turning, or 180 degrees U-turn

2:08 PM 6/27/2013 Thursday

- added saving the data to files and read boundary data


v4 	3:16 PM 7/18/2013 Thursday
- support physX
- trianglemesh cooking support
- bugs exist for kinematic actors update in physx

v5	5:57 PM 8/2/2013 Friday
- physX collision detection between vehicle & wall enabled and tested
- small views have been added
- can load boundary data in two ways: 3ds Max model or descrete points, both can be cooked into physx
- a small mfc control dialog added

v6	6:40 PM 8/12/2013 Monday
- crane interactive control added, there is self-locking in reversing.
- added a 1st person view.

v7	7:19 PM 8/23/2013 Friday
- control mechanism has been changed to make it feel more comfortable
- 2 joysticks supported, one for wheel control and another for view control
- still have problems when in animation (stuck occasionly)

v9	12:41 PM 8/27/2013 Tuesday
- Model is AC700
- Front 2 wheels simulation
- Rewrote the turning control to make it soomth without vibrating

v10	2:14 AM 9/2/2013 Monday
- still use centimenter as the unit, next version will use meters
- precipitation added
- fog, and skybox added.

v11	1:09 PM 9/5/2013 Thursday
- use meter as the unit
- new steering wheel and joysticks supported
- 4 driving modes (road driving, all-steering, etc.) implemented