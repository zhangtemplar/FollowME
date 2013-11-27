#pragma once
/*
this function stores the necessary configuration of the program
*/
class FollowMEConfig
{
public:
	FollowMEConfig(void);
	~FollowMEConfig(void);
public:
	// the directory for storing the image
	char snap_shoot_dir[255];
	// the length of sequence
	int sequence_length;
	// the threshold for a gesture
	// to make the algorithm robust, we classify each image in the sequence
	// if the number of positives is larger than the threshold, we mark the gesture detected
	int gesture_threshold;
	// the interval between the detection and tracking
	int detection_interval;
	// the threshold for defining active optical flow
	double diff_threshold;
};
